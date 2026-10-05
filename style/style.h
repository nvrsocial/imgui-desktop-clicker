#pragma once

#include "../imgui/imgui.h"

void style_dark();

inline ImVec4 Darken(ImVec4 c, float factor) {
	return ImVec4(
		c.x * factor,
		c.y * factor,
		c.z * factor,
		c.w
	);
}

inline ImVec4 Lighten(ImVec4 c, float factor) {
	return ImVec4(
		(c.x * factor > 1.f ? 1.f : c.x * factor),
		(c.y * factor > 1.f ? 1.f : c.y * factor),
		(c.z * factor > 1.f ? 1.f : c.z * factor),
		c.w
	);
}

struct ThemeColors {
	ImVec4 Accent = ImVec4(1.0f, 0.55f, 0.24f, 1.0f);

	ImVec4 AccentDark = ImVec4(0.67f, 0.31f, 0.12f, 1.0f);
	ImVec4 AccentLight = ImVec4(1.0f, 0.65f, 0.31f, 1.0f);

	ImVec4 Border = ImVec4(1.0f, 0.47f, 0.19f, 0.35f);

	ImVec4 Background = ImVec4(0.07f, 0.07f, 0.08f, 1.0f);
	ImVec4 Hover = ImVec4(0.88f, 0.47f, 0.18f, 1.0f);
	ImVec4 Active = ImVec4(0.70f, 0.35f, 0.14f, 1.0f);

	ImVec4 Text = ImVec4(1.f, 0.96f, 0.92f, 1.f);
};

inline ThemeColors g_Theme;