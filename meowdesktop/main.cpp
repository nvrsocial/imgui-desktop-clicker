#include "main.hpp"

#include "../keybind/keybind.hpp"

#include "../clicker/clicker.hpp"

#include "../style/style.h"
#include "../style/icons.h"
#include "../style/particles.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	GetWindowRect(GetDesktopWindow(), &gui::desktop);
	ImGui_ImplWin32_EnableDpiAwareness();

	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandleW(nullptr), nullptr, nullptr, nullptr, nullptr, L"Meow", nullptr };
	RegisterClassExW(&wc);

	HWND hwnd = CreateWindowExW(WS_EX_APPWINDOW | WS_EX_LAYERED, wc.lpszClassName, L"Meow", WS_POPUP, (gui::desktop.right / 2) - ((int)gui::size.x / 2), (gui::desktop.bottom / 2) - ((int)gui::size.y / 2), (int)gui::size.x, (int)gui::size.y, nullptr, nullptr, wc.hInstance, nullptr);
	SetLayeredWindowAttributes(hwnd, 0, 245, LWA_ALPHA);

	HWND g_MainWindow = hwnd;

	gui::g_TrayIcon.cbSize = sizeof(NOTIFYICONDATAW);
	gui::g_TrayIcon.hWnd = hwnd;
	gui::g_TrayIcon.uID = 1;
	gui::g_TrayIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	gui::g_TrayIcon.uCallbackMessage = (WM_USER + 1);
	gui::g_TrayIcon.hIcon = (HICON)LoadImageW(nullptr, L"blank.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	wcscpy_s(gui::g_TrayIcon.szTip, L"Meow");
	Shell_NotifyIconW(NIM_ADD, &gui::g_TrayIcon);

	MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea(hwnd, &margins);

	if (!CreateDeviceD3D(hwnd)) {
		CleanupDeviceD3D();
		DestroyWindow(hwnd);
		UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = nullptr;
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 20.0f);
	ImFont* logoFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arialbd.ttf", 24.0f);

	HRESULT MINIMIZE_HR = D3DX11CreateShaderResourceViewFromMemory(gui::g_pd3dDevice, minimize_p, sizeof(minimize_p), nullptr, nullptr, &gui::minimize, nullptr);
	HRESULT CLOSE_HR = D3DX11CreateShaderResourceViewFromMemory(gui::g_pd3dDevice, close_p, sizeof(close_p), nullptr, nullptr, &gui::close, nullptr);

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(gui::g_pd3dDevice, gui::g_pd3dDeviceContext);

	style_dark();

	g_keyBind.InitDefaults();

	StartClicker();
	StartAfkClicker();
	StartRandomClicker();

	InitializeParticles();

	bool open = true;
	while (open) {
		g_keyBind.Update();
		MSG msg{};
		while (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);

			if (msg.message == WM_QUIT) open = false;
		}

		if (!open) break;

		if (gui::g_SwapChainOccluded && gui::g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
			Sleep(10);
			continue;
		}

		gui::g_SwapChainOccluded = false;

		if (gui::g_ResizeWidth != 0 && gui::g_ResizeHeight != 0) {
			CleanupRenderTarget();
			gui::g_pSwapChain->ResizeBuffers(0, gui::g_ResizeWidth, gui::g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			gui::size = ImVec2((float)gui::g_ResizeWidth, (float)gui::g_ResizeHeight);
			gui::g_ResizeWidth = 0;
			gui::g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(640, 600));
			ImGui::Begin("Meow", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			{
				ImDrawList* draw = ImGui::GetWindowDrawList();
				ImVec2 p = ImGui::GetWindowPos(), s = ImGui::GetWindowSize();

				static bool particles_on = false;

				ImVec4 border = g_Theme.Accent; border.w = 0.15f;

				// border painting
				draw->AddRect(p, ImVec2(p.x + s.x, p.y + s.y), ImColor(border), 12.0f, 0, 3.0f);
				draw->AddRect(ImVec2(p.x + 1, p.y + 1), ImVec2(p.x + s.x - 1, p.y + s.y - 1), ImColor(border), 12.0f, 0, 1.5f);
				draw->AddRect(ImVec2(p.x + 2, p.y + 2), ImVec2(p.x + s.x - 2, p.y + s.y - 2), ImColor(border), 12.0f, 0, 1.0f);

				//draw logo
				[&] { ImGui::PushFont(logoFont); ImGui::SetCursorPos({ 245, 8 }); ImGui::Text("Meow Desktop"); ImGui::PopFont(); }();

				// exit(0)
				ImGui::SetCursorPos(ImVec2(610, 8));
				ImGui::Image(gui::close, ImVec2(20, 20));
				if (ImGui::IsItemClicked(0)) {
					DestroyWindow(hwnd);
				}

				// default collapse
				ImGui::SetCursorPos(ImVec2(585, 8));
				ImGui::Image(gui::minimize, ImVec2(19, 19));
				if (ImGui::IsItemClicked(0)) {
					ShowWindow(hwnd, SW_MINIMIZE);
				}

				// hide mode
				ImGui::SetCursorPos(ImVec2(20, 8));
				ImGui::Image(gui::minimize, ImVec2(19, 19));
				if (ImGui::IsItemClicked(0)) {
					ShowWindow(hwnd, SW_HIDE);
				}

				for (int i = 0; i < 2; i++) {
					ImGui::Spacing();
				}

				ImGui::Separator();

				ImGui::SetCursorPos(ImVec2(15, 70));
				ImGui::BeginChild("##PvP", ImVec2(610, 290), ImGuiChildFlags_Borders);
				{
					bool random_enabled = left_random_clicker_on.load();
					bool left_enabled = left_clicker_on.load();
					bool right_enabled = right_clicker_on.load();

					draw->AddText(ImVec2(35, 60), IM_COL32_WHITE, "PvP");

					ImGui::SetCursorPos(ImVec2(15, 25));
					if (ImGui::Checkbox("Left Button", &left_enabled)) {
						left_clicker_on.store(left_enabled);

						if (left_enabled) {
							left_random_clicker_on.store(false);
						}
					}

					if (left_enabled) {
						int left_current_cps = left_cps.load();

						ImGui::SetCursorPos(ImVec2(500, 20));
						g_keyBind.RenderHotkey("", KeybindIndex::LeftClicker);

						ImGui::SetCursorPos(ImVec2(170, 17.5));
						ImGui::PushItemWidth(220.0f);
						if (ImGui::SliderInt("##left", &left_current_cps, 1, 35)) {
							left_cps.store(left_current_cps);
						}
					}

					ImGui::SetCursorPos(ImVec2(15, 90));
					if (ImGui::Checkbox("Random Left", &random_enabled)) {
						left_random_clicker_on.store(random_enabled);

						if (random_enabled) {
							left_clicker_on.store(false);
						}
					}

					if (random_enabled) {
						int min_cps = left_random_min_cps.load();
						int max_cps = left_random_max_cps.load();

						ImGui::SetCursorPos(ImVec2(500, 85));
						g_keyBind.RenderHotkey("", KeybindIndex::LeftClicker);

						ImGui::SetCursorPos(ImVec2(170, 82.5));
						ImGui::PushItemWidth(75.0f);
						if (ImGui::SliderInt("##left_rand_min", &min_cps, 1, 35)) {
							left_random_min_cps.store(min_cps);
						}

						ImGui::SetCursorPos(ImVec2(315, 82.5));
						ImGui::PushItemWidth(75.0f);
						if (ImGui::SliderInt("##left_rand_max", &max_cps, 1, 35)) {
							left_random_max_cps.store(max_cps);
						}
					}

					ImGui::SetCursorPos(ImVec2(15, 57.5));
					if (ImGui::Checkbox("Right Button", &right_enabled)) {
						right_clicker_on.store(right_enabled);
					}

					if (right_enabled) {
						int right_current_cps = right_cps.load();

						ImGui::SetCursorPos(ImVec2(500, 52.5));
						g_keyBind.RenderHotkey("", KeybindIndex::RightClicker);

						ImGui::SetCursorPos(ImVec2(170, 50));
						ImGui::PushItemWidth(220.0f);
						if (ImGui::SliderInt("##right", &right_current_cps, 1, 35)) {
							right_cps.store(right_current_cps);
						}
					}
				}
				ImGui::EndChild();

				ImGui::SetCursorPos(ImVec2(15, 370));
				ImGui::BeginChild("##PvE", ImVec2(610, 145), ImGuiChildFlags_Borders);
				{
					bool afk_enabled = left_afk_on.load();

					draw->AddText(ImVec2(35, 360), IM_COL32_WHITE, "PvE");

					ImGui::SetCursorPos(ImVec2(15, 25));
					if (ImGui::Checkbox("Hold Left", &afk_enabled)) {
						left_afk_on.store(afk_enabled);
					}

					if (afk_enabled) {
						ImGui::SetCursorPos(ImVec2(500, 25));
						g_keyBind.RenderHotkey("", KeybindIndex::AfkClicker);
					}
				}
				ImGui::EndChild();

				ImGui::SetCursorPos(ImVec2(15, 525));
				ImGui::BeginChild("##About", ImVec2(610, 65), ImGuiChildFlags_Borders);
				{
					draw->AddText(ImVec2(35, 515), IM_COL32_WHITE, "Design");

					ImGui::SetCursorPos(ImVec2(15, 17.5));
					if (ImGui::ColorEdit4("Theme", (float*)&g_Theme.Accent))
					{
						g_Theme.AccentLight = Lighten(g_Theme.Accent, 1.25f);
						g_Theme.AccentDark = Darken(g_Theme.Accent, 0.65f);

						g_Theme.Border = g_Theme.Accent;
						g_Theme.Border.w = 0.35f;
						style_dark();
					}

					ImGui::SetCursorPos(ImVec2(125, 17.5));
					ImGui::Checkbox("Particles", &particles_on);

					ImGui::SetCursorPos(ImVec2(515, 17.5));
					ImGui::Text("Beta | 0.1");
				}
				ImGui::EndChild();

				if (particles_on) {
					UpdateParticles(0.01);
					RenderParticles();
				}
			}
			ImGui::End();
		}
		ImGui::Render();

		const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		gui::g_pd3dDeviceContext->OMSetRenderTargets(1, &gui::g_mainRenderTargetView, nullptr);
		gui::g_pd3dDeviceContext->ClearRenderTargetView(gui::g_mainRenderTargetView, clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		HRESULT hr = gui::g_pSwapChain->Present(1, 0);
		gui::g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(hwnd);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

bool CreateDeviceD3D(HWND hWnd) {
	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel{};
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &gui::g_pSwapChain, &gui::g_pd3dDevice, &featureLevel, &gui::g_pd3dDeviceContext);

	if (res == DXGI_ERROR_UNSUPPORTED) {
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &gui::g_pSwapChain, &gui::g_pd3dDevice, &featureLevel, &gui::g_pd3dDeviceContext);
	}

	if (res != S_OK) return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D() {
	CleanupRenderTarget();

	if (gui::g_pSwapChain) {
		gui::g_pSwapChain->Release();
		gui::g_pSwapChain = nullptr;
	}

	if (gui::g_pd3dDeviceContext) {
		gui::g_pd3dDeviceContext->Release();
		gui::g_pd3dDeviceContext = nullptr;
	}

	if (gui::g_pd3dDevice) {
		gui::g_pd3dDevice->Release();
		gui::g_pd3dDevice = nullptr;
	}
}

void CreateRenderTarget() {
	ID3D11Texture2D* backBuffer = nullptr;
	gui::g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	gui::g_pd3dDevice->CreateRenderTargetView(backBuffer, nullptr, &gui::g_mainRenderTargetView);
	backBuffer->Release();
}

void CleanupRenderTarget() {
	if (gui::g_mainRenderTargetView) {
		gui::g_mainRenderTargetView->Release();
		gui::g_mainRenderTargetView = nullptr;
	}
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case (WM_USER + 1):
		if (lParam == WM_LBUTTONDBLCLK) {
			ShowWindow(hWnd, SW_SHOW);
			ShowWindow(hWnd, SW_RESTORE);
			SetForegroundWindow(hWnd);
		}
		return 0;

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED) return 0;
		gui::g_ResizeWidth = (UINT)LOWORD(lParam);
		gui::g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;

	case WM_SYSCOMMAND:
		if ((wParam & 0xFFF0) == SC_KEYMENU) return 0;
		break;

	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		return 0;

	case WM_DESTROY:
		Shell_NotifyIconW(NIM_DELETE, &gui::g_TrayIcon);
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
		gui::guiPosition = MAKEPOINTS(lParam);
		return 0;

	case WM_MOUSEMOVE:
		if ((wParam & MK_LBUTTON) == MK_LBUTTON) {
			const POINTS points = MAKEPOINTS(lParam);
			RECT rect{};
			GetWindowRect(hWnd, &rect);

			rect.left += points.x - gui::guiPosition.x, rect.top += points.y - gui::guiPosition.y;

			if (!block_moving && gui::guiPosition.x >= 0 && gui::guiPosition.x <= (gui::size.x - windows_title_height) && gui::guiPosition.y >= 0 && gui::guiPosition.y <= windows_title_height) {
				SetWindowPos(hWnd, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
			}
		}
		return 0;
	}

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}
