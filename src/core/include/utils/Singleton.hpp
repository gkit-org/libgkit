#pragma once

namespace gkit {

template<typename T>
class Singleton {
    friend T;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;

    Singleton(const Singleton&)  = delete;
    Singleton(const Singleton&&) = delete;
    Singleton operator=(const Singleton&)  = delete;
    Singleton operator=(const Singleton&&) = delete;

public:
    inline static T& instance() noexcept {
        static T instance;
        return instance;
    }
}; // class Singleton

} // namespace gkit