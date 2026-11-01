// © Joseph Cameron - MIT License

#ifndef GDK_INPUT_EXCEPTION_H
#define GDK_INPUT_EXCEPTION_H

#include <exception>
#include <memory>
#include <string>

namespace gdk::input {
    /// \brief root exception type for this library
    class exception : public std::exception {
        std::shared_ptr<const std::string> mpWhat;

    public:
        explicit exception(const std::string &aWhat);

        exception(const exception &aOther) noexcept = default;

        exception &operator=(const exception &aOther) noexcept = default;

        const char *what() const noexcept override;
    };
}

#endif
