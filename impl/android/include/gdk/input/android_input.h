// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_ANDROID_INPUT_H
#define GDK_INPUT_ANDROID_INPUT_H

#include <gdk/input/null_context.h>

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <utility>

struct AInputEvent;

namespace gdk::input {
    /// \brief an Android activity's input
    class android_input final {
    public:
        explicit android_input(std::shared_ptr<null_context> apContext);

        bool feed(const AInputEvent *const apEvent, const std::pair<int, int> aWindowSize);

    private:
        [[nodiscard]] std::optional<std::size_t> player_of(const std::int32_t aDevice);

        bool feed_key(const AInputEvent *const apEvent);
        bool feed_motion(const AInputEvent *const apEvent, const std::pair<int, int> aWindowSize);

        std::shared_ptr<null_context> m_pContext;
        std::map<std::int32_t, std::size_t> m_Players;
        mouse::cursor_2d_type m_Cursor{0, 0};
    };
}

#endif
