// © Joseph Cameron - All Rights Reserved

#include <gdk/input/null_gamepad.h>

#include <gdk/input/impl_gamepad_mappings.h>

#include <cmath>

using namespace gdk::input;

bool null_gamepad::connected() const { return mConnected; }

std::string_view null_gamepad::name() const { return mName; }

std::string_view null_gamepad::guid() const { return mGuid; }

void null_gamepad::set_port(const std::size_t aPort) { mLastPort = aPort; }

std::optional<std::size_t> null_gamepad::last_port() const { return mLastPort; }

bool null_gamepad::just_connected() const { return mJustConnected; }

bool null_gamepad::just_disconnected() const { return mJustDisconnected; }

bool null_gamepad::has_standard_mapping() const { return mConnected && mHasStandardMapping; }

std::size_t null_gamepad::button_count() const { return mButtons.size(); }

std::size_t null_gamepad::axis_count() const { return mAxes.size(); }

std::size_t null_gamepad::hat_count() const { return mHats.size(); }

gamepad::hat_state_type null_gamepad::hat(const index_type index) const {
    if (index >= mHats.size()) return {};

    return mHats[index];
}

void null_gamepad::attach_unmapped(const std::string &aName, const std::string &aGuid,
    const std::size_t aButtonCount, const std::size_t aAxisCount, const std::size_t aHatCount) {
    attach(aName, aGuid, aButtonCount, aAxisCount);

    mHasStandardMapping = false;
    mHats.assign(aHatCount, hat_state_type{});
}

void null_gamepad::set_hat(const index_type index, const hat_state_type aState) {
    if (index < mHats.size()) mHats[index] = aState;
}

void null_gamepad::set_standard_mapping(const bool aMapped) {
    if (mHasStandardMapping == aMapped) return;

    mHasStandardMapping = aMapped;

    if (aMapped) rest_triggers();
}

void null_gamepad::rest_triggers() {
    for (std::size_t i = 0; i < mPending.size(); ++i)
        if (is_standard_trigger(i)) mPending[i] = -1.f;
}

void null_gamepad::attach(const std::string &aName, const std::string &aGuid,
    const std::size_t aButtonCount, const std::size_t aAxisCount) {
    mConnected = true;
    mName = aName;
    mGuid = aGuid;
    mHasStandardMapping = true;
    mJustConnected = true;
    mJustDisconnected = false;
    mHats.clear();

    mButtons.assign(aButtonCount, button_state::up);
    mHeld.assign(aButtonCount, false);
    mPending.assign(aAxisCount, 0);
    mAxes.assign(aAxisCount, 0);
    mLastAxes.assign(aAxisCount, 0);

    if (mHasStandardMapping) rest_triggers();
}

void null_gamepad::detach() {
    const bool wasConnected = mConnected;

    mConnected = false;
    mJustConnected = false;
    mJustDisconnected = wasConnected;
    mName.clear();

    mButtons.clear();
    mHeld.clear();
    mPending.clear();
    mAxes.clear();
    mLastAxes.clear();
    mHats.clear();
    mHasStandardMapping = false;
}

void null_gamepad::press(const index_type index) {
    if (index < mHeld.size()) mHeld[index] = true;
}

void null_gamepad::release(const index_type index) {
    if (index < mHeld.size()) mHeld[index] = false;
}

void null_gamepad::set_axis(const index_type index, const axis_value_type value) {
    if (index < mPending.size()) mPending[index] = value;
}

void null_gamepad::update() {
    const bool wasJustConnected = mJustConnected;
    const bool wasJustDisconnected = mJustDisconnected;

    if (!mConnected) {
        if (wasJustDisconnected) mJustDisconnected = false;
        return;
    }

    if (wasJustConnected) mJustConnected = false;

    mLastAxes = mAxes;
    mAxes = mPending;

    if (mHasStandardMapping)
        for (std::size_t i = 0; i < mAxes.size(); ++i)
            if (is_standard_trigger(i)) mAxes[i] = standard_trigger_value(mAxes[i]);

    for (std::size_t i = 0; i < mButtons.size(); ++i) {
        const bool held = i < mHeld.size() && mHeld[i];

        switch (mButtons[i]) {
            case button_state::up:
                mButtons[i] = held ? button_state::just_pressed : button_state::up;
            break;

            case button_state::just_pressed:
                mButtons[i] = held ? button_state::held_down : button_state::just_released;
            break;

            case button_state::held_down:
                mButtons[i] = held ? button_state::held_down : button_state::just_released;
            break;

            case button_state::just_released:
                mButtons[i] = held ? button_state::just_pressed : button_state::up;
            break;
        }
    }
}

null_gamepad::axis_value_type null_gamepad::axis_value(const index_type index,
    const axis_value_type threshold) const {
    if (index >= mAxes.size()) return 0;

    const auto value = mAxes[index];

    if (std::abs(value) < threshold) return 0;

    return value;
}

null_gamepad::axis_value_type null_gamepad::previous_axis_value(const index_type index,
    const axis_value_type threshold) const {
    if (index >= mLastAxes.size()) return 0;

    const auto value = mLastAxes[index];

    if (std::abs(value) < threshold) return 0;

    return value;
}

bool null_gamepad::axis_just_exceeded_threshold(const index_type index,
    const axis_value_type threshold) const {
    if (index >= mAxes.size()) return false;

    const auto last = index < mLastAxes.size() ? mLastAxes[index] : 0;

    return std::abs(mAxes[index]) >= threshold && std::abs(last) < threshold;
}

bool null_gamepad::axis_just_dropped_below_threshold(const index_type index,
    const axis_value_type threshold) const {
    if (index >= mAxes.size()) return false;

    const auto last = index < mLastAxes.size() ? mLastAxes[index] : 0;

    return std::abs(mAxes[index]) < threshold && std::abs(last) >= threshold;
}

std::optional<std::pair<null_gamepad::index_type, null_gamepad::axis_value_type>>
null_gamepad::any_axis_down(axis_value_type threshold) const {
    for (std::size_t i = 0; i < mAxes.size(); ++i)
        if (const auto value = axis_value(static_cast<index_type>(i), threshold))
            return {{static_cast<index_type>(i), value}};

    return {};
}

bool null_gamepad::button_down(const index_type index) const {
    if (index >= mButtons.size()) return false;

    return mButtons[index] == button_state::held_down
        || mButtons[index] == button_state::just_pressed;
}

bool null_gamepad::button_just_pressed(const index_type index) const {
    if (index >= mButtons.size()) return false;

    return mButtons[index] == button_state::just_pressed;
}

bool null_gamepad::button_just_released(const index_type index) const {
    if (index >= mButtons.size()) return false;

    return mButtons[index] == button_state::just_released;
}

std::optional<null_gamepad::button_collection_type::size_type> null_gamepad::any_button_down() const {
    for (std::size_t i = 0; i < mButtons.size(); ++i)
        if (mButtons[i] != button_state::up) return i;

    return {};
}
