// © Joseph Cameron - All Rights Reserved

#include <gdk/input/null_context.h>

#include <gdk/input/impl_gamepad_mappings.h>

#include <gdk/input/impl_slot_assignment.h>

#include <cmath>

using namespace gdk::input;

null_context::null_context() {
    for (auto &pGamepad : mGamepads) pGamepad = std::make_shared<null_gamepad>();
}

std::shared_ptr<null_context> null_context::make() {
    return std::shared_ptr<null_context>(new null_context());
}

bool null_context::key_down(const keyboard::key &aKey) const {
    return mKeys.held.count(aKey) != 0;
}

bool null_context::key_just_pressed(const keyboard::key &aKey) const {
    return mKeys.pressedThisFrame.count(aKey) != 0;
}

bool null_context::key_just_released(const keyboard::key &aKey) const {
    return mKeys.releasedThisFrame.count(aKey) != 0;
}

bool null_context::mouse_button_down(const mouse::button &aButton) const {
    return mMouseButtons.held.count(aButton) != 0;
}

bool null_context::mouse_button_just_pressed(const mouse::button &aButton) const {
    return mMouseButtons.pressedThisFrame.count(aButton) != 0;
}

bool null_context::mouse_button_just_released(const mouse::button &aButton) const {
    return mMouseButtons.releasedThisFrame.count(aButton) != 0;
}

mouse::cursor_2d_type null_context::mouse_cursor_position() const { return mCursorPosition; }

mouse::cursor_2d_type null_context::mouse_delta() const { return mMouseDelta; }

mouse::scroll_2d_type null_context::mouse_scroll_delta() const { return mScrollDelta; }

std::optional<mouse::axis> null_context::any_mouse_axis_down(float threshold) const {
    if (std::abs(mMouseDelta.x) > threshold) return mouse::axis::x;
    if (std::abs(mMouseDelta.y) > threshold) return mouse::axis::y;

    return {};
}

mouse::cursor_mode null_context::mouse_cursor_mode() const { return mCursorMode; }

void null_context::set_mouse_cursor_mode(mouse::cursor_mode aMode) { mCursorMode = aMode; }

context::gamepad_ptr null_context::get_gamepad(const size_t index) {
    if (index >= mGamepads.size()) {
        static const auto pAbsent = std::make_shared<null_gamepad>();

        return pAbsent;
    }

    return mGamepads[index];
}

context::gamepad_collection_type null_context::gamepads() {
    gamepad_collection_type out;

    out.reserve(mGamepads.size());

    for (const auto &pGamepad : mGamepads) out.push_back(pGamepad);

    return out;
}

std::shared_ptr<null_gamepad> null_context::gamepad_at(const std::size_t index) {
    return index < mGamepads.size() ? mGamepads[index] : nullptr;
}

std::optional<std::size_t> null_context::attach_gamepad(const std::string &aName,
    const std::string &aGuid) {
    std::size_t port = 0;

    while (mPortsInUse.count(port)) ++port;

    return attach_gamepad_at_port(port, aName, aGuid);
}

std::optional<std::size_t> null_context::attach_gamepad_at_port(const std::size_t aPort,
    const std::string &aName, const std::string &aGuid) {
    std::vector<slot_memory> slots;

    slots.reserve(mGamepads.size());

    for (const auto &pGamepad : mGamepads) {
        slot_memory memory;

        memory.occupied = pGamepad->connected();
        memory.guid = std::string(pGamepad->guid());
        memory.backendIndex = pGamepad->last_port();

        slots.push_back(std::move(memory));
    }

    const auto player = assign_slot(slots, aGuid, aPort);

    if (!player) return {};

    mGamepads[*player]->attach(aName, aGuid);
    mGamepads[*player]->set_port(aPort);

    mPortsInUse.insert(aPort);

    return player;
}

void null_context::detach_gamepad(const std::size_t aPlayer) {
    if (aPlayer >= mGamepads.size()) return;

    if (const auto port = mGamepads[aPlayer]->last_port()) mPortsInUse.erase(*port);

    mGamepads[aPlayer]->detach();
}

std::optional<std::size_t> null_context::port_of(const std::size_t aPlayer) const {
    if (aPlayer >= mGamepads.size() || !mGamepads[aPlayer]->connected()) return {};

    return mGamepads[aPlayer]->last_port();
}

void null_context::press_key(const keyboard::key aKey) {
    if (mKeys.held.insert(aKey).second) mKeys.pressedThisFrame.insert(aKey);
}

void null_context::release_key(const keyboard::key aKey) {
    if (mKeys.held.erase(aKey)) mKeys.releasedThisFrame.insert(aKey);
}

void null_context::press_mouse_button(const mouse::button aButton) {
    if (mMouseButtons.held.insert(aButton).second) mMouseButtons.pressedThisFrame.insert(aButton);
}

void null_context::release_mouse_button(const mouse::button aButton) {
    if (mMouseButtons.held.erase(aButton)) mMouseButtons.releasedThisFrame.insert(aButton);
}

void null_context::set_cursor_position(const mouse::cursor_2d_type aPosition) {
    mCursorPosition = aPosition;
}

void null_context::set_mouse_delta(const mouse::cursor_2d_type aDelta) { mMouseDelta = aDelta; }

void null_context::set_scroll_delta(const mouse::scroll_2d_type aDelta) { mScrollDelta = aDelta; }

void null_context::swap_players(const std::size_t aLeft, const std::size_t aRight) {
    if (aLeft == aRight || aLeft >= mGamepads.size() || aRight >= mGamepads.size()) return;

    std::swap(mGamepads[aLeft], mGamepads[aRight]);
}

std::size_t null_context::add_gamepad_mappings(const std::string &aMappings) {
    const auto guids = mapping_guids(aMappings);

    mMappedGuids.insert(guids.begin(), guids.end());

    promote_mapped_devices();

    return guids.size();
}

void null_context::promote_mapped_devices() {
    for (auto &pGamepad : mGamepads)
        if (pGamepad->connected() && mMappedGuids.count(std::string(pGamepad->guid())))
            pGamepad->set_standard_mapping(true);
}

void null_context::update() {
    promote_mapped_devices();

    mKeys.advance();
    mMouseButtons.advance();

    for (auto &pGamepad : mGamepads) pGamepad->update();
}
