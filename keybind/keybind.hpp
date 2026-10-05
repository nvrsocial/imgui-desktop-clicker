#pragma once

#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <atomic>

#include "../imgui/imgui.h"

#include "../style/style.h"

enum class KeybindMode {
	Off,
	Hold,
	HoldOff,
	Always,
	Toggle
};

enum class InputType {
	None,
	VirtualKey,
};

struct Keybind {
	std::atomic<InputType> type{ InputType::None };
	std::atomic<int> vKey{ 0 };
	std::atomic<KeybindMode> mode{ KeybindMode::Toggle };
	std::atomic<bool> state{ false };
	std::atomic<bool> isPressed{ false };
};

enum class KeybindIndex {
	LeftClicker,
	RightClicker,
	AfkClicker,
	COUNT
};

class KeybindSystem {
private:
	friend class Settings;
	Keybind hotkeys[static_cast<size_t>(KeybindIndex::COUNT)];

	bool IsKeyPressed(const Keybind& hotkey) {
		if (hotkey.type == InputType::VirtualKey) {
			return (GetAsyncKeyState(hotkey.vKey.load()) & 0x8000) && !(GetAsyncKeyState(hotkey.vKey.load()) & 0x4000);
		}
		return false;
	}

	bool IsKeyDown(const Keybind& hotkey) {
		if (hotkey.type == InputType::VirtualKey) {
			return GetAsyncKeyState(hotkey.vKey.load()) & 0x8000;
		}
		return false;
	}

	const char* GetKeyName(const Keybind& hotkey) {
		static char keyName[256];
		if (hotkey.type == InputType::None) {
			return "None";
		}

		int vKey = hotkey.vKey.load();

		switch (vKey) {
		case VK_LBUTTON: return "M1";
		case VK_RBUTTON: return "M2";
		case VK_MBUTTON: return "M3";
		case VK_XBUTTON1: return "M4";
		case VK_XBUTTON2: return "M5";
		case VK_BACK: return "Backspace";
		case VK_TAB: return "Tab";
		case VK_RETURN: return "Enter";
		case VK_PAUSE: return "Pause";
		case VK_CAPITAL: return "CapsL";
		case VK_ESCAPE: return "Escape";
		case VK_SPACE: return "Space";
		case VK_PRIOR: return "Page Up";
		case VK_NEXT: return "Page Down";
		case VK_END: return "End";
		case VK_HOME: return "Home";
		case VK_LEFT: return "Left";
		case VK_UP: return "Up";
		case VK_RIGHT: return "Right";
		case VK_DOWN: return "Down";
		case VK_SNAPSHOT: return "PrintSc";
		case VK_INSERT: return "Insert";
		case VK_DELETE: return "Delete";
		case VK_LWIN: return "Left Win";
		case VK_RWIN: return "Right Win";
		case VK_APPS: return "Menu";
		case VK_NUMPAD0: return "Num 0";
		case VK_NUMPAD1: return "Num 1";
		case VK_NUMPAD2: return "Num 2";
		case VK_NUMPAD3: return "Num 3";
		case VK_NUMPAD4: return "Num 4";
		case VK_NUMPAD5: return "Num 5";
		case VK_NUMPAD6: return "Num 6";
		case VK_NUMPAD7: return "Num 7";
		case VK_NUMPAD8: return "Num 8";
		case VK_NUMPAD9: return "Num 9";
		case VK_MULTIPLY: return "Num *";
		case VK_ADD: return "Num +";
		case VK_SEPARATOR: return "Separator";
		case VK_SUBTRACT: return "Num -";
		case VK_DECIMAL: return "Num .";
		case VK_DIVIDE: return "Num /";
		case VK_NUMLOCK: return "Num Lock";
		case VK_F1: return "F1";
		case VK_F2: return "F2";
		case VK_F3: return "F3";
		case VK_F4: return "F4";
		case VK_F5: return "F5";
		case VK_F6: return "F6";
		case VK_F7: return "F7";
		case VK_F8: return "F8";
		case VK_F9: return "F9";
		case VK_F10: return "F10";
		case VK_F11: return "F11";
		case VK_F12: return "F12";
		case VK_LSHIFT: return "Left Shift";
		case VK_RSHIFT: return "Right Shift";
		case VK_LCONTROL: return "Left Ctrl";
		case VK_RCONTROL: return "Right Ctrl";
		case VK_LMENU: return "Left Alt";
		case VK_RMENU: return "Right Alt";
		case VK_VOLUME_MUTE: return "Vol Mute";
		case VK_VOLUME_DOWN: return "Vol Down";
		case VK_VOLUME_UP: return "Vol Up";
		case VK_MEDIA_NEXT_TRACK: return "Next Track";
		case VK_MEDIA_PREV_TRACK: return "Prev Track";
		case VK_MEDIA_STOP: return "Media Stop";
		case VK_MEDIA_PLAY_PAUSE: return "Play/Pause";

		default:
			UINT scanCode = MapVirtualKey(vKey, MAPVK_VK_TO_VSC);
			if (GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName))) {
				return keyName;
			}

			snprintf(keyName, sizeof(keyName), "Key 0x%X", vKey);
			return keyName;
		}
	}

	const char* GetModeName(KeybindMode mode)
	{
		switch (mode)
		{
		case KeybindMode::Off:     return "Off";
		case KeybindMode::Hold:    return "Hold";
		case KeybindMode::HoldOff: return "HoldOff";
		case KeybindMode::Always:  return "Always";
		case KeybindMode::Toggle:  return "Toggle";
		default:                   return "Unknown";
		}
	}

	int GetPressedKey() {
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) return VK_LBUTTON;
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) return VK_RBUTTON;
		if (GetAsyncKeyState(VK_MBUTTON) & 0x8000) return VK_MBUTTON;
		if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) return VK_XBUTTON1;
		if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) return VK_XBUTTON2;

		for (int i = 0; i < 256; i++) {
			if (GetAsyncKeyState(i) & 0x8000) {
				return i;
			}
		}
		return 0;
	}

