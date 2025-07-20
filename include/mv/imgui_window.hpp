#ifndef MV_IMGUI_WINDOW_HPP
#define MV_IMGUI_WINDOW_HPP

#include <imgui.h>

namespace mv
{
    class ImGuiWindow
    {
    private:
        ImFont *font{};

    public:
        explicit ImGuiWindow(const char *name, const float font_scale = 1.0F, ImFont *f = nullptr)
          : font{f}
        {
            ImGui::Begin(name);

            if (font != nullptr) {
                ImGui::PushFont(font);
            }

            ImGui::SetWindowFontScale(font_scale);
        }

        ~ImGuiWindow()
        {
            if (font != nullptr) {
                ImGui::PopFont();
            }

            ImGui::End();
        }
    };
} // namespace mv

#endif /* MV_IMGUI_WINDOW_HPP */
