// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_EXCEPTION_H
#define GDK_INPUT_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace gdk::input {
    /// \brief root exception type for this library
    class exception : public std::runtime_error {
    public:
        explicit exception(const std::string &aWhat);
    };
}

#endif
