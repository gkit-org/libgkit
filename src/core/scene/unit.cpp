#include "scene/unit.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <utility>
#include <vector>


gkit::scene::Unit::Unit() noexcept : 
    children(std::vector<std::unique_ptr<Unit>>()),
    active_index_cache(),
    children_rw_mutex() { }
// gkit::scene::Unit::Unit(const Unit&) noexcept {}
// gkit::scene::Unit::Unit(Unit&& other) noexcept : 
//     name(other.name),
//     parent(nullptr),
//     modified(true),
//     active_index_cache(),
//     process_enabled(other.process_enabled.load()),
//     ready_to_drop(other.ready_to_drop.load())
// {
//     for (auto&& child_ptr : other.children) {
//         if (child_ptr == nullptr) continue; 
//         this->children.push_back(std::move(child_ptr));
//     }
//     this->update_index_cache();
// }
// gkit::scene::Unit::Unit(std::string&& name) noexcept : gkit::scene::Unit(name) { }
gkit::scene::Unit::Unit(std::string name) noexcept : gkit::scene::Unit() {
    this->name = name;
}


auto gkit::scene::Unit::_ready()            -> void { }
auto gkit::scene::Unit::_process()          -> void { }
auto gkit::scene::Unit::_physics_process()  -> void { }
auto gkit::scene::Unit::_exit()             -> void { }


auto gkit::scene::Unit::ready_handler() noexcept -> void {
    for (auto child_index : this->active_index_cache) {
        auto& child = this->children[child_index];
        child->ready_handler();
    }
    this->_ready();
}


auto gkit::scene::Unit::process_handler() noexcept -> void {
    this->update_index_cache();
    for (auto child_index : this->active_index_cache) {
        auto& child = this->children[child_index];
        if (child == nullptr || !child->process_enabled) continue;
        child->process_handler();
    }
    this->_process();
    this->drop_children();
}


auto gkit::scene::Unit::physics_process_handler() noexcept -> void {

}


auto gkit::scene::Unit::exit_handler() noexcept -> void {
    for (auto child_index : this->active_index_cache) {
        this->children[child_index]->exit_handler();
    }
    this->_exit();
}


auto gkit::scene::Unit::add_child(std::unique_ptr<Unit>&& child_ptr) noexcept -> void {
    if (child_ptr == nullptr) {
        return;
    }

    child_ptr->_ready();
    {
        std::unique_lock<std::shared_mutex> w_lock(this->children_rw_mutex);
        child_ptr->parent = this;
        children.push_back(std::move(child_ptr));
    }
    this->modified.store(true);
}


auto gkit::scene::Unit::remove_child(uint32_t index) noexcept -> void {
    auto child_opt = this->get_available_child(index);
    if (!child_opt.has_value()) return;
    child_opt.value()->drop();
}


auto gkit::scene::Unit::get_available_child(uint32_t index) noexcept -> std::optional<Unit*> {
    // check modified flag and update cache
    this->update_index_cache();
    std::shared_lock<std::shared_mutex> r_lock(this->children_rw_mutex);

    if (index >= this->active_index_cache.size()) {
        return std::nullopt;
    }
    auto raw_index = this->active_index_cache.at(index);
    return this->children.at(raw_index).get();
}


auto gkit::scene::Unit::update_index_cache() -> void {
    bool need_remap = false;
    // overload factor calculation
    {
        std::shared_lock<std::shared_mutex> r_lock(this->children_rw_mutex);
        std::shared_lock<std::shared_mutex> cache_r_lock(this->index_cache_rw_mutex);

        auto cache_size = this->active_index_cache.size();
        auto children_size = this->children.size();
        if ((float)cache_size / children_size < Unit::overload_factor) {
            need_remap = true;
        }
    }

    // remap children and cache and exit
    if (need_remap) {
        this->remap_children_and_cache();
        return; 
    }

    if (!this->modified) return;
    std::shared_lock<std::shared_mutex> r_lock(this->children_rw_mutex);
    std::unique_lock<std::shared_mutex> w_lock(this->index_cache_rw_mutex); 

    this->active_index_cache.clear();
    for (auto i = 0u; i < this->children.size(); ++i) {
        auto& child_ptr = this->children.at(i);
        if (child_ptr == nullptr) continue;
        this->active_index_cache.push_back(i);
    }

    this->modified = false;
}


auto gkit::scene::Unit::remap_children_and_cache() -> void {
    std::unique_lock<std::shared_mutex> cache_w_lock(this->index_cache_rw_mutex);
    std::unique_lock<std::shared_mutex> children_w_lock(this->children_rw_mutex);
    this->active_index_cache.clear();

    // remap children
    auto new_children_vec = std::vector<std::unique_ptr<Unit>>(0);
    for (auto&& child_ptr : this->children) {
        if (child_ptr == nullptr) continue;
        new_children_vec.emplace_back(std::move(child_ptr));
    }

    this->children = std::move(new_children_vec);
    this->children.shrink_to_fit(); // free some memory

    // update cache
    for (auto i = 0; i < this->children.size(); ++i) {
        this->active_index_cache.emplace_back(i);
    }
}


auto gkit::scene::Unit::drop_children() -> void {
    for (auto& active_index : this->active_index_cache) {
        auto& child_ptr = this->children[active_index];
        if (child_ptr != nullptr && child_ptr->ready_to_drop == true) {
            auto child_temp = std::move(child_ptr);
            child_ptr.reset();
            this->modified = true;
            child_temp->exit_handler();
        }
    }
}


template<>
auto gkit::scene::Unit::get_parent<gkit::scene::Unit>() noexcept -> std::optional<std::reference_wrapper<Unit>> {
    if (parent == nullptr) return std::nullopt;
    return std::ref(*parent);
}
