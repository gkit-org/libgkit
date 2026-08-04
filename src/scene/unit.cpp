#include "gkit/scene/unit.hpp"
#include "gkit/core/object_id.hpp"

#include <cstdint>
#include <functional>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

namespace gkit::scene {
    auto Unit::regist_method() -> void {
        auto& db = core::reflect::ClassDB::instance();
        db.regist<Unit>("Unit").add_field("Unit", "name", &Unit::name);
    }

    core::reflect::RegistHolder Unit::register_holder([]() {
        Unit::regist_method();
        return;
    });

    Unit::Unit(std::string&& name) noexcept : Unit() {
        this->name = name;
    }

    auto Unit::ready() -> void {}
    auto Unit::process() -> void {}
    auto Unit::physics_process() -> void {}
    auto Unit::exit() -> void {}

    auto Unit::ready_handler() noexcept -> void {
        std::unique_lock<std::mutex> lock(this->children_mutex);
        for (auto& child_p : this->children) {
            auto child_ptr =dynamic_cast<scene::Unit*>(child_p.get());
            if (child_ptr == nullptr) continue;
            child_ptr->ready_handler();
        }
        this->ready();
    }

    auto Unit::process_handler() noexcept -> void {
        std::unique_lock<std::mutex> lock(this->children_mutex);
        for (auto& child_p : this->children) {
            auto child_ptr =dynamic_cast<scene::Unit*>(child_p.get());
            if (child_ptr == nullptr || !child_ptr->process_enabled) continue;
            child_ptr->process_handler();
        }
        this->process();
        this->drop_children();
    }

    auto Unit::physics_process_handler() noexcept -> void {}

    auto Unit::exit_handler() noexcept -> void {
        std::unique_lock<std::mutex> lock(this->children_mutex);
        for (auto& child_p : this->children) {
            auto child_ptr =dynamic_cast<scene::Unit*>(child_p.get());
            if (child_ptr == nullptr) continue;
            child_ptr->exit_handler();
        }
        this->exit();
    }

    auto Unit::add_child(core::UniqueObject&& child_p) -> void {
        auto child_ptr = dynamic_cast<scene::Unit*>(child_p.get());
        if (child_ptr == nullptr) {
            throw std::invalid_argument("child_ptr is nullptr");
        }

        if (child_ptr->name.empty()) {
            throw std::invalid_argument("child_ptr name is empty");
        }

        child_ptr->ready();
        {
            std::unique_lock<std::mutex> lock(this->children_mutex);
            if (this->name_map_cache.contains(child_ptr->name)) {
                throw std::invalid_argument("child_ptr name is already exist");
            }
            child_ptr->parent = this->get_object_id();
            this->children.push_back(std::move(child_p));
            this->name_map_cache.emplace(child_ptr->name, this->children.back().get_id());
        }
    }

    auto Unit::remove_child(uint32_t index) noexcept -> void {
        auto child_ptr = dynamic_cast<scene::Unit*>(core::ObjectPool::instance().deref_from(this->get_child(index)));
        if (child_ptr == nullptr) return;
        child_ptr->ready_to_drop();
    }

    auto Unit::remove_child(const std::string& child_name) noexcept -> void {
        auto child_ptr = dynamic_cast<scene::Unit*>(core::ObjectPool::instance().deref_from(this->get_child(child_name)));
        if (child_ptr == nullptr) return;
        child_ptr->ready_to_drop();
    }

    auto Unit::get_child(uint32_t index) noexcept -> core::ObjectId {
        std::unique_lock<std::mutex> lock(this->children_mutex);

        if (index >= this->children.size()) {
            return core::ObjectId{};
        }
        return this->children[index].get_id();
    }

    auto Unit::get_child(const std::string& child_name) noexcept -> core::ObjectId {
        std::unique_lock<std::mutex> lock(this->children_mutex);
        auto iter = this->name_map_cache.find(child_name);
        if (iter == this->name_map_cache.end()) {
            return core::ObjectId{};
        }
        return iter->second;
    }

