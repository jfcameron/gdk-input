// © Joseph Cameron - All Rights Reserved

#include <gdk/input/impl_glfw_text_input.h>

#include <algorithm>
#include <cmath>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

using namespace gdk::input;

namespace {
    std::unordered_map<GLFWwindow *, text_input_glfw *> &receivers() {
        static std::unordered_map<GLFWwindow *, text_input_glfw *> instance;
        return instance;
    }

    [[nodiscard]] std::optional<keyboard::key> editing_key(const int aKey) {
        switch (aKey) {
            case GLFW_KEY_BACKSPACE: return keyboard::key::backspace;
            case GLFW_KEY_DELETE:    return keyboard::key::deletekey;
            case GLFW_KEY_LEFT:      return keyboard::key::leftarrow;
            case GLFW_KEY_RIGHT:     return keyboard::key::rightarrow;
            case GLFW_KEY_UP:        return keyboard::key::uparrow;
            case GLFW_KEY_DOWN:      return keyboard::key::downarrow;
            case GLFW_KEY_HOME:      return keyboard::key::home;
            case GLFW_KEY_END:       return keyboard::key::end;
            case GLFW_KEY_PAGE_UP:   return keyboard::key::pageup;
            case GLFW_KEY_PAGE_DOWN: return keyboard::key::pagedown;
            case GLFW_KEY_ENTER:     return keyboard::key::enter;
            case GLFW_KEY_KP_ENTER:  return keyboard::key::numenter;
            case GLFW_KEY_ESCAPE:    return keyboard::key::escape;
            case GLFW_KEY_TAB:       return keyboard::key::tab;

            default: return {};
        }
    }

    [[nodiscard]] text_input_glfw *receiver(GLFWwindow *const pWindow) {
        const auto found = receivers().find(pWindow);

        return found == receivers().end() ? nullptr : found->second;
    }

    template <typename callback_type>
    void give_back(GLFWwindow *const pWindow, callback_type (*const aSet)(GLFWwindow *, callback_type),
        const callback_type aOurs, const callback_type aPrevious) {
        const auto current = aSet(pWindow, aPrevious);

        if (current != aOurs) aSet(pWindow, current);
    }
}

text_input_glfw::text_input_glfw(std::shared_ptr<GLFWwindow> pWindow)
: m_pWindow(std::move(pWindow)) {
    auto *const pGLFWWindow = m_pWindow.get();

    receivers()[pGLFWWindow] = this;

    m_PreviousCharacter = glfwSetCharCallback(pGLFWWindow, on_character);
    m_PreviousKey = glfwSetKeyCallback(pGLFWWindow, on_key);

#if GDK_INPUT_GLFW_IME
    m_PreviousPreedit = glfwSetPreeditCallback(pGLFWWindow, on_preedit);
    glfwSetTextInputFocus(pGLFWWindow, GLFW_FALSE);
#endif
}

text_input_glfw::~text_input_glfw() {
    auto *const pGLFWWindow = m_pWindow.get();

    give_back<GLFWcharfun>(pGLFWWindow, glfwSetCharCallback, on_character, m_PreviousCharacter);
    give_back<GLFWkeyfun>(pGLFWWindow, glfwSetKeyCallback, on_key, m_PreviousKey);
#if GDK_INPUT_GLFW_IME
    give_back<GLFWpreeditfun>(pGLFWWindow, glfwSetPreeditCallback, on_preedit, m_PreviousPreedit);
#endif

    if (receiver(pGLFWWindow) == this) receivers().erase(pGLFWWindow);
}

void text_input_glfw::update() {
    m_Current.clear();

    std::swap(m_Current, m_Pending);
    m_Composition = m_PendingComposition;
    m_Presses = std::exchange(m_PendingPresses, {});
}

void text_input_glfw::set_focus(const bool aFocus) {
    if (aFocus == m_Focus) return;

    m_Focus = aFocus;

#if GDK_INPUT_GLFW_IME
    glfwSetTextInputFocus(m_pWindow.get(), aFocus ? GLFW_TRUE : GLFW_FALSE);
#endif

    if (!aFocus) {
        m_PendingComposition = {};
        m_Composition = {};
    }
}

