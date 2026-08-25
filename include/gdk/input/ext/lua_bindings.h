// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_EXT_LUA_BINDINGS_H
#define GDK_INPUT_EXT_LUA_BINDINGS_H

#include <gdk/input/controls.h>

#include <jfc/lua/data_table.h>

#include <cstddef>
#include <string>

/// \file serialization for lua tables
///
/// This table codec is written against the library's interface, so
/// it serves as living documentation for how to write additional ones
/// (eg for json), and as a convenience for users who are using lua
namespace gdk::input::ext {
    /// \brief the format version this writes
    ///
    /// Written into every table so a file outliving a change to the layout can be recognised rather
    /// than misread.
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

    /// \brief replace a controls' bindings with the ones in a table
    std::size_t from_data_table(controls &aControls, const jfc::lua::data_table &aTable);

    /// \brief serialize a controls to a lua table
    [[nodiscard]] std::string to_string(const controls &aControls);

    /// \brief parsed from_data_table
    std::size_t from_string(controls &aControls, const std::string &aText);
}

#endif