    auto Unit::drop_children() -> void {
        std::vector<core::UniqueObject> to_exit;
        to_exit.reserve(this->children.size() / 2);

        std::erase_if(this->children, [&](core::UniqueObject& p_origin) -> bool {
            auto p = dynamic_cast<scene::Unit*>(p_origin.get());
            if (p && p->drop_flag.load() == true) {
                {
                    std::unique_lock<std::mutex> lock(this->children_mutex);
                    this->name_map_cache.erase(p->name);
                }
                to_exit.push_back(std::move(p_origin));
                return true;
            }
            return false;
        });

        for (auto& ch : to_exit) {
            auto child = dynamic_cast<scene::Unit*>(ch.get());
            child->exit_handler();
        }
    }

    template<>
    auto Unit::get_parent<Unit>() noexcept -> std::optional<std::reference_wrapper<Unit>> {
        if (parent == core::ObjectId{}) return std::nullopt;
        auto* parent_ptr = dynamic_cast<Unit*>(core::ObjectPool::instance().deref_from(parent));
        if (parent_ptr == nullptr) return std::nullopt;
        return std::ref(*parent_ptr);
    }

    // UnitIterator

    template<bool IsConst>
    Unit::UnitIterator<IsConst>::UnitIterator(const core::ObjectId owner, size_t pos) : m_owner(owner), m_pos(pos) {}

    template<bool IsConst>
    auto Unit::UnitIterator<IsConst>::operator*() const -> reference {
        auto* owner   = dynamic_cast<Unit*>(core::ObjectPool::instance().deref_from(m_owner));
        return owner->get_child(static_cast<uint32_t>(m_pos));
    }

    template<bool IsConst>
    auto Unit::UnitIterator<IsConst>::operator->() const -> pointer {
        auto* owner = dynamic_cast<Unit*>(core::ObjectPool::instance().deref_from(m_owner));
        return owner->get_child(static_cast<uint32_t>(m_pos));
    }

    template<bool IsConst>
    auto Unit::UnitIterator<IsConst>::operator++() -> UnitIterator& {
        ++m_pos;
        return *this;
    }

    template<bool IsConst>
    auto Unit::UnitIterator<IsConst>::operator++(int) -> UnitIterator {
        UnitIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    template<bool IsConst>
    auto Unit::UnitIterator<IsConst>::operator--() -> UnitIterator& {
        --m_pos;
        return *this;
    }

    template<bool IsConst>
    auto Unit::UnitIterator<IsConst>::operator--(int) -> UnitIterator {
        UnitIterator tmp = *this;
        --(*this);
        return tmp;
    }

    template<bool IsConst>
    auto Unit::UnitIterator<IsConst>::operator==(const UnitIterator& other) const -> bool {
        return m_owner == other.m_owner && m_pos == other.m_pos;
    }

    template class Unit::UnitIterator<true>;
    template class Unit::UnitIterator<false>;

    auto Unit::begin() -> iterator {
        return iterator(this->get_object_id(), 0);
    }
    auto Unit::end() -> iterator {
        return iterator(this->get_object_id(), children.size());
    }
    auto Unit::begin() const -> const_iterator {
        return const_iterator(this->get_object_id(), 0);
    }
    auto Unit::end() const -> const_iterator {
        return const_iterator(this->get_object_id(), children.size());
    }
    auto Unit::cbegin() const -> const_iterator {
        return begin();
    }
    auto Unit::cend() const -> const_iterator {
        return end();
    }

    auto Unit::rbegin() -> reverse_iterator {
        return reverse_iterator(end());
    }
    auto Unit::rend() -> reverse_iterator {
        return reverse_iterator(begin());
    }
    auto Unit::rbegin() const -> const_reverse_iterator {
        return const_reverse_iterator(end());
    }
    auto Unit::rend() const -> const_reverse_iterator {
        return const_reverse_iterator(begin());
    }
    auto Unit::crbegin() const -> const_reverse_iterator {
        return rbegin();
    }
    auto Unit::crend() const -> const_reverse_iterator {
        return rend();
    }

} // namespace gkit::scene
