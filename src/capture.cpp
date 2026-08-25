// © Joseph Cameron - All Rights Reserved

#include <gdk/input/context.h>
#include <gdk/input/capture.h>

#include <algorithm>
#include <cmath>

using namespace gdk::input;

capture::capture(context_ptr_type aContext, const std::size_t aPlayer)
: mpContext(std::move(aContext))
, mPlayer(aPlayer) {
    mPrevious = observe();
}

std::shared_ptr<capture> capture::make(context_ptr_type aContext, const std::size_t aPlayer) {
    return std::shared_ptr<capture>(new capture(std::move(aContext), aPlayer));
}

context::gamepad_ptr capture::pad() const {
    return mpContext ? mpContext->get_gamepad(mPlayer) : nullptr;
}

capture::value_type capture::threshold() const { return mThreshold; }

void capture::set_threshold(const value_type aThreshold) { mThreshold = aThreshold; }

bool capture::accepts_mouse() const { return mAcceptsMouse; }

void capture::set_accepts_mouse(const bool aAccepts) { mAcceptsMouse = aAccepts; }

bool capture::captured() const { return mResult.has_value(); }

std::optional<binding> capture::result() const { return mResult; }

void capture::reset() {
    mResult.reset();
    mPrevious = observe();
}

capture::observation capture::observe() const {
    observation out;

    if (!mpContext) return out;

    out.keys = mpContext->keys_down();
    out.mouseButtons = mpContext->mouse_buttons_down();

    if (const auto pGamepad = pad(); pGamepad && pGamepad->connected()) {
        out.guid = pGamepad->guid();

        for (std::size_t i = 0; i < pGamepad->button_count(); ++i) {
            const auto index = static_cast<gamepad::index_type>(i);

            if (pGamepad->button_down(index)) out.gamepadButtons.push_back(index);
        }

        for (std::size_t i = 0; i < pGamepad->axis_count(); ++i) {
            const auto index = static_cast<gamepad::index_type>(i);

            if (const auto value = pGamepad->axis_value(index, mThreshold))
                out.gamepadAxes.push_back({index, value});
        }
    }

    return out;
}

namespace {
    template <typename T>
    [[nodiscard]] std::optional<T> arrived(const std::vector<T> &aNow, const std::vector<T> &aBefore) {
        for (const auto &candidate : aNow)
            if (std::find(aBefore.begin(), aBefore.end(), candidate) == aBefore.end())
                return candidate;

        return {};
    }

    [[nodiscard]] std::optional<std::pair<gamepad::index_type, capture::value_type>> arrived_axis(
        const std::vector<std::pair<gamepad::index_type, capture::value_type>> &aNow,
        const std::vector<std::pair<gamepad::index_type, capture::value_type>> &aBefore) {
        for (const auto &candidate : aNow) {
            const auto held = std::any_of(aBefore.begin(), aBefore.end(),
                [&](const auto &aOld) { return aOld.first == candidate.first; });

            if (!held) return candidate;
        }

        return {};
    }
}

std::optional<binding> capture::answer_from(const observation &aNow) const {
    if (const auto key = arrived(aNow.keys, mPrevious.keys)) return binding::of(*key);

    if (mAcceptsMouse)
        if (const auto button = arrived(aNow.mouseButtons, mPrevious.mouseButtons))
            return binding::of(*button);

    const auto pGamepad = pad();

    if (!pGamepad) return {};

    if (aNow.guid != mPrevious.guid) return {};

    const auto mapped = pGamepad->has_standard_mapping();

    if (const auto button = arrived(aNow.gamepadButtons, mPrevious.gamepadButtons)) {
        if (mapped && *button <= static_cast<gamepad::index_type>(gamepad::button::dpad_left))
            return binding::of(static_cast<gamepad::button>(*button));

        return binding::of_device_button(aNow.guid, *button);
    }

    if (const auto axis = arrived_axis(aNow.gamepadAxes, mPrevious.gamepadAxes)) {
        const value_type scale = axis->second < 0 ? -1.f : 1.f;

        if (mapped && axis->first <= static_cast<gamepad::index_type>(gamepad::axis::right_trigger))
            return binding::of(static_cast<gamepad::axis>(axis->first), scale);

        return binding::of_device_axis(aNow.guid, axis->first, scale);
    }

    return {};
}

std::optional<binding> capture::poll() {
    if (mResult) return mResult;

    const auto now = observe();

    mResult = answer_from(now);

    mPrevious = now;

    return mResult;
}
