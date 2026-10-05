#include "clicker.hpp"

#include <windows.h>
#include <thread>
#include <chrono>

#include "../imgui/imgui.h"

#include "../keybind/keybind.hpp"

std::atomic<bool> left_clicker_on(false);
std::atomic<bool> right_clicker_on(false);
std::atomic<int> left_cps(0);
std::atomic<int> right_cps(0);

std::atomic<bool> left_afk_on(false);

std::atomic<bool> left_random_clicker_on(false);
std::atomic<int> left_random_min_cps(0);
std::atomic<int> left_random_max_cps(0);

void StartClicker() {
	static bool started = false;

	std::random_device rd;
	std::mt19937 rng(rd());

	if (started) return;
	started = true;

	std::thread([]() {
		using namespace std::chrono;

		std::chrono::steady_clock::time_point nextLeftClick;
		std::chrono::steady_clock::time_point nextRightClick;

		while (true) {
			auto now = steady_clock::now();

			if (left_clicker_on.load() && g_keyBind.IsActive(KeybindIndex::LeftClicker)) {
				int cps = max(1, left_cps.load());
				auto interval = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(1.0 / cps));

				if (now >= nextLeftClick) {
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					nextLeftClick += interval;
				}
			}
			else {
				nextLeftClick = now;
			}

			if (right_clicker_on.load() && g_keyBind.IsActive(KeybindIndex::RightClicker)) {
				int cps = max(1, right_cps.load());
				auto interval = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(1.0 / cps));

				if (now >= nextRightClick) {
					mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
					nextRightClick += interval;
				}
			}
			else {
				nextRightClick = now;
			}
			Sleep(1);
		}
		}).detach();
}

void StartAfkClicker()
{
	static bool started = false;
	if (started) return;
	started = true;

	std::thread([]() {
		bool holding = false;

		while (true) {
			bool shouldHold = left_afk_on.load() && g_keyBind.IsActive(KeybindIndex::AfkClicker);

			if (shouldHold && !holding) {
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				holding = true;
			}
			else if (!shouldHold && holding) {
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				holding = false;
			}
			Sleep(1);
		}
		}).detach();
}

void StartRandomClicker()
{
	static bool started = false;
	if (started) return;
	started = true;

	std::thread([]() {
		using namespace std::chrono;

		std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<int> dist;

		steady_clock::time_point nextClick;

		while (true) {
			auto now = steady_clock::now();

			if (left_random_clicker_on.load() && g_keyBind.IsActive(KeybindIndex::LeftClicker)) {  // можно тот же биндинг 
				int min = left_random_min_cps.load();
				int max = left_random_max_cps.load();

				if (min > max) std::swap(min, max);

				dist = std::uniform_int_distribution<int>(min, max);
				int cps = dist(rng);
				auto interval = duration_cast<steady_clock::duration>(duration<double>(1.0 / cps));

				if (now >= nextClick) {
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					nextClick += interval;
				}
			}
			else {
				nextClick = now;
			}

			Sleep(1);
		}
		}).detach();
}