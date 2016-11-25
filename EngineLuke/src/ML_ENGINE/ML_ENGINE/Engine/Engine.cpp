/******************************************************************************/
/*!
\file  Engine.cpp
\project  CS460
\author Jorge Antonio Robles

Copyright (C) 2015 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "../UI/Imgui/imgui.h"
#include "../UI/Imgui/imgui_impl_dx11.h"

#define WIN32_LEAN_AND_MEAN
#include <windowsx.h>

#include "Engine.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;

void CreateRenderTarget()
{
    DXGI_SWAP_CHAIN_DESC sd;
    g_pSwapChain->GetDesc(&sd);

    // Create the render target
    ID3D11Texture2D* pBackBuffer;
    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
    ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
    render_target_view_desc.Format = sd.BufferDesc.Format;
    render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &g_mainRenderTargetView);
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

HRESULT CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    {
        ZeroMemory(&sd, sizeof(sd));
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
    }

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[1] = { D3D_FEATURE_LEVEL_11_0, };
    HRESULT m_result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 1, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (m_result != S_OK)
        return E_FAIL;

    // Setup rasterizer
    {
        D3D11_RASTERIZER_DESC RSDesc;
        memset(&RSDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
        RSDesc.FillMode = D3D11_FILL_SOLID;
        RSDesc.CullMode = D3D11_CULL_NONE;
        RSDesc.FrontCounterClockwise = FALSE;
        RSDesc.DepthBias = 0;
        RSDesc.SlopeScaledDepthBias = 0.0f;
        RSDesc.DepthBiasClamp = 0;
        RSDesc.DepthClipEnable = TRUE;
        RSDesc.ScissorEnable = TRUE;
        RSDesc.AntialiasedLineEnable = FALSE;
        RSDesc.MultisampleEnable = (sd.SampleDesc.Count > 1) ? TRUE : FALSE;

        ID3D11RasterizerState* pRState = NULL;
        g_pd3dDevice->CreateRasterizerState(&RSDesc, &pRState);
        g_pd3dDeviceContext->RSSetState(pRState);
        pRState->Release();
    }

    CreateRenderTarget();

    return S_OK;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplDX11_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            ImGui_ImplDX11_InvalidateDeviceObjects();
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
            ImGui_ImplDX11_CreateDeviceObjects();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


Engine::Engine() :  m_IsRunning(false)
{
}

Engine::~Engine()
{
}

Engine* Engine::GetEngine()
{
    static Engine* engine = nullptr;

    if (!engine)
        engine = new Engine();

    return engine;
}

bool Engine::Initialize(HINSTANCE hInstance)
{
    UI::Create();
    UI::Get().Initialize();
	return m_IsRunning = true;	
}

void Engine::Shutdown()
{
    UI::Delete();
}

void Engine::Run()
{
    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, "ML_Engine", NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow("ML_Engine", "ML_Engine", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (CreateDeviceD3D(hwnd) < 0)
    {
        CleanupDeviceD3D();
        UnregisterClass("ML_Engine", wc.hInstance);
    }

    // Show the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Setup ImGui binding
    ImGui_ImplDX11_Init(hwnd, g_pd3dDevice, g_pd3dDeviceContext);

    ImVec4 clear_col = ImColor(255, 255, 255);

	MSG msg = { 0 };

	while (IsRunning())
	{
		// If there are Window messages then process them.
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
			if (msg.message == WM_QUIT)
			{
				m_IsRunning = false;
			}
		}
        ImGui_ImplDX11_NewFrame();
        
        UI::Get().Update();
       
        // Rendering
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_col);
        ImGui::Render();
        g_pSwapChain->Present(0, 0);
	}

    ImGui_ImplDX11_Shutdown();
    CleanupDeviceD3D();
    UnregisterClass("ML_Engine Example", wc.hInstance);
}

bool Engine::IsRunning() const
{
	return m_IsRunning;
}


void Engine::Quit()
{
	m_IsRunning = false;
}
