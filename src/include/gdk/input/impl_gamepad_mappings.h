// © Joseph Cameron - All Rights Reserved

#ifndef GDK_INPUT_IMPL_GAMEPAD_MAPPINGS_H
#define GDK_INPUT_IMPL_GAMEPAD_MAPPINGS_H

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace gdk::input {
    [[nodiscard]] inline std::vector<std::string> mapping_guids(const std::string &aText) {
        std::vector<std::string> out;

        std::size_t at = 0;

        while (at < aText.size()) {
            const auto lineEnd = aText.find_first_of("\r\n", at);
            const auto line = aText.substr(at,
                lineEnd == std::string::npos ? std::string::npos : lineEnd - at);

            at = lineEnd == std::string::npos ? aText.size() : lineEnd + 1;

            const auto comma = line.find(',');

            if (comma != 32) continue;

            const auto guid = line.substr(0, 32);

            const auto hex = [](const char aCharacter) {
                return std::isxdigit(static_cast<unsigned char>(aCharacter)) != 0;
            };

            if (std::all_of(guid.begin(), guid.end(), hex)) out.push_back(guid);
        }

        return out;
    }

    [[nodiscard]] constexpr bool is_standard_trigger(const std::size_t aIndex) {
        return aIndex == 4 || aIndex == 5;   
    }

    [[nodiscard]] constexpr float standard_trigger_value(const float aRaw) {
        return (aRaw + 1.f) * 0.5f;
    }
}

#endif
