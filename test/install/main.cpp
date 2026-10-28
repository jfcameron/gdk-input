// © Joseph Cameron - MIT License

#include <gdk/input/controls.h>
#include <gdk/input/null_context.h>

int main() {
    using namespace gdk::input;

    const auto pContext = null_context::make();
    const auto pControls = controls::make(pContext);

    pControls->bind("jump", keyboard::key::space);

    pContext->press_key(keyboard::key::space);

    return pControls->just_pressed("jump") ? 0 : 1;
}
