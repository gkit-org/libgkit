#include "gkit/core/processer.hpp"
#include "gkit/scene/scene.hpp"
#include "test_utils.hpp"

#include <SDL3/SDL.h>

#include <barrier>
#include <cmath>
#include <future>
#include <thread>

using namespace gkit;
using namespace gkit::core;
namespace {
    auto check(bool condition) -> void {
        if (!condition) throw std::runtime_error("processer regression failed");
    }
    struct App : Processer {
        mutable int time = 0;
        int frames       = 0;
        int limit        = 2;
        std::function<void(const FrameContext&)> before, after;
        App() {
            auto value              = settings();
            value.initialize_sdl    = false;
            value.fixed_delta       = 0.125;
            value.max_frame_delta   = 1;
            value.max_fixed_steps   = 2;
            value.target_frame_rate = 0;
            configure(value);
        }
        auto now() const -> std::chrono::steady_clock::time_point override {
            return std::chrono::steady_clock::time_point{} + std::chrono::seconds(time++);
        }
        auto before_frame(const FrameContext& context) -> void override {
            if (before) before(context);
        }
        auto after_frame(const FrameContext& context) -> void override {
            if (after) after(context);
            if (++frames >= limit) stop();
        }
    };
    struct Probe : Node {
        std::vector<std::string>& events;
        std::string label;
        int ready_count = 0, entered = 0, exited = 0, processes = 0, physics = 0;
        Probe(std::vector<std::string>& events, std::string label) : events(events), label(std::move(label)) {}
        auto enter_tree() -> void override {
            ++entered;
            events.push_back(label + "+");
        }
        auto ready() -> void override {
            ++ready_count;
            events.push_back(label + "!");
        }
        auto exit_tree() -> void override {
            ++exited;
            events.push_back(label + "-");
        }
        auto process(const FrameContext& context) -> void override {
            check(context.delta == 1);
            ++processes;
            events.push_back(label);
        }
        auto physics_process(const FrameContext& context) -> void override {
            check(context.delta == 0.125);
            ++physics;
        }
    };
    auto test_lifecycle_and_clock() -> bool {
        std::vector<std::string> events;
        App app;
        auto parent_owner = NodeOwner<Probe>::create(events, "parent");
        auto parent       = parent_owner.handle();
        auto child_owner  = NodeOwner<Probe>::create(events, "child");
        auto child        = child_owner.handle();
        parent.with([&](Node& node) {
            node.edit_children([&](NodeEditor& e) {
                e.add(parent, "child", std::move(child_owner));
            });
        });
        app.root().with([&](Node& node) {
            node.edit_children([&](NodeEditor& e) {
                e.add(app.root(), "parent", std::move(parent_owner));
            });
        });
        app.run();
        check(app.dropped_physics_steps() == 12);
        check(events ==
              std::vector<std::string>{
                  "parent+", "child+", "child!", "parent!", "parent", "child", "parent", "child", "child-", "parent-"});
        parent.with([](Probe& node) {
            check(node.physics == 4 && node.ready_count == 1 && node.exited == 1);
        });
        app.frames = 0;
        app.run();
        child.with([](Probe& node) {
            check(node.entered == 2 && node.ready_count == 1 && node.exited == 2);
        });
        return true;
    }
    auto test_deferred_scene_edits() -> bool {
        App app;
        app.limit        = 4;
        auto scene_owner = NodeOwner<scene::Scene>::create();
        auto scene       = scene_owner.handle();
        std::vector<std::string> events;
        auto child_owner = NodeOwner<Probe>::create(events, "child");
        auto child       = child_owner.handle();
        scene.with([&](Node& node) {
            node.edit_children([&](NodeEditor& e) {
                e.add(scene, "child", std::move(child_owner));
            });
        });
        app.root().with([&](Node& node) {
            node.edit_children([&](NodeEditor& e) {
                e.add(app.root(), "scene", std::move(scene_owner));
            });
        });
        std::optional<NodeOwner<>> detached;
        app.after = [&](const FrameContext& context) {
            if (context.frame == 0)
                scene.with([](scene::Scene& value) {
                    value.pause();
                });
            if (context.frame == 1)
                app.root().with([&](Node& node) {
                    node.edit_children(
                        [&](NodeEditor& e) {
                            e.detach(scene);
                        },
                        [&](EditResult result) {
                            check(result.error == NodeError::None);
                            detached.emplace(std::move(result.owners.at(0)));
                        });
                    check(node.children().size() == 1);
                });
            if (context.frame == 2) {
                check(app.root().children().empty() && detached.has_value());
                scene.with([](scene::Scene& value) {
                    value.resume();
                });
                app.root().with([&](Node& node) {
                    node.edit_children([&](NodeEditor& e) {
                        e.add(app.root(), "scene", std::move(*detached));
                    });
                });
            }
        };
        app.run();
        child.with([](Probe& node) {
            check(node.processes == 2 && node.entered == 2 && node.exited == 2 && node.ready_count == 1);
        });
        return true;
    }
    struct ParallelNode : Node {
        std::barrier<>& barrier;
        NodeHandle<ParallelNode> peer;
        int posts = 0;
        int calls = 0;
        explicit ParallelNode(std::barrier<>& barrier) : barrier(barrier) { set_parallel(true); }
        auto process(const FrameContext&) -> void override {
            ++calls;
            bool denied = false;
            try {
                peer.with([](auto&) {});
            } catch (const std::logic_error&) {
                denied = true;
            }
            check(denied);
            check(peer.post([](ParallelNode& other) {
                ++other.posts;
            }));
            barrier.arrive_and_wait();
        }
    };
    auto test_parallel_domains() -> bool {
        App app;
        auto config                    = app.settings();
        config.parallel_update_enabled = true;
        config.worker_count            = 2;
        app.configure(config);
        std::barrier barrier(2);
        auto left_owner  = NodeOwner<ParallelNode>::create(barrier);
        auto right_owner = NodeOwner<ParallelNode>::create(barrier);
        auto left = left_owner.handle(), right = right_owner.handle();
        left.with([&](auto& node) {
            node.peer = right;
        });
        right.with([&](auto& node) {
            node.peer = left;
        });
        app.root().with([&](Node& node) {
            node.edit_children([&](NodeEditor& e) {
                e.add(app.root(), "left", std::move(left_owner));
                e.add(app.root(), "right", std::move(right_owner));
            });
        });
        app.before = [&](const auto&) {
            bool rejected = false;
            try {
                app.configure(config);
            } catch (const std::logic_error&) {
                rejected = true;
            }
            check(rejected);
        };
        app.run();
        left.with([](auto& node) {
            check(node.calls == 2 && node.posts == 1);
        });
        right.with([](auto& node) {
            check(node.calls == 2 && node.posts == 1);
        });
        return true;
    }
    struct ThrowingNode : Node {
        int exits = 0;
        auto process(const FrameContext&) -> void override { throw std::runtime_error("update failure"); }
        auto exit_tree() -> void override { ++exits; }
    };
    auto test_exception_cleanup() -> bool {
        App app;
        auto owner  = NodeOwner<ThrowingNode>::create();
        auto handle = owner.handle();
        app.root().with([&](Node& node) {
            node.edit_children([&](NodeEditor& e) {
                e.add(app.root(), "failure", std::move(owner));
            });
        });
        bool failed = false;
        try {
            app.run();
        } catch (const std::runtime_error& error) {
            failed = std::string(error.what()) == "update failure";
        }
        check(failed);
        handle.with([](auto& node) {
            check(node.exits == 1);
        });
        return true;
    }
    struct EditingNode : Node {
        NodeHandle<> created;
        int entered = 0;
        auto ready() -> void override {
            edit_children([&](NodeEditor& edit) {
                created = edit.emplace(handle(), "during_ready");
            });
        }
        auto enter_tree() -> void override { ++entered; }
        auto process(const FrameContext&) -> void override {
            check(children().find("during_ready").has_value());
            edit_children([&](NodeEditor& edit) {
                edit.rename(created, "during_ready");
            });
        }
    };
    auto test_reentrant_edits_and_expired_posts() -> bool {
        App app;
        auto owner  = NodeOwner<EditingNode>::create();
        auto handle = owner.handle();
        app.root().with([&](Node& node) {
            node.edit_children([&](NodeEditor& edit) {
                edit.add(app.root(), "editor", std::move(owner));
            });
        });
        NodeHandle<> victim;
        app.root().with([&](Node& node) {
            node.edit_children([&](NodeEditor& edit) {
                victim = edit.emplace(app.root(), "victim");
            });
        });
        int completions = 0;
        app.after       = [&](const FrameContext& context) {
            if (context.frame == 0) {
                check(victim.post(
                    [](auto&) {
                        throw std::runtime_error("expired callback must not execute");
                    },
                    [&](NodeError error) {
                        check(error == NodeError::Expired);
                        ++completions;
                    }));
                app.root().with([&](Node& node) {
                    node.edit_children([&](NodeEditor& edit) {
                        edit.destroy(victim);
                    });
                });
            }
            if (context.frame == 1) {
                check(handle.post([](auto&) {},
                                  [&](NodeError error) {
                                      check(error == NodeError::Stopped);
                                      ++completions;
                                  }));
            }
        };
        app.run();
        check(completions == 2);
        handle.with([](auto& node) {
            check(node.entered == 1 && node.children().size() == 1);
        });
        return true;
    }
    auto test_worker_exception_and_nested_domains() -> bool {
        App app;
        auto settings                    = app.settings();
        settings.parallel_update_enabled = true;
        settings.worker_count            = 1;
        app.configure(settings);
        auto owner    = NodeOwner<>::create();
        auto handle   = owner.handle();
        auto throwing = NodeOwner<ThrowingNode>::create();
        auto nested   = throwing.handle();
        nested.with([](auto& node) {
            node.set_parallel(true);
        });
        handle.with([&](Node& node) {
            node.set_parallel(true);
            node.edit_children([&](NodeEditor& edit) {
                edit.add(handle, "nested", std::move(throwing));
            });
        });
        app.root().with([&](Node& node) {
            node.edit_children([&](NodeEditor& edit) {
                edit.add(app.root(), "parallel", std::move(owner));
            });
        });
        bool failed = false;
        try {
            app.run();
        } catch (const std::runtime_error&) {
            failed = true;
        }
        check(failed);
        nested.with([](auto& node) {
            check(node.exits == 1);
        });
        return true;
    }
    struct WaitingApp : Processer {
        std::promise<void> waiting;
        auto before_frame(const FrameContext&) -> void override { waiting.set_value(); }
    };
    auto test_cross_thread_stop() -> bool {
        WaitingApp app;
        auto settings              = app.settings();
        settings.initialize_sdl    = false;
        settings.target_frame_rate = 0.01; // Would sleep 100 seconds without stop notification.
        app.configure(settings);
        auto started = app.waiting.get_future();
        std::thread stopper([&] {
            started.wait();
            app.stop();
        });
        app.run();
        stopper.join();
        return true;
    }
    auto test_sdl_lifetime() -> bool {
        check(SDL_InitSubSystem(SDL_INIT_EVENTS));
        App app;
        auto settings           = app.settings();
        settings.initialize_sdl = true;
        settings.sdl_flags      = SDL_INIT_EVENTS;
        app.configure(settings);
        for (int i = 0; i < 2; ++i) {
            SDL_Event quit{};
            quit.type = SDL_EVENT_QUIT;
            check(SDL_PushEvent(&quit));
            app.run();
            check(SDL_WasInit(SDL_INIT_EVENTS) & SDL_INIT_EVENTS);
        }
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
        return true;
    }

    struct RejectingNode : Node {
        int& returned;
        explicit RejectingNode(int& returned) : returned(returned) { set_parallel(true); }
        auto process(const FrameContext& context) -> void override {
            if (context.frame != 0) return;
            edit_children(
                [&](NodeEditor& edit) {
                    edit.emplace(handle(), "duplicate");
                    edit.emplace(handle(), "duplicate");
                },
                [&](EditResult result) {
                    check(result.error == NodeError::InvalidEdit && result.owners.size() == 2);
                    for (auto& owner : result.owners) check(owner.with([](Node&) {}));
                    ++returned;
                });
        }
    };
    auto test_returned_ownership_and_runtime_expiry() -> bool {
        int returned = 0;
        App app;
        auto settings                    = app.settings();
        settings.parallel_update_enabled = true;
        settings.worker_count            = 1;
        app.configure(settings);
        auto rejecting = NodeOwner<RejectingNode>::create(returned);
        NodeHandle<> victim;
        app.root().with([&](Node& node) {
            node.edit_children([&](NodeEditor& edit) {
                edit.add(app.root(), "rejecting", std::move(rejecting));
                victim = edit.emplace(app.root(), "victim");
            });
        });
        int expired = 0;
        std::optional<NodeOwner<>> detached;
        app.after = [&](const FrameContext& context) {
            if (context.frame != 0) return;
            app.root().with([&](Node& node) {
                node.edit_children(
                    [&](NodeEditor& edit) {
                        edit.detach(victim);
                    },
                    [&](EditResult result) {
                        detached.emplace(std::move(result.owners.at(0)));
                    });
            });
            victim.with([&](Node& node) {
                node.edit_children(
                    [&](NodeEditor& edit) {
                        edit.emplace(victim, "late");
                    },
                    [&](EditResult result) {
                        check(result.error == NodeError::Expired && result.owners.size() == 1);
                        ++expired;
                    });
            });
            check(victim.post(
                [](auto&) {
                    throw std::runtime_error("detached target must not receive old posts");
                },
                [&](NodeError error) {
                    check(error == NodeError::Expired);
                    ++expired;
                }));
        };
        app.run();
        check(returned == 1 && expired == 2 && detached.has_value());
        check(victim.children().empty());
        return true;
    }

} // namespace
int main() {
    test::TestRunner()
        .add_test_func(test_lifecycle_and_clock)
        .add_test_func(test_deferred_scene_edits)
        .add_test_func(test_parallel_domains)
        .add_test_func(test_exception_cleanup)
        .add_test_func(test_reentrant_edits_and_expired_posts)
        .add_test_func(test_worker_exception_and_nested_domains)
        .add_test_func(test_cross_thread_stop)
        .add_test_func(test_sdl_lifetime)
        .add_test_func(test_returned_ownership_and_runtime_expiry)
        .run();
}
