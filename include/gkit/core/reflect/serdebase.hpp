#pragma once

#include "gkit/core/object_id.hpp"
#include "gkit/core/value.hpp"

#include <memory>
#include <string>
#include <utility>

namespace gkit::core::reflect {

    /**
     * @brief Format-agnostic serializer base.
     *
     * Builds a private intermediate tree from reflection data, then renders the
     * tree through pure-virtual hooks. Derived classes implement a target format
     * (Json / Xml / Toml ...) by overriding the wrapper / gap / leaf hooks.
     */
    class SerdeBase {
        struct SerdeData;
        std::unique_ptr<SerdeData> data;

    public:
        using WrapperPair = std::pair<std::string, std::string>;

        SerdeBase();
        virtual ~SerdeBase();

        /**
         * @brief Serialize an object to serdedata
         * @throw @ref std::invalid_argument, if objectid is invalid
         */
        auto from(const ObjectId& id) -> void;

        /**
         * @brief Serialize a value to serdedata
         * @throw @ref std::invalid_argument, if value is null.
         */
        auto from(const Value& v) -> void;

        /**
         * @brief Serialize serdedata to string.
         * @return Empty string when the SerdeBase cannot be built.
         */
        [[nodiscard]] auto to_string() const -> std::string;

        // =====================================================================
        // Format hooks (pure virtual)
        // =====================================================================

        /** @brief Begin/end delimiters around a node (keyed or unkeyed). */
        virtual auto wrapper(Type type, const std::string& key) const -> WrapperPair = 0;

        /** @brief Separator between sibling elements of a container. */
        virtual auto element_gap(Type type) const -> std::string = 0;

        /** @brief Encode a leaf value into the target format. */
        virtual auto leaf_value(const Value& v) const -> std::string = 0;
    };

} // namespace gkit::core::reflect