public:
	void InitDefaults()
	{
		hotkeys[(int)KeybindIndex::LeftClicker].type.store(InputType::VirtualKey);
		hotkeys[(int)KeybindIndex::LeftClicker].vKey.store(VK_XBUTTON2);
		hotkeys[(int)KeybindIndex::LeftClicker].mode.store(KeybindMode::Hold);

		hotkeys[(int)KeybindIndex::RightClicker].type.store(InputType::VirtualKey);
		hotkeys[(int)KeybindIndex::RightClicker].vKey.store(VK_XBUTTON1);
		hotkeys[(int)KeybindIndex::RightClicker].mode.store(KeybindMode::Hold);

		hotkeys[(int)KeybindIndex::AfkClicker].type.store(InputType::VirtualKey);
		hotkeys[(int)KeybindIndex::AfkClicker].vKey.store(VK_MBUTTON);
		hotkeys[(int)KeybindIndex::AfkClicker].mode.store(KeybindMode::Toggle);
	}

	void Update() {
		for (auto& hotkey : hotkeys) {
			bool currentlyPressed = IsKeyDown(hotkey);
			bool wasPressed = hotkey.isPressed.exchange(currentlyPressed);

			switch (hotkey.mode.load()) {
			case KeybindMode::Hold:
				hotkey.state.store(currentlyPressed);
				break;
			case KeybindMode::Toggle:
				if (currentlyPressed && !wasPressed) {
					hotkey.state.store(!hotkey.state.load());
				}
				break;
			case KeybindMode::HoldOff:
				hotkey.state.store(!currentlyPressed);
				break;
			case KeybindMode::Always:
				hotkey.state.store(true);
				break;
			default:
				hotkey.state.store(false);
				break;
			}
		}
	}

	int GetHotkeyVK(KeybindIndex index) const {
		return hotkeys[static_cast<size_t>(index)].vKey;
	}

	bool IsActive(KeybindIndex index) {
		return hotkeys[static_cast<size_t>(index)].state.load();
	}

	static bool BeginStyledHotkeyPopup(const char* id)
	{
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0, 0, 0, 0));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 12));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);

		bool open = ImGui::BeginPopupModal(id, NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		if (!open) {
			ImGui::PopStyleVar(2);
			ImGui::PopStyleColor(2);
			return false;
		}

		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImVec2 pmin = ImGui::GetWindowPos(), size = ImGui::GetWindowSize(), pmax(pmin.x + size.x, pmin.y + size.y);

		draw->AddRectFilled(pmin, pmax, ImColor(g_Theme.AccentDark), 6.f);
		draw->AddRectFilledMultiColor(ImVec2(pmin.x + 1, pmin.y + 1), ImVec2(pmax.x - 1, pmax.y - 1), ImColor(g_Theme.AccentLight), ImColor(g_Theme.Accent), ImColor(g_Theme.AccentDark), ImColor(Darken(g_Theme.Accent, 0.8f)));
		draw->AddRect(pmin, pmax, ImColor(g_Theme.Border), 6.f);

		return true;
	}

	static void EndStyledHotkeyPopup() {
		ImGui::EndPopup();
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(1);
	}

	bool RenderHotkey(const char* label, KeybindIndex index) {
		bool changed = false;

		float verticalOffset = (ImGui::GetFrameHeight() - ImGui::GetTextLineHeight()) * 0.5f;

		Keybind& hotkey = hotkeys[static_cast<size_t>(index)];

		ImGui::PushID(static_cast<int>(index));

		ImGui::BeginGroup();
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);
			if (ImGui::Button(GetKeyName(hotkey), ImVec2(80, 25))) {
				ImGui::OpenPopup("Change Hotkey");
			}

			if (ImGui::BeginPopupContextItem("ModePopup"))
			{
				KeybindMode current = hotkey.mode.load();

				if (ImGui::MenuItem("Off", nullptr, current == KeybindMode::Off))
					hotkey.mode.store(KeybindMode::Off);

				if (ImGui::MenuItem("Hold", nullptr, current == KeybindMode::Hold))
					hotkey.mode.store(KeybindMode::Hold);

				if (ImGui::MenuItem("Toggle", nullptr, current == KeybindMode::Toggle))
					hotkey.mode.store(KeybindMode::Toggle);

				if (ImGui::MenuItem("Always", nullptr, current == KeybindMode::Always))
					hotkey.mode.store(KeybindMode::Always);

				if (ImGui::MenuItem("Hold Off", nullptr, current == KeybindMode::HoldOff))
					hotkey.mode.store(KeybindMode::HoldOff);

				ImGui::EndPopup();
			}

			if (BeginStyledHotkeyPopup("Change Hotkey")) {
				ImGui::TextColored(ImColor(g_Theme.Text), "Press any key!");

				int pressedKey = GetPressedKey();

				if (pressedKey == VK_ESCAPE) {
					hotkey.type = InputType::None;
					hotkey.vKey = 0;
					changed = true;
					ImGui::CloseCurrentPopup();
				}
				else if (pressedKey != 0) {
					hotkey.type = InputType::VirtualKey;
					hotkey.vKey = pressedKey;
					changed = true;
					ImGui::CloseCurrentPopup();
				}

				ImGui::PopStyleColor();

				EndStyledHotkeyPopup();
			}
		}
		ImGui::EndGroup();

		ImGui::PopID();
		return changed;
	}
};

extern KeybindSystem g_keyBind;