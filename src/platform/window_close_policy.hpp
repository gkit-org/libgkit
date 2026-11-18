#pragma once

#include <optional>
#include <string>

namespace gkit::platform::detail {
    /** @brief Suppress the platform's implicit quit while an application-managed window exists.
     * Acquire and release on the platform thread, around the native window lifetime. */
    class KeepRunningOnWindowClose {
    public:
        KeepRunningOnWindowClose();
        ~KeepRunningOnWindowClose();
        KeepRunningOnWindowClose(const KeepRunningOnWindowClose&)                    = delete;
        auto operator=(const KeepRunningOnWindowClose&) -> KeepRunningOnWindowClose& = delete;

    private:
        std::optional<std::string> previous;
        bool changed           = false;
        bool override_priority = false;
    };
} // namespace gkit::platform::detail
