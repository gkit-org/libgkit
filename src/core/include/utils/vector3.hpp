#pragma once

#include <cstdint>
#include <cmath>
#include <tuple>

namespace gkit::utils {
    class Vector3 final {
    public:
        Vector3() noexcept = default;
        Vector3(float x, float y, float z)    noexcept;
        Vector3(const Vector3& other)  noexcept;
        Vector3(const Vector3&& other) noexcept;

    public: // Arithmetic operators
        inline auto operator=(const Vector3& other)  noexcept -> Vector3& { this->x = other.x; this->y = other.y; this->z = other.z; return *this;};
        inline auto operator=(const Vector3&& other) noexcept -> Vector3& { this->x = other.x; this->y = other.y; this->z = other.z; return *this;}
        inline auto operator==(const Vector3& other) noexcept -> bool { return (this->x == other.x) && (this->y == other.y) && (this->z == other.z);}
        inline auto operator!=(const Vector3& other) noexcept -> bool { return (this->x != other.x) || (this->y != other.y) || (this->z != other.z);}
        inline auto operator+(const Vector3& other)  noexcept -> Vector3 { return Vector3(x + other.x, y + other.y, z + other.z); }
        inline auto operator-(const Vector3& other)  noexcept -> Vector3 { return Vector3(x - other.x, y - other.y, z - other.z);}
        inline auto operator+=(const Vector3& other) noexcept -> const Vector3& { this->x += other.x; this->y += other.y; this->z += other.z; return *this; }
        inline auto operator-=(const Vector3& other) noexcept -> const Vector3& { this->x -= other.x; this->y -= other.y; this->z -= other.z; return *this; }
        inline auto operator*(const int32_t n)  noexcept -> Vector3 { return Vector3(x * n, y * n, z * n);}
        inline auto operator/(const int32_t n)  noexcept -> Vector3 { return Vector3(x / n, y / n, z / n);}
        inline auto operator*=(const int32_t n) noexcept -> const Vector3& { this->x *= n; this->y *= n; this->z *= n; return *this;}
        inline auto operator/=(const int32_t n) noexcept -> const Vector3& { this->x /= n; this->y /= n; this->z /= n; return *this;}

    public: 
        static auto zero() -> const Vector3&;
        auto normalization() -> void;
        inline constexpr auto length() const -> float { return std::sqrt(x * x + y * y + z * z); }
        inline auto properties() -> auto { return std::tie(x, y, z);}

    private:
        float x = 0.f, 
              y = 0.f,
              z = 0.f;
    }; // class Vector3
} // namespace gkit::utils