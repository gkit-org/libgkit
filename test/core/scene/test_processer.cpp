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
            set_initialize_sdl(false)
                .set_fixed_delta(0.125)
                .set_max_frame_delta(1)
                .set_max_fixed_steps(2)
                .set_target_frame_rate(0);
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
    auto test_configuration() -> bool {
        App app;
        app.set_fixed_delta(-1);
        try {
            app.run();
            return false;
        } catch (const std::invalid_argument&) {
        }
        check(app.frames == 0);
        app.set_fixed_delta(0.125);
        app.before = [&](const auto&) {
            app.set_worker_count(0).set_parallel_update_enabled(true);
        };
        app.run();
        try {
            app.run();
            return false;
        } catch (const std::invalid_argument&) {
        }
        return true;
    }
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
        auto parent_owner = UniqueObject::create<Probe>(events, "parent");
        auto parent       = parent_owner.get_id();
        auto child_owner  = UniqueObject::create<Probe>(events, "child");
        auto child        = child_owner.get_id();
        parent.with<Node>([&](Node& node) {
            node.add_child("child", std::move(child_owner));
        });
        app.root().add_child("parent", std::move(parent_owner));
        app.run();
        check(app.dropped_physics_steps() == 12);
        check(events ==
              std::vector<std::string>{
                  "parent+", "child+", "child!", "parent!", "parent", "child", "parent", "child", "child-", "parent-"});
        parent.with<Probe>([](Probe& node) {
            check(node.physics == 4 && node.ready_count == 1 && node.exited == 1);
        });
        app.frames = 0;
        app.run();
        child.with<Probe>([](Probe& node) {
            check(node.entered == 2 && node.ready_count == 1 && node.exited == 2);
        });
        return true;
    }
    auto test_deferred_scene_edits() -> bool {
        App app;
        app.limit        = 4;
        auto scene_owner = UniqueObject::create<scene::Scene>();
        auto scene       = scene_owner.get_id();
        std::vector<std::string> events;
        auto child_owner = UniqueObject::create<Probe>(events, "child");
        auto child       = child_owner.get_id();
        scene.with<Node>([&](Node& node) {
            node.add_child("child", std::move(child_owner));
        });
        app.root().add_child("scene", std::move(scene_owner));
        std::optional<UniqueObject> detached;
        app.after = [&](const FrameContext& context) {
            if (context.frame == 0)
                scene.with<scene::Scene>([](scene::Scene& value) {
                    value.pause();
                });
            if (context.frame == 1) {
                app.root().detach_child(scene, [&](EditResult result) {
                    check(result.error == NodeError::None);
                    detached = std::move(result.owner);
                });
                check(app.root().children().size() == 1);
            }
            if (context.frame == 2) {
                check(app.root().children().empty() && detached.has_value());
                scene.with<scene::Scene>([](scene::Scene& value) {
                    value.resume();
                });
                app.root().add_child("scene", std::move(*detached));
            }
        };
        app.run();
        child.with<Probe>([](Probe& node) {
            check(node.processes == 2 && node.entered == 2 && node.exited == 2 && node.ready_count == 1);
        });
        return true;
    }
    struct ParallelNode : Node {
        std::barrier<>& barrier;
        ObjectId peer;
        int posts = 0;
        int calls = 0;
        explicit ParallelNode(std::barrier<>& barrier) : barrier(barrier) { set_parallel(true); }
        auto process(const FrameContext&) -> void override {
            ++calls;
            check(peer.with<ParallelNode>([](ParallelNode& node) {
                check(node.post([](Node& target) {
                    ++static_cast<ParallelNode&>(target).posts;
                }));
            }));
            barrier.arrive_and_wait();
        }
    };
    auto test_parallel_domains() -> bool {
        App app;
        app.set_parallel_update_enabled(true);
        app.set_worker_count(2);
        std::barrier barrier(2);
        auto left_owner  = UniqueObject::create<ParallelNode>(barrier);
        auto right_owner = UniqueObject::create<ParallelNode>(barrier);
        auto left = left_owner.get_id(), right = right_owner.get_id();
        left.with<ParallelNode>([&](auto& node) {
            node.peer = right;
        });
        right.with<ParallelNode>([&](auto& node) {
            node.peer = left;
        });
        app.root().add_child("left", std::move(left_owner));
        app.root().add_child("right", std::move(right_owner));
        app.before = [&](const auto&) {
            app.set_parallel_update_enabled(false);
        };
        app.run();
        left.with<ParallelNode>([](auto& node) {
            check(node.calls == 2 && node.posts == 1);
        });
        right.with<ParallelNode>([](auto& node) {
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
        auto owner  = UniqueObject::create<ThrowingNode>();
        auto handle = owner.get_id();
        app.root().add_child("failure", std::move(owner));
        bool failed = false;
        try {
            app.run();
        } catch (const std::runtime_error& error) {
            failed = std::string(error.what()) == "update failure";
        }
        check(failed);
        handle.with<ThrowingNode>([](auto& node) {
            check(node.exits == 1);
        });
        return true;
    }
    struct EditingNode : Node {
        ObjectId created;
        int entered = 0;
        auto ready() -> void override {
            auto owner = UniqueObject::create<Node>();
            created    = owner.get_id();
            add_child("during_ready", std::move(owner));
        }
        auto enter_tree() -> void override { ++entered; }
        auto process(const FrameContext&) -> void override {
            check(find_child("during_ready").has_value());
            created.with<Node>([](Node& child) {
                child.rename("during_ready");
            });
        }
    };
    auto test_reentrant_edits_and_expired_posts() -> bool {
        App app;
        auto owner  = UniqueObject::create<EditingNode>();
        auto handle = owner.get_id();
        app.root().add_child("editor", std::move(owner));
        ObjectId victim;
        app.root().add_child("victim", UniqueObject::create<Node>());
        victim          = *app.root().find_child("victim");
        int completions = 0;
        app.after       = [&](const FrameContext& context) {
            if (context.frame == 0) {
                victim.with<Node>([&](Node& target) {
                    check(target.post(
                        [](auto&) {
                            throw std::runtime_error("expired callback must not execute");
                        },
                        [&](NodeError error) {
                            check(error == NodeError::Expired);
                            ++completions;
                        }));
                });
                app.root().remove_child(victim);
            }
            if (context.frame == 1) {
                handle.with<Node>([&](Node& node) {
                    check(node.post([](auto&) {},
                                    [&](NodeError error) {
                                        check(error == NodeError::Stopped);
                                        ++completions;
                                    }));
                });
            }
        };
        app.run();
        check(completions == 2);
        handle.with<EditingNode>([](auto& node) {
            check(node.entered == 1 && node.children().size() == 1);
        });
        return true;
    }
    auto test_worker_exception_and_nested_domains() -> bool {
        App app;
        app.set_parallel_update_enabled(true);
        app.set_worker_count(1);
        auto owner    = UniqueObject::create<Node>();
        auto handle   = owner.get_id();
        auto throwing = UniqueObject::create<ThrowingNode>();
        auto nested   = throwing.get_id();
        nested.with<ThrowingNode>([](auto& node) {
            node.set_parallel(true);
        });
        handle.with<Node>([&](Node& node) {
            node.set_parallel(true);
            node.add_child("nested", std::move(throwing));
        });
        app.root().add_child("parallel", std::move(owner));
        bool failed = false;
        try {
            app.run();
        } catch (const std::runtime_error&) {
            failed = true;
        }
        check(failed);
        nested.with<ThrowingNode>([](auto& node) {
            check(node.exits == 1);
        });
        return true;
    }
    auto test_concurrent_edit_cancellation() -> bool {
        App app;
        std::optional<UniqueObject> returned;
        app.after = [&](const FrameContext& frame) {
            if (frame.frame == 0) {
                std::thread writer([&] {
                    app.root().add_child("first", UniqueObject::create<Node>());
                });
                writer.join();
                check(!app.root().find_child("first"));
            } else {
                check(app.root().find_child("first").has_value());
                app.root().add_child("late", UniqueObject::create<Node>(), [&](EditResult result) {
                    check(result.error == NodeError::Stopped);
                    returned = std::move(result.owner);
                });
            }
        };
        app.run();
        return returned && returned->get() && !app.root().find_child("late");
    }
    struct WaitingApp : Processer {
        std::promise<void> waiting;
        auto before_frame(const FrameContext&) -> void override { waiting.set_value(); }
    };
    auto test_cross_thread_stop() -> bool {
        WaitingApp app;
        app.set_initialize_sdl(false);
        app.set_target_frame_rate(0.01); // Would sleep 100 seconds without stop notification.
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
        app.set_initialize_sdl(true);
        app.set_sdl_flags(SDL_INIT_EVENTS);
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
            add_child("duplicate", UniqueObject::create<Node>());
            add_child("duplicate", UniqueObject::create<Node>(), [&](EditResult result) {
                check(result.error == NodeError::InvalidEdit && result.owner && result.owner->get());
                ++returned;
            });
        }
    };
    auto test_returned_ownership_and_runtime_expiry() -> bool {
        int returned = 0;
        App app;
        app.set_parallel_update_enabled(true);
        app.set_worker_count(1);
        auto rejecting = UniqueObject::create<RejectingNode>(returned);
        ObjectId victim;
        app.root().add_child("rejecting", std::move(rejecting));
        app.root().add_child("victim", UniqueObject::create<Node>());
        victim      = *app.root().find_child("victim");
        int expired = 0;
        std::optional<UniqueObject> detached;
        app.after = [&](const FrameContext& context) {
            if (context.frame != 0) return;
            app.root().detach_child(victim, [&](EditResult result) {
                detached = std::move(result.owner);
            });
            victim.with<Node>([&](Node& node) {
                node.add_child("late", UniqueObject::create<Node>(), [&](EditResult result) {
                    check(result.error == NodeError::Expired && result.owner.has_value());
                    ++expired;
                });
            });
            victim.with<Node>([&](Node& target) {
                check(target.post(
                    [](auto&) {
                        throw std::runtime_error("detached target must not receive old posts");
                    },
                    [&](NodeError error) {
                        check(error == NodeError::Expired);
                        ++expired;
                    }));
            });
        };
        app.run();
        check(returned == 1 && expired == 2 && detached.has_value());
        check(victim.with<Node>([](Node& node) {
            check(node.children().empty());
        }));
        return true;
    }

} // namespace
int main() {
    test::TestRunner()
        .add_test_func(test_configuration)
        .add_test_func(test_lifecycle_and_clock)
        .add_test_func(test_deferred_scene_edits)
        .add_test_func(test_parallel_domains)
        .add_test_func(test_exception_cleanup)
        .add_test_func(test_reentrant_edits_and_expired_posts)
        .add_test_func(test_worker_exception_and_nested_domains)
        .add_test_func(test_concurrent_edit_cancellation)
        .add_test_func(test_cross_thread_stop)
        .add_test_func(test_sdl_lifetime)
        .add_test_func(test_returned_ownership_and_runtime_expiry)
        .run();
}
