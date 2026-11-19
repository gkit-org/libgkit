#pragma once

namespace gkit {
    class Processer;
}

namespace gkit::core::templates {
    template<typename T>
    class Singleton {
        friend T;

    protected:
        Singleton()          = default;
        virtual ~Singleton() = default;

    public:
        Singleton(const Singleton&)                    = delete;
        Singleton(const Singleton&&)                   = delete;
        auto operator=(const Singleton&) -> Singleton  = delete;
        auto operator=(const Singleton&&) -> Singleton = delete;

        static auto instance() -> T&;
    }; // class Singleton

    template<typename T>
    auto Singleton<T>::instance() -> T& {
        static T instance;
        return instance;
    }

} // namespace gkit::core::templates