void text_input_glfw::set_caret(const text::caret &aCaret) {
#if GDK_INPUT_GLFW_IME
    int width, height;

    glfwGetWindowSize(m_pWindow.get(), &width, &height);

    /// glfw's rectangle is in the window's coordinates, from the top left; the caret's is normalised
    /// from the bottom left, as the cursor is
    const auto x = static_cast<int>(std::lround(aCaret.x * width));
    const auto top = static_cast<int>(std::lround((1 - aCaret.y - aCaret.height) * height));
    const auto w = static_cast<int>(std::lround(aCaret.width * width));
    const auto h = static_cast<int>(std::lround(aCaret.height * height));

    glfwSetPreeditCursorRectangle(m_pWindow.get(), x, top, w, h);
#else
    (void)aCaret;
#endif
}

void text_input_glfw::on_character(GLFWwindow *const pWindow, const unsigned int aCodePoint) {
    if (auto *const pInput = receiver(pWindow)) {
        std::string character;

        append_utf8(character, static_cast<char32_t>(aCodePoint));

        append_text(pInput->m_Pending, character);

        if (pInput->m_PreviousCharacter) pInput->m_PreviousCharacter(pWindow, aCodePoint);
    }
}

void text_input_glfw::on_key(GLFWwindow *const pWindow, const int aKey, const int aScancode,
    const int aAction, const int aMods) {
    auto *const pInput = receiver(pWindow);

    if (!pInput) return;

    if (aAction == GLFW_PRESS) pInput->m_PendingPresses.insert(aKey);

    if (const auto key = editing_key(aKey); key && aAction != GLFW_RELEASE) {
        text::edit edit;

        edit.key = *key;
        edit.held.shift = aMods & GLFW_MOD_SHIFT;
        edit.held.control = aMods & GLFW_MOD_CONTROL;
        edit.held.alt = aMods & GLFW_MOD_ALT;
        edit.held.super = aMods & GLFW_MOD_SUPER;
        edit.repeat = aAction == GLFW_REPEAT;

        pInput->m_Pending.emplace_back(edit);
    }

    if (pInput->m_PreviousKey) pInput->m_PreviousKey(pWindow, aKey, aScancode, aAction, aMods);
}

#if GDK_INPUT_GLFW_IME
void text_input_glfw::on_preedit(GLFWwindow *const pWindow, const int aCount,
    unsigned int *const pCodePoints, const int aBlockCount, int *const pBlockSizes,
    const int aFocusedBlock, const int aCaret) {
    auto *const pInput = receiver(pWindow);

    if (!pInput) return;

    text::composition composition;

    const int count = pCodePoints ? std::max(aCount, 0) : 0;

    std::vector<std::size_t> starts;

    starts.reserve(static_cast<std::size_t>(count) + 1);

    for (int i = 0; i < count; ++i) {
        starts.push_back(composition.text.size());

        append_utf8(composition.text, static_cast<char32_t>(pCodePoints[i]));
    }

    starts.push_back(composition.text.size());

    const auto at = [&starts, count](const int aIndex) {
        return starts[static_cast<std::size_t>(std::clamp(aIndex, 0, count))];
    };

    composition.caret = at(aCaret);

    if (pBlockSizes && aFocusedBlock >= 0 && aFocusedBlock < aBlockCount) {
        int begin = 0;

        for (int block = 0; block < aFocusedBlock; ++block) begin += pBlockSizes[block];

        composition.focusBegin = at(begin);
        composition.focusEnd = at(begin + pBlockSizes[aFocusedBlock]);
    }

    pInput->m_PendingComposition = std::move(composition);

    if (pInput->m_PreviousPreedit)
        pInput->m_PreviousPreedit(pWindow, aCount, pCodePoints, aBlockCount, pBlockSizes,
            aFocusedBlock, aCaret);
}
#endif
