#pragma once

#include "../imgui/imgui.h"

#include "../style/style.h"

void style_dark() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(12, 12);
    style.FramePadding = ImVec2(8, 6);
    style.ItemSpacing = ImVec2(10, 8);

    style.WindowRounding = 12.0f;
    style.ChildRounding = 10.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 8.0f;

    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;

    ImVec4* colors = style.Colors, base = g_Theme.Background;

    colors[ImGuiCol_WindowBg] = base;
    colors[ImGuiCol_ChildBg] = ImVec4(base.x + 0.015f, base.y + 0.015f, base.z + 0.018f, 1.0f);
    colors[ImGuiCol_PopupBg] = colors[ImGuiCol_ChildBg];

    colors[ImGuiCol_Text] = g_Theme.Text;
    colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.55f, 0.58f, 1.0f);

    colors[ImGuiCol_Border] = ImVec4(g_Theme.Accent.x, g_Theme.Accent.y, g_Theme.Accent.z, 0.25f);

    colors[ImGuiCol_FrameBg] = base;
    colors[ImGuiCol_FrameBgHovered] = Darken(g_Theme.Accent, 0.25f);
    colors[ImGuiCol_FrameBgActive] = Darken(g_Theme.Accent, 0.45f);

    colors[ImGuiCol_CheckMark] = g_Theme.Accent;

    colors[ImGuiCol_Header] = Darken(g_Theme.Accent, 0.45f);
    colors[ImGuiCol_HeaderHovered] = g_Theme.Accent;
    colors[ImGuiCol_HeaderActive] = g_Theme.AccentLight;

    colors[ImGuiCol_Button] = g_Theme.Accent;
    colors[ImGuiCol_ButtonHovered] = g_Theme.AccentLight;
    colors[ImGuiCol_ButtonActive] = g_Theme.AccentDark;

    colors[ImGuiCol_SliderGrab] = g_Theme.Accent;
    colors[ImGuiCol_SliderGrabActive] = g_Theme.AccentLight;

    colors[ImGuiCol_ScrollbarBg] = base;
    colors[ImGuiCol_ScrollbarGrab] = g_Theme.AccentDark;
    colors[ImGuiCol_ScrollbarGrabHovered] = g_Theme.Accent;
    colors[ImGuiCol_ScrollbarGrabActive] = g_Theme.AccentLight;

    colors[ImGuiCol_ResizeGrip] = g_Theme.AccentDark;
    colors[ImGuiCol_ResizeGripHovered] = g_Theme.Accent;
    colors[ImGuiCol_ResizeGripActive] = g_Theme.AccentLight;

    colors[ImGuiCol_Tab] = Darken(g_Theme.Accent, 0.5f);
    colors[ImGuiCol_TabHovered] = g_Theme.Accent;
    colors[ImGuiCol_TabActive] = g_Theme.AccentLight;

    colors[ImGuiCol_TitleBg] = base;
    colors[ImGuiCol_TitleBgActive] = base;
    colors[ImGuiCol_TitleBgCollapsed] = base;

    colors[ImGuiCol_Separator] = g_Theme.Border;
    colors[ImGuiCol_MenuBarBg] = base;
}