#pragma once

#include <atomic>
#include <random>

extern std::atomic<bool> left_clicker_on;
extern std::atomic<bool> right_clicker_on;
extern std::atomic<int> left_cps;
extern std::atomic<int> right_cps;

extern std::atomic<bool> left_afk_on;

extern std::atomic<bool> left_random_clicker_on;
extern std::atomic<int> left_random_min_cps;
extern std::atomic<int> left_random_max_cps;

void StartRandomClicker();

void StartClicker();

void StartAfkClicker();