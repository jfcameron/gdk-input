// © Joseph Cameron - MIT License

#include <gdk/input/context.h>
#include <gdk/input/controls.h>
#include <gdk/input/exception.h>

#include <algorithm>
#include <cmath>
#include <string_view>
#include <vector>

using namespace gdk::input;

controls::controls(context_ptr_type aContext, const std::size_t aPlayer)
: mpContext(std::move(aContext))
, mPlayer(aPlayer)
{}

std::shared_ptr<controls> controls::make(context_ptr_type aContext, const std::size_t aPlayer) {
    return std::shared_ptr<controls>(new controls(std::move(aContext), aPlayer));
}

std::size_t controls::player() const { return mPlayer; }

context::gamepad_ptr controls::pad() const {
    return mpContext ? mpContext->get_gamepad(mPlayer) : nullptr;
}

void controls::bind(const std::string &aAction, const binding &aBinding) {
    if (aBinding.which == binding::kind::key && is_modifier(aBinding.key))
        throw exception("\"" + aAction + "\": a modifier key is held with another key, never bound alone");

    mBindings[aAction].push_back(aBinding);
}

void controls::bind(const std::string &aAction, const keyboard::key aKey) {
    bind(aAction, binding::of(aKey));
}

void controls::bind(const std::string &aAction, const mouse::button aButton) {
    bind(aAction, binding::of(aButton));
}

void controls::bind(const std::string &aAction, const mouse::axis aAxis,
    const value_type aScale) {
    bind(aAction, binding::of(aAxis, aScale));
}

void controls::bind(const std::string &aAction, const gamepad::button aButton) {
    bind(aAction, binding::of(aButton));
}

void controls::bind(const std::string &aAction, const gamepad::axis aAxis, const value_type aScale) {
    bind(aAction, binding::of(aAxis, aScale));
}

void controls::bind_device_button(const std::string &aAction, const std::string &aGuid,
    const gamepad::index_type aIndex) {
    bind(aAction, binding::of_device_button(aGuid, aIndex));
}

void controls::bind_device_axis(const std::string &aAction, const std::string &aGuid,
    const gamepad::index_type aIndex, const value_type aScale) {
    bind(aAction, binding::of_device_axis(aGuid, aIndex, aScale));
}

void controls::declare(const std::string &aAction) { mBindings.try_emplace(aAction); }

void controls::unbind(const std::string &aAction) { mBindings.erase(aAction); }

bool controls::unbind(std::string_view aAction, const binding &aBinding) {
    const auto found = mBindings.find(aAction);

    if (found == mBindings.end()) return false;

    auto &sources = found->second;

    const auto at = std::find(sources.begin(), sources.end(), aBinding);

    if (at == sources.end()) return false;

    sources.erase(at);

    if (sources.empty()) mBindings.erase(found);

    return true;
}

std::vector<std::string> controls::actions() const {
    std::vector<std::string> out;

    out.reserve(mBindings.size());

    for (const auto &pair : mBindings) out.push_back(pair.first);

    return out;
}

std::size_t controls::source_count(std::string_view aAction) const {
    const auto *pSources = sources_for(aAction);

    return pSources ? pSources->size() : 0;
}

std::vector<binding> controls::sources(std::string_view aAction) const {
    const auto *pSources = sources_for(aAction);

    return pSources ? *pSources : std::vector<binding>{};
}

const std::vector<binding> *controls::sources_for(std::string_view aAction) const {
    const auto found = mBindings.find(aAction);

    return found == mBindings.end() ? nullptr : &found->second;
}

text::modifiers controls::modifiers() const { return mpContext ? mpContext->modifiers_held() : text::modifiers{}; }

controls::value_type controls::threshold() const { return mThreshold; }

void controls::set_threshold(const value_type aThreshold) { mThreshold = aThreshold; }

namespace {
    struct reading final {
        controls::value_type value{0};

        bool now{false};
        bool before{false};
        bool started{false};
        bool ended{false};
    };

    [[nodiscard]] bool addresses(const context::gamepad_ptr &apGamepad, const binding &aBinding) {
        return apGamepad && apGamepad->guid() == aBinding.guid;
    }

    [[nodiscard]] bool same_control(const binding &a, const binding &b) {
        if (a.which != b.which) return false;

        return a.which == binding::kind::key ? a.key == b.key : a.mouseButton == b.mouseButton;
    }

    [[nodiscard]] reading button_reading(const bool aDown, const bool aJustPressed, const bool aJustReleased) {
        reading out;

        out.value = aDown ? 1.f : 0.f;
        out.now = aDown;
        out.started = aJustPressed;
        out.ended = aJustReleased;
        out.before = !aJustPressed && (aDown || aJustReleased);

        return out;
    }

    [[nodiscard]] reading axis_reading(const controls::value_type aNow, const controls::value_type aBefore,
        const controls::value_type aThreshold) {
        reading out;

        out.value = aNow;
        out.now = aNow >= aThreshold;
        out.before = aBefore >= aThreshold;
        out.started = out.now && !out.before;
        out.ended = out.before && !out.now;

        return out;
    }

