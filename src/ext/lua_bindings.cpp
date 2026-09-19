// © Joseph Cameron - All Rights Reserved

#include <gdk/input/ext/lua_bindings.h>

#include <gdk/input/exception.h>

#include <magic_enum/magic_enum.hpp>

#include <optional>
#include <string>
#include <variant>

using namespace gdk::input;
using namespace gdk::input::ext;

using jfc::lua::data_table;

namespace {
    constexpr auto VERSION_KEY = "version";
    constexpr auto PLAYER_KEY = "player";
    constexpr auto THRESHOLD_KEY = "threshold";
    constexpr auto ACTIONS_KEY = "actions";
    constexpr auto KIND_KEY = "kind";
    constexpr auto SCALE_KEY = "scale";
    constexpr auto GUID_KEY = "guid";
    constexpr auto INDEX_KEY = "index";
    constexpr auto KEY_KEY = "key";
    constexpr auto BUTTON_KEY = "button";
    constexpr auto AXIS_KEY = "axis";

    template <typename T>
    [[nodiscard]] std::string name_of(const T aValue) {
        return std::string(magic_enum::enum_name(aValue));
    }

    template <typename T>
    [[nodiscard]] std::optional<T> value_of(const std::optional<std::string> &aName) {
        if (!aName) return {};

        return magic_enum::enum_cast<T>(*aName);
    }

    [[nodiscard]] data_table encode(const binding &aBinding) {
        data_table out;

        out.set(KIND_KEY, name_of(aBinding.which));

        switch (aBinding.which) {
            case binding::kind::key:
                out.set(KEY_KEY, name_of(aBinding.key));
            break;

            case binding::kind::mouse_button:
                out.set(BUTTON_KEY, name_of(aBinding.mouseButton));
            break;

            case binding::kind::pointer_axis:
                out.set(AXIS_KEY, name_of(aBinding.pointerAxis));
                out.set(SCALE_KEY, static_cast<double>(aBinding.scale));
            break;

            case binding::kind::gamepad_button:
                out.set(BUTTON_KEY, name_of(aBinding.gamepadButton));
            break;

            case binding::kind::gamepad_axis:
                out.set(AXIS_KEY, name_of(aBinding.gamepadAxis));
                out.set(SCALE_KEY, static_cast<double>(aBinding.scale));
            break;

            case binding::kind::device_button:
                out.set(GUID_KEY, aBinding.guid);
                out.set(INDEX_KEY, static_cast<double>(aBinding.index));
            break;

            case binding::kind::device_axis:
                out.set(GUID_KEY, aBinding.guid);
                out.set(INDEX_KEY, static_cast<double>(aBinding.index));
                out.set(SCALE_KEY, static_cast<double>(aBinding.scale));
            break;
        }

        return out;
    }

    [[nodiscard]] std::optional<binding> decode(const data_table &aEntry) {
        const auto which = value_of<binding::kind>(aEntry.get_string(KIND_KEY));

        if (!which) return {};

        const auto scale = static_cast<binding::value_type>(
            aEntry.get_number(SCALE_KEY).value_or(1));

        switch (*which) {
            case binding::kind::key: {
                const auto key = value_of<keyboard::key>(aEntry.get_string(KEY_KEY));

                if (!key) return {};

                return binding::of(*key);
            }

            case binding::kind::mouse_button: {
                const auto button = value_of<mouse::button>(aEntry.get_string(BUTTON_KEY));

                if (!button) return {};

                return binding::of(*button);
            }

            case binding::kind::pointer_axis: {
                const auto axis = value_of<mouse::axis>(aEntry.get_string(AXIS_KEY));

                if (!axis) return {};

                return binding::of(*axis, scale);
            }

            case binding::kind::gamepad_button: {
                const auto button = value_of<gamepad::button>(aEntry.get_string(BUTTON_KEY));

                if (!button) return {};

                return binding::of(*button);
            }

            case binding::kind::gamepad_axis: {
                const auto axis = value_of<gamepad::axis>(aEntry.get_string(AXIS_KEY));

                if (!axis) return {};

                return binding::of(*axis, scale);
            }

            case binding::kind::device_button: {
                const auto guid = aEntry.get_string(GUID_KEY);
                const auto index = aEntry.get_number(INDEX_KEY);

                if (!guid || !index) return {};

                return binding::of_device_button(*guid,
                    static_cast<gamepad::index_type>(*index));
            }

            case binding::kind::device_axis: {
                const auto guid = aEntry.get_string(GUID_KEY);
                const auto index = aEntry.get_number(INDEX_KEY);

                if (!guid || !index) return {};

                return binding::of_device_axis(*guid,
                    static_cast<gamepad::index_type>(*index), scale);
            }
        }

        return {};
    }
}

data_table gdk::input::ext::to_data_table(const controls &aControls) {
    data_table out;

    out.set(VERSION_KEY, LUA_BINDINGS_VERSION);
    out.set(PLAYER_KEY, static_cast<double>(aControls.player()));
    out.set(THRESHOLD_KEY, static_cast<double>(aControls.threshold()));

    data_table actions;

    for (const auto &action : aControls.actions()) {
        data_table sources;

        double at = 1;

        for (const auto &source : aControls.sources(action)) sources.set(at++, encode(source));

        actions.set(action, sources);
    }

    out.set(ACTIONS_KEY, actions);

    return out;
}

std::size_t gdk::input::ext::from_data_table(controls &aControls, const data_table &aTable) {
    const auto version = aTable.get_number(VERSION_KEY);

    if (!version)
        throw exception("bindings table has no version; it is not a gdk-input bindings file");

    if (*version > LUA_BINDINGS_VERSION)
        throw exception("bindings table is version " + std::to_string(static_cast<int>(*version))
            + ", which is newer than this build understands ("
            + std::to_string(static_cast<int>(LUA_BINDINGS_VERSION)) + ")");

    const auto pActions = aTable.get_data_table(ACTIONS_KEY);

    if (!pActions) throw exception("bindings table has no actions table");

    for (const auto &action : aControls.actions()) aControls.unbind(action);

    if (const auto threshold = aTable.get_number(THRESHOLD_KEY))
        aControls.set_threshold(static_cast<controls::value_type>(*threshold));

    std::size_t skipped = 0;

    for (const auto &key : pActions->keys()) {
        if (!key.is_string()) { ++skipped; continue; }

        const auto pSources = pActions->get_data_table(key);

        if (!pSources) { ++skipped; continue; }

        for (const auto &at : pSources->keys()) {
            const auto pEntry = pSources->get_data_table(at);

            if (!pEntry) { ++skipped; continue; }

            if (const auto decoded = decode(*pEntry)) aControls.bind(std::get<std::string>(key.value()), *decoded);
            else ++skipped;
        }
    }

    return skipped;
}

std::string gdk::input::ext::to_string(const controls &aControls) {
    return to_data_table(aControls).to_string();
}

std::size_t gdk::input::ext::from_string(controls &aControls, const std::string &aText) {
    return from_data_table(aControls, data_table::from_string(aText));
}
