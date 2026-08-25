// © Joseph Cameron - All Rights Reserved

#include <gdk/input/context.h>
#include <gdk/input/controls.h>

#include <algorithm>
#include <cmath>

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
    mBindings[aAction].push_back(aBinding);
}

void controls::bind(const std::string &aAction, const keyboard::key aKey) {
    bind(aAction, binding::of(aKey));
}

void controls::bind(const std::string &aAction, const mouse::button aButton) {
    bind(aAction, binding::of(aButton));
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

void controls::unbind(const std::string &aAction) { mBindings.erase(aAction); }

bool controls::unbind(const std::string &aAction, const binding &aBinding) {
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

std::size_t controls::source_count(const std::string &aAction) const {
    const auto *pSources = sources_for(aAction);

    return pSources ? pSources->size() : 0;
}

std::vector<binding> controls::sources(const std::string &aAction) const {
    const auto *pSources = sources_for(aAction);

    return pSources ? *pSources : std::vector<binding>{};
}

const std::vector<binding> *controls::sources_for(const std::string &aAction) const {
    const auto found = mBindings.find(aAction);

    return found == mBindings.end() ? nullptr : &found->second;
}

controls::value_type controls::threshold() const { return mThreshold; }

void controls::set_threshold(const value_type aThreshold) { mThreshold = aThreshold; }

namespace {
    [[nodiscard]] bool addresses(const context::gamepad_ptr &apGamepad, const binding &aBinding) {
        return apGamepad && apGamepad->guid() == aBinding.guid;
    }
}

controls::value_type controls::get(const std::string &aAction) const {
    const auto *pSources = sources_for(aAction);

    if (!pSources || !mpContext) return 0;

    const auto pGamepad = pad();

    value_type strongest = 0;

    const auto consider = [&strongest](const value_type aValue) {
        if (std::abs(aValue) > std::abs(strongest)) strongest = aValue;
    };

    for (const auto &s : *pSources) {
        switch (s.which) {
            case binding::kind::key:
                if (mpContext->key_down(s.key)) consider(1);
            break;

            case binding::kind::mouse_button:
                if (mpContext->mouse_button_down(s.mouseButton)) consider(1);
            break;

            case binding::kind::gamepad_button:
                if (pGamepad && pGamepad->button_down(s.gamepadButton)) consider(1);
            break;

            case binding::kind::gamepad_axis:
                if (pGamepad) consider(pGamepad->axis_value(s.gamepadAxis, mThreshold) * s.scale);
            break;

            case binding::kind::device_button:
                if (addresses(pGamepad, s) && pGamepad->button_down(s.index)) consider(1);
            break;

            case binding::kind::device_axis:
                if (addresses(pGamepad, s))
                    consider(pGamepad->axis_value(s.index, mThreshold) * s.scale);
            break;
        }
    }

    return strongest;
}

bool controls::down(const std::string &aAction) const {
    return get(aAction) >= mThreshold;
}

bool controls::just_pressed(const std::string &aAction) const {
    const auto *pSources = sources_for(aAction);

    if (!pSources || !mpContext) return false;

    const auto pGamepad = pad();

    const auto axis_began = [&](const value_type aNow, const value_type aScale) {
        return aNow * aScale > 0;
    };

    for (const auto &s : *pSources) {
        switch (s.which) {
            case binding::kind::key:
                if (mpContext->key_just_pressed(s.key)) return true;
            break;

            case binding::kind::mouse_button:
                if (mpContext->mouse_button_just_pressed(s.mouseButton)) return true;
            break;

            case binding::kind::gamepad_button:
                if (pGamepad && pGamepad->button_just_pressed(s.gamepadButton)) return true;
            break;

            case binding::kind::gamepad_axis:
                if (pGamepad && pGamepad->axis_just_exceeded_threshold(s.gamepadAxis, mThreshold)
                    && axis_began(pGamepad->axis_value(s.gamepadAxis, mThreshold), s.scale))
                    return true;
            break;

            case binding::kind::device_button:
                if (addresses(pGamepad, s) && pGamepad->button_just_pressed(s.index)) return true;
            break;

            case binding::kind::device_axis:
                if (addresses(pGamepad, s)
                    && pGamepad->axis_just_exceeded_threshold(s.index, mThreshold)
                    && axis_began(pGamepad->axis_value(s.index, mThreshold), s.scale))
                    return true;
            break;
        }
    }

    return false;
}

bool controls::just_released(const std::string &aAction) const {
    const auto *pSources = sources_for(aAction);

    if (!pSources || !mpContext) return false;

    const auto pGamepad = pad();

    const auto axis_ended = [&](const value_type aBefore, const value_type aScale) {
        return aBefore * aScale > 0;
    };

    for (const auto &s : *pSources) {
        switch (s.which) {
            case binding::kind::key:
                if (mpContext->key_just_released(s.key)) return true;
            break;

            case binding::kind::mouse_button:
                if (mpContext->mouse_button_just_released(s.mouseButton)) return true;
            break;

            case binding::kind::gamepad_button:
                if (pGamepad && pGamepad->button_just_released(s.gamepadButton)) return true;
            break;

            case binding::kind::gamepad_axis:
                if (pGamepad && pGamepad->axis_just_dropped_below_threshold(s.gamepadAxis, mThreshold)
                    && axis_ended(pGamepad->previous_axis_value(s.gamepadAxis, mThreshold), s.scale))
                    return true;
            break;

            case binding::kind::device_button:
                if (addresses(pGamepad, s) && pGamepad->button_just_released(s.index)) return true;
            break;

            case binding::kind::device_axis:
                if (addresses(pGamepad, s)
                    && pGamepad->axis_just_dropped_below_threshold(s.index, mThreshold)
                    && axis_ended(pGamepad->previous_axis_value(s.index, mThreshold), s.scale))
                    return true;
            break;
        }
    }

    return false;
}
