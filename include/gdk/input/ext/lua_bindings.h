// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_EXT_LUA_BINDINGS_H
#define GDK_INPUT_EXT_LUA_BINDINGS_H

#include <gdk/input/controls.h>

#include <jfc/lua/data_table.h>

#include <cstddef>
#include <string>

/// \file
/// \brief serialization for lua tables
namespace gdk::input::ext {
    inline constexpr double LUA_BINDINGS_VERSION{1};

    /// \brief every binding a controls holds as a table
    ///
    /// Example:
    ///
    /// ~~~{.lua}
    /// {
    ///     version = 1,
    ///     player = 0,
    ///     threshold = 0.1,
    ///     actions = {
    ///         jump = {[1] = {kind = "key", key = "space"},
    ///                 [2] = {kind = "gamepad_button", button = "a"}},
    ///         move_left = {[1] = {kind = "gamepad_axis", axis = "left_x", scale = -1}}
    ///     }
    /// }
    /// ~~~
    [[nodiscard]] jfc::lua::data_table to_data_table(const controls &aControls);

    /// \brief what a table's bindings do to the ones already there
    enum class binding_order {
        replace, //<! what was bound is unbound first
        add      //<! what is in the table is bound beside what was already there
    };

    /// \brief put the bindings in a table onto a controls
    ///
    /// \param aOrder whether what is already bound is cleared first. **Replacing is the default**,
    ///        which is what this has always done.
    std::size_t from_data_table(controls &aControls, const jfc::lua::data_table &aTable,
        binding_order aOrder = binding_order::replace);

    /// \brief serialize a controls to a lua table
    [[nodiscard]] std::string to_string(const controls &aControls);

    /// \brief parsed from_data_table
    std::size_t from_string(controls &aControls, const std::string &aText,
        binding_order aOrder = binding_order::replace);
}

#endif
