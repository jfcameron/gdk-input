// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_TEXT_INPUT_GLFW_H
#define GDK_INPUT_TEXT_INPUT_GLFW_H

#include <gdk/input/impl_text_input.h>
#include <gdk/input/text.h>

#include <GLFW/glfw3.h>

#include <memory>
#include <set>
#include <vector>

/// whether this glfw can talk to an input method: the IME branch's, and not under emscripten, which
/// links its own glfw with none of it whatever header is included
#if defined(GLFW_IME) && !defined(__EMSCRIPTEN__)
    #define GDK_INPUT_GLFW_IME 1
#else
    #define GDK_INPUT_GLFW_IME 0
#endif

namespace gdk::input {
    /// \brief text input from a glfw window
    class text_input_glfw final {
    public:
        void update();

        [[nodiscard]] const std::vector<text::event> &events() const { return m_Current; }

        [[nodiscard]] text::composition composition() const { return m_Composition; }

        [[nodiscard]] bool focus() const { return m_Focus; }

        /// \brief every glfw key pressed since the update before this one
        [[nodiscard]] const std::set<int> &presses() const { return m_Presses; }

        void set_focus(const bool aFocus);

        void set_caret(const text::caret &aCaret);

        explicit text_input_glfw(std::shared_ptr<GLFWwindow> pWindow);

        ~text_input_glfw();

        text_input_glfw(const text_input_glfw &) = delete;
        text_input_glfw &operator=(const text_input_glfw &) = delete;

    private:
        static void on_character(GLFWwindow *pWindow, unsigned int aCodePoint);
        static void on_key(GLFWwindow *pWindow, int aKey, int aScancode, int aAction, int aMods);
#if GDK_INPUT_GLFW_IME
        static void on_preedit(GLFWwindow *pWindow, int aCount, unsigned int *pCodePoints,
            int aBlockCount, int *pBlockSizes, int aFocusedBlock, int aCaret);
#endif

        std::shared_ptr<GLFWwindow> m_pWindow;

        std::vector<text::event> m_Pending;
        std::vector<text::event> m_Current;

        text::composition m_PendingComposition;
        text::composition m_Composition;

        std::set<int> m_PendingPresses;
        std::set<int> m_Presses;

        bool m_Focus = false;

        GLFWcharfun m_PreviousCharacter = nullptr;
        GLFWkeyfun m_PreviousKey = nullptr;
#if GDK_INPUT_GLFW_IME
        GLFWpreeditfun m_PreviousPreedit = nullptr;
#endif
    };
}

#endif
