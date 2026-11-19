#pragma once

#include "singleton.hpp"

#include <cstdint>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace gkit::core::templates {
    /**
     * @brief Generic ID value type with a generation counter.
     *
     * An id is identified by the pair (id, gen). The generation is bumped on
     * every recycle so a stale reference from a previous incarnation of the id
     * never aliases a newly allocated one (prevents the ABA problem).
     *
     * @tparam T tag type used only to give each id flavor a distinct type.
     */
    template<class T>
    class GenId {
        std::uint32_t id  = 0u;
        std::uint32_t gen = 0u;

    public:
        class IdAllocator;

        constexpr GenId() noexcept = default;
        ~GenId()                   = default;

        GenId(const GenId<T>& other) noexcept                       = default;
        GenId(GenId<T>&& other) noexcept                            = default;
        auto operator=(const GenId<T>& other) noexcept -> GenId<T>& = default;
        auto operator=(GenId<T>&& other) noexcept -> GenId<T>&      = default;

        /**
         * @brief check the id is valid
         * @return bool - both id and generation are non-zero.
         */
        [[nodiscard]] constexpr auto available() const noexcept -> bool;

        /**
         * @brief the raw id number
         */
        [[nodiscard]] constexpr auto get_id() const noexcept -> std::uint32_t;

        /**
         * @brief the generation counter of the id
         */
        [[nodiscard]] constexpr auto get_generation() const noexcept -> std::uint32_t;

        friend constexpr auto operator==(const GenId<T>& lhs, const GenId<T>& rhs) noexcept -> bool {
            return lhs.id == rhs.id && lhs.gen == rhs.gen;
        }

        friend constexpr auto operator!=(const GenId<T>& lhs, const GenId<T>& rhs) noexcept -> bool {
            return !(lhs == rhs);
        }
    }; // class GenId

    template<class T>
    class GenId<T>::IdAllocator : public Singleton<GenId<T>::IdAllocator> {
        friend Singleton<GenId<T>::IdAllocator>;

        struct Slot {
            std::uint32_t generation = 1;
            std::uint32_t next       = 0;
            bool live                = true;
        };

        std::mutex alloc_mutex;
        std::vector<Slot> slots;
        std::uint32_t free_head = 0;

        IdAllocator() = default;

    public:
        auto new_one() -> GenId<T>;

        /** @brief Recycle a live generation without allocating; stale drops are ignored. */
        auto drop(const GenId<T>& id) -> void;
    };

    template<class T>
    constexpr auto GenId<T>::available() const noexcept -> bool {
        return this->id != 0 && this->gen != 0;
    }

    template<class T>
    constexpr auto GenId<T>::get_id() const noexcept -> std::uint32_t {
        return this->id;
    }

    template<class T>
    constexpr auto GenId<T>::get_generation() const noexcept -> std::uint32_t {
        return this->gen;
    }

    template<class T>
    auto GenId<T>::IdAllocator::new_one() -> GenId<T> {
        std::scoped_lock lock(this->alloc_mutex);
        GenId<T> result;
        if (this->free_head != 0) {
            result.id       = this->free_head;
            auto& slot      = this->slots[result.id - 1];
            this->free_head = slot.next;
            slot.live       = true;
            result.gen      = slot.generation;
        } else {
            if (this->slots.size() == UINT32_MAX) throw std::overflow_error("ID space exhausted");
            this->slots.emplace_back();
            result.id  = static_cast<std::uint32_t>(this->slots.size());
            result.gen = 1;
        }
        return result;
    }

    template<class T>
    auto GenId<T>::IdAllocator::drop(const GenId<T>& id) -> void {
        std::scoped_lock lock(this->alloc_mutex);
        if (!id.available() || id.id > this->slots.size()) return;
        auto& slot = this->slots[id.id - 1];
        if (!slot.live || slot.generation != id.gen) return;
        slot.live = false;
        if (slot.generation == UINT32_MAX) return; // Retire instead of wrapping.
        ++slot.generation;
        slot.next       = this->free_head;
        this->free_head = id.id;
    }

} // namespace gkit::core::templates

template<class T>
struct std::hash<gkit::core::templates::GenId<T>> {
    auto operator()(const gkit::core::templates::GenId<T>& obj_id) const -> std::size_t {
        auto uint_hash = std::hash<std::uint32_t>();
        return uint_hash(obj_id.get_id()) ^ (uint_hash(obj_id.get_generation()) << 1);
    }
};
