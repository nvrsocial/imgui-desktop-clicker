#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <cmath>
#include <vector>
#include <chrono>
#include <map>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_internal.h"

#include <d3d11.h>
#include <D3DX11tex.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "dxgi.lib")

#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

namespace gui {
	static ID3D11Device* g_pd3dDevice = nullptr;
	static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
	static IDXGISwapChain* g_pSwapChain = nullptr;
	static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

	ID3D11ShaderResourceView* minimize = nullptr;
	ID3D11ShaderResourceView* close = nullptr;

	static bool g_SwapChainOccluded = false;
	static UINT g_ResizeWidth = 0;
	static UINT g_ResizeHeight = 0;

	NOTIFYICONDATAW g_TrayIcon = {};

	static POINTS guiPosition = {};
	static ImVec2 size = { 640, 600 };
	RECT desktop{};
}

static int windows_title_height = 40;
static bool block_moving = false;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

extern HWND g_MainWindow;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