    [[nodiscard]] controls::value_type pointer_value(const context &aContext, const binding &aBinding) {
        const auto delta = aContext.mouse_delta();
        const auto scroll = aContext.mouse_scroll_delta();

        const auto raw = [&]() -> double {
            switch (aBinding.pointerAxis) {
                case mouse::axis::x: return delta.x;
                case mouse::axis::y: return delta.y;
                case mouse::axis::scroll_x: return scroll.x;
                case mouse::axis::scroll_y: return scroll.y;
            }

            return 0;
        }();

        const auto scaled = static_cast<controls::value_type>(raw) * aBinding.scale;

        const auto isWheel = aBinding.pointerAxis == mouse::axis::scroll_x
            || aBinding.pointerAxis == mouse::axis::scroll_y;

        if (isWheel) return scaled;

        return std::max(controls::value_type{-1}, std::min(controls::value_type{1}, scaled));
    }

    [[nodiscard]] reading read(const context &aContext, const context::gamepad_ptr &apGamepad,
        const binding &aSource, const controls::value_type aThreshold) {
        switch (aSource.which) {
            case binding::kind::key:
                return button_reading(aContext.key_down(aSource.key), aContext.key_just_pressed(aSource.key),
                    aContext.key_just_released(aSource.key));

            case binding::kind::mouse_button:
                return button_reading(aContext.mouse_button_down(aSource.mouseButton),
                    aContext.mouse_button_just_pressed(aSource.mouseButton),
                    aContext.mouse_button_just_released(aSource.mouseButton));

            case binding::kind::pointer_axis: {
                reading out;

                out.value = pointer_value(aContext, aSource);
                out.now = out.value >= aThreshold;

                return out;
            }

            case binding::kind::gamepad_button:
                if (!apGamepad) return {};

                return button_reading(apGamepad->button_down(aSource.gamepadButton),
                    apGamepad->button_just_pressed(aSource.gamepadButton),
                    apGamepad->button_just_released(aSource.gamepadButton));

            case binding::kind::gamepad_axis:
                if (!apGamepad) return {};

                return axis_reading(apGamepad->axis_value(aSource.gamepadAxis, aThreshold) * aSource.scale,
                    apGamepad->previous_axis_value(aSource.gamepadAxis, aThreshold) * aSource.scale, aThreshold);

            case binding::kind::device_button:
                if (!addresses(apGamepad, aSource)) return {};

                return button_reading(apGamepad->button_down(aSource.index),
                    apGamepad->button_just_pressed(aSource.index), apGamepad->button_just_released(aSource.index));

            case binding::kind::device_axis:
                if (!addresses(apGamepad, aSource)) return {};

                return axis_reading(apGamepad->axis_value(aSource.index, aThreshold) * aSource.scale,
                    apGamepad->previous_axis_value(aSource.index, aThreshold) * aSource.scale, aThreshold);
        }

        return {};
    }

    [[nodiscard]] std::vector<reading> read_all(const context &aContext, const context::gamepad_ptr &apGamepad,
        const std::vector<binding> &aSources, const controls::value_type aThreshold) {
        std::vector<reading> out;

        out.reserve(aSources.size());

        for (const auto &source : aSources) out.push_back(read(aContext, apGamepad, source, aThreshold));

        return out;
    }

    template <typename predicate_type>
    [[nodiscard]] bool any(const std::vector<reading> &aReadings, const predicate_type aPredicate) {
        return std::any_of(aReadings.begin(), aReadings.end(), aPredicate);
    }
}

bool controls::modifiers_allow(const binding &aBinding) const {
    if (aBinding.which != binding::kind::key && aBinding.which != binding::kind::mouse_button) return true;

    const auto held = modifiers();

    if (aBinding.is_chord()) return aBinding.with == held;

    if (held == text::modifiers{}) return true;

    for (const auto &[action, sources] : mBindings)
        for (const auto &each : sources)
            if (each.is_chord() && each.with == held && same_control(each, aBinding)) return false;

    return true;
}

std::vector<binding> controls::allowed(const std::vector<binding> &aSources, const bool aEnding) const {
    std::vector<binding> out;

    for (const auto &each : aSources)
        if ((aEnding && each.is_chord()) || modifiers_allow(each)) out.push_back(each);

    return out;
}

controls::value_type controls::get(std::string_view aAction) const {
    const auto *pSources = sources_for(aAction);

    if (!pSources || !mpContext) return 0;

    value_type strongest = 0;

    for (const auto &r : read_all(*mpContext, pad(), allowed(*pSources, false), mThreshold))
        if (std::abs(r.value) > std::abs(strongest)) strongest = r.value;

    return strongest;
}

bool controls::down(std::string_view aAction) const {
    const auto *pSources = sources_for(aAction);

    if (!pSources || !mpContext) return false;

    return any(read_all(*mpContext, pad(), allowed(*pSources, false), mThreshold),
        [](const reading &r) { return r.now; });
}

bool controls::just_pressed(std::string_view aAction) const {
    const auto *pSources = sources_for(aAction);

    if (!pSources || !mpContext) return false;

    const auto all = read_all(*mpContext, pad(), allowed(*pSources, false), mThreshold);

    return any(all, [](const reading &r) { return r.started; }) && !any(all, [](const reading &r) { return r.before; });
}

bool controls::just_released(std::string_view aAction) const {
    const auto *pSources = sources_for(aAction);

    if (!pSources || !mpContext) return false;

    const auto all = read_all(*mpContext, pad(), allowed(*pSources, true), mThreshold);

    return any(all, [](const reading &r) { return r.ended; }) && !any(all, [](const reading &r) { return r.now; });
}
