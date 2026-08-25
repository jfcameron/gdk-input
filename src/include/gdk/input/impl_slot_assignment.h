// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_IMPL_SLOT_ASSIGNMENT_H
#define GDK_INPUT_IMPL_SLOT_ASSIGNMENT_H

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace gdk::input {
    /// \brief what a player slot remembers about the device that was last in it
    struct slot_memory final {
        bool occupied{false};

        //! the identity of whatever is was last held in this slot
        std::string guid;

        //! where the backend had that device. Nothing if it was never occupied.
        std::optional<std::size_t> backendIndex;
    };

    /// \brief choose which player a newly connected device belongs to
    [[nodiscard]] inline std::optional<std::size_t> assign_slot(
        const std::vector<slot_memory> &aSlots, const std::string &aGuid,
        const std::size_t aBackendIndex) {
        for (std::size_t i = 0; i < aSlots.size(); ++i)
            if (!aSlots[i].occupied && aSlots[i].guid == aGuid && !aGuid.empty()
                && aSlots[i].backendIndex == aBackendIndex)
                return i;

        for (std::size_t i = 0; i < aSlots.size(); ++i)
            if (!aSlots[i].occupied && aSlots[i].guid == aGuid && !aGuid.empty()) return i;

        for (std::size_t i = 0; i < aSlots.size(); ++i)
            if (!aSlots[i].occupied) return i;

        return {};
    }
}

#endif

