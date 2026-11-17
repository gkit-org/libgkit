#include "gkit/core/reflect/serdebase.hpp"

#include "core/reflect/seralize.hpp"
#include "gkit/core/value.hpp"
#include "seralize.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace gkit::core::reflect {

    namespace {

        auto render_node(const SerdeNode& node, const SerdeBase& base) -> std::string {
            // ObjectId values do not participate in (de)serialization for now.
            if (node.get_type() == Type::ObjectId) {
                return {};
            }

            auto res = std::string();

            auto wrap = base.wrapper(node.get_type(), node.get_key());
            res += wrap.first;

            switch (node.get_type()) {
            case Type::Array:
            case Type::Map:
            case Type::Object: {
                const auto& children = node.get_children();
                for (auto it = children.cbegin(); it != children.cend(); ++it) {
                    res += render_node(**it, base);
                    if (it + 1 != children.cend()) {
                        res += base.element_gap(node.get_type());
                    }
                }
                break;
            }

            default: {
                res += base.leaf_value(node.get_value());
                break;
            }
            }

            res += wrap.second;
            return res;
        }

    } // namespace

    struct SerdeBase::SerdeData {
        SerdeStruct ss{};
    };

    SerdeBase::SerdeBase() : data(std::make_unique<SerdeData>()) {}
    SerdeBase::~SerdeBase() {}

    auto SerdeBase::from(const ObjectId& id) -> void {
        this->data->ss.from(id);
    }

    auto SerdeBase::from(const Value& v) -> void {
        this->data->ss.from(v);
    }

    auto SerdeBase::to_string() const -> std::string {
        auto& ss = this->data->ss;
        if (!ss.available()) {
            return {};
        }

        return render_node(ss.root(), *this);
    }

} // namespace gkit::core::reflect
