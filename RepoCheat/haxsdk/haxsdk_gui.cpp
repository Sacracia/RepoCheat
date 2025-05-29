#include "haxsdk_gui.h"

//-------------------------------------------------------------------------
// [SECTION] INCLUDES
//-------------------------------------------------------------------------

// C-Headers
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

// STD Headers
#include <fstream>
#include <format>
#include <algorithm>

// OpenGL
#pragma comment(lib, "OpenGL32.lib")

// DirectX 9
#include <d3d9.h>
#pragma comment(lib, "D3d9.lib")

// DirectX 10
#include <d3d10_1.h>
#include <dxgi.h>
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "dxgi.lib")

// DirectX 11
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

// Directx 12
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#ifdef _VULKAN
// Vulkan
#include <vulkan/vulkan.h>
#pragma comment(lib, "vulkan-1.lib")
#endif

// Detours
#ifdef _WIN64
    #if __has_include("third_party/detours/x64/detours.h")
        #include "haxsdk/third_party/detours/x64/detours.h"
        #pragma comment(lib, "haxsdk/third_party/detours/x64/detours.lib")
    #else
        #include "../third_party/detours/x64/detours.h"
        #pragma comment(lib, "../third_party/detours/x64/detours.lib")
    #endif
#else
    #if __has_include("third_party/detours/x86/detours.h")
        #include "third_party/detours/x86/detours.h"
        #pragma comment(lib, "third_party/detours/x86/detours.lib")
    #else
        #include "../third_party/detours/x86/detours.h"
        #pragma comment(lib, "../third_party/detours/x86/detours.lib")
    #endif
#endif

// Dear ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_internal.h"
#include "third_party/imgui/backend/imgui_impl_dx9.h"
#include "third_party/imgui/backend/imgui_impl_dx10.h"
#include "third_party/imgui/backend/imgui_impl_dx11.h"
#include "third_party/imgui/backend/imgui_impl_dx12.h"
#ifdef _VULKAN
#include "third_party/imgui/backend/imgui_impl_vulkan.h"
#endif
#include "third_party/imgui/backend/imgui_impl_win32.h"
#include "third_party/imgui/backend/imgui_impl_opengl3.h"
#include "third_party/imgui/backend/imgui_impl_opengl3_loader.h"

// STB
#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

// HaxSdk
#include "haxsdk_system.h"
#include "haxsdk_logger.h"

#undef DrawText

//-------------------------------------------------------------------------
// [SECTION] FORWARD DECLARATIONS
//-------------------------------------------------------------------------

// Function types
using t_SetCursorPos            = BOOL(WINAPI*)(int, int); // Type of SetCursorPos function (WinAPI)
using t_ClipCursor              = BOOL(WINAPI*)(const RECT*); // Type of ClipCursor function (WinAPI)
using t_SetPhysicalCursorPos    = BOOL(WINAPI*)(int, int); // Type of SetPhysicalCursorPos function (WinAPI)
using t_mouse_event             = void(WINAPI*)(DWORD, DWORD, DWORD, DWORD, ULONG_PTR); // Type of mouse_event function (WinAPI)
using t_GetCursorPos            = BOOL(WINAPI*)(LPPOINT); // Type of GetCursorPos function (WinAPI)
using t_SendInput               = UINT(WINAPI*)(UINT, LPINPUT, int); // Type of SendInput function (WinAPI)
using t_SendMessageW            = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM); // Type of SendMessageW function (WinAPI)
#ifdef _VULKAN
using t_vkAcquireNextImageKHR   = VkResult(VKAPI_CALL*)(VkDevice, VkSwapchainKHR, uint64_t, VkSemaphore, VkFence, uint32_t*); // Type of vkAcquireNextImageKHR function (Vulkan)
using t_vkAcquireNextImage2KHR  = VkResult(VKAPI_CALL*)(VkDevice, const VkAcquireNextImageInfoKHR*, uint32_t*); // Type of vkAcquireNextImage2KHR function (Vulkan)
using t_vkQueuePresentKHR       = VkResult(VKAPI_CALL*)(VkQueue, const VkPresentInfoKHR*); // Type of vkQueuePresentKHR function (Vulkan)
using t_vkCreateSwapchainKHR    = VkResult(VKAPI_CALL*)(VkDevice, const VkSwapchainCreateInfoKHR*, const VkAllocationCallbacks*, VkSwapchainKHR*); // type of vkCreateSwapchainKHR function (Vulkan)
#endif
using t_SwapBuffers             = bool(WINAPI*)(HDC); // Type of wglSwapBuffers function (OpenGL)
using t_Reset                   = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*); // Type of IDirect3DDevice9::Reset (DirectX 9)
using t_EndScene                = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9); // Type of IDirect3DDevice9::EndScene (DirectX9)
using t_Present                 = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT); // Type of IDXGISwapChain::Present (DXGI)
using t_ResizeBuffers           = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT); // Type of IDXGISwapChain::ResizeBuffers fuction (DXGI)
using t_SetRenderTargets11      = void(WINAPI*)(ID3D11DeviceContext*, UINT, ID3D11RenderTargetView* const*, ID3D11DepthStencilView*); // Type of ID3D11DeviceContext::OMSetRenderTargets (DirectX 11)
using t_ExecuteCommandLists     = void(WINAPI*)(ID3D12CommandQueue*, UINT, ID3D12CommandList*); // Type of ID3D12CommandQueue::ExecuteCommandLists (DirectX 12)
using t_SetRenderTargets12      = void(WINAPI*)(ID3D12GraphicsCommandList*, UINT, const D3D12_CPU_DESCRIPTOR_HANDLE*, BOOL, const D3D12_CPU_DESCRIPTOR_HANDLE*); // Type of ID3D12GraphicsCommandList::OMSetRenderTargets (DirectX 12)

// Combines required parameters from different APIs to create an ImGui context
struct ImGuiContextParams 
{
    GraphicsApi                         m_GraphicsApi;
    LPDIRECT3DDEVICE9                   m_pDevice9;
    HDC                                 m_HDC;
    IDXGISwapChain*                     m_pSwapChain;
};

// General hooks and procedures
static LRESULT WINAPI                   HookedPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags); // Hooked present to render menu in DirectX 10-12
static LRESULT WINAPI                   HookedWndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // Hooked WndProc to send window messages to ImGui menu
static BOOL WINAPI                      HookedSetCursorPos(int X, int Y); // Hooked SetCursorPos to remove forced positioning of the cursor (for example, from the center of the screen for shooters)
static BOOL WINAPI                      HookedClipCursor(const RECT* lpRect); // Hooked ClipCursor to remove forced positioning of the cursor (for example, from the center of the screen for shooters) 
static BOOL WINAPI                      HookedSetPhysicalCursorPos(int x, int y); // Hooked SetPhysicalCursorPos to remove forced positioning of the cursor (for example, from the center of the screen for shooters) 
static UINT WINAPI                      HookedSendInput(UINT cInputs, LPINPUT pInputs, int cbSize); // Hooked SendInput to interrupt the transmission of keystrokes and mouse clicks when the menu is open
static void WINAPI                      HookedMouseEvent(DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, ULONG_PTR dwExtraInfo); // Hooked mouse_event to block mouse input when menu is open
static LRESULT WINAPI                   HookedSendMessageW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam); // Hooked SendMessageW to intercept window messages

static void                             InitImGuiContext(const ImGuiContextParams& params); // Single function used by all API hooks to initialize ImGui context

namespace opengl 
{
    static void                         Hook(); // Setting up menu rendering for OpenGL. Finding and hooking all functions that are needed
    static bool WINAPI                  HookedSwapBuffers(HDC hdc); // Hooked SwapBuffers to render cheat menu in OpenGL
    static HaxTexture                   LoadTextureFromResource(int32_t id); // Load image from embedded resource
}

namespace dx9 
{
    static void                         Hook(); // Setting up menu rendering for DirectX 9. Finding and hooking all functions that are needed
    static HRESULT WINAPI               HookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters); // Hooked Reset to track when game window was resized in order to recreate ImGui devices.
    static HRESULT WINAPI               HookedEndScene(LPDIRECT3DDEVICE9 pDevice); // Hooked EndScene to render menu in DX9
}

namespace dx10 
{
    static void                         Setup(); // Setting up menu to render in DirectX 10. Finding and hooking all functions that are needed
    static void                         Render(IDXGISwapChain* pSwapChain); // Render ImGui cheat menu in DirectX 10
    static void                         CreateRenderTarget(IDXGISwapChain* pSwapChain); // Create render target for DirectX 10
    static HRESULT WINAPI               HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);  // Hooked ResizeBuffers to track when game window was resized 
                                                                                                                                                                                // in order to recreate ImGui render target.
}

namespace dx11 
{
    static void                         Setup(); // Setting up menu to render in DirectX 11. Finding and hooking all functions that are needed
    static void                         Render(IDXGISwapChain* pSwapChain); // Render ImGui cheat menu in DirectX 11
    static void                         CreateRenderTarget(IDXGISwapChain* pSwapChain); // Create render target for DirectX 11
    static HRESULT WINAPI               HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags); // Hooked ResizeBuffers to track when game window was resized 
                                                                                                                                                                                // in order to recreate ImGui render target.
    static HaxTexture                   LoadTextureFromResource(int32_t id); // Load image from embedded resource
}

namespace dx12 
{
    #include <dxgi1_4.h>
    static void                         Setup(); // Setting up menu to render in DirectX 12. Finding and hooking all functions that are needed
    static void                         Render(IDXGISwapChain3* pSwapChain); // Render ImGui cheat menu in DirectX 12
    static void                         CreateRenderTarget(IDXGISwapChain* pSwapChain); // Create render target for DirectX 12
    static HRESULT WINAPI               HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, 
                                                            UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags); // Hooked ResizeBuffers to track when game window was resized in order to recreate ImGui render target.
    static void WINAPI                  HookedExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists); // Hooked ExecuteCommandLists for getting CommandQueue
}

#ifdef _VULKAN
namespace vulkan
{
    static void                         Setup(); // Setting up menu to render in Vulkan. Finding and hooking all functions that are needed
    static bool                         CreateDevice(); // Create Vulkan logical device
    static void                         CleanupRenderTarget(); // Cleanup Vulkan render targets
    static void                         CreateRenderTarget(VkDevice device, VkSwapchainKHR swapchain); // Create Vulkan render target
    static bool                         DoesQueueSupportGraphic(VkQueue queue, VkQueue* pGraphicQueue); // Check if queue supports graphics
    static void                         RenderImGui_Vulkan(VkQueue queue, const VkPresentInfoKHR* pPresentInfo); // Render ImGui menu in Vulkan
}
#endif

// Global variables
static GraphicsApi                      g_GraphicsApi;          // Current graphics API being used
static HWND                             g_DummyHWND;            // Dummy window for initialization
static WNDPROC                          oWndproc;               // Original window procedure
static t_SetCursorPos                   oSetCursorPos;          // Original SetCursorPos function
static t_ClipCursor                     oClipCursor;            // Original ClipCursor function
static t_SetPhysicalCursorPos           oSetPhysicalCursorPos;  // Original SetPhysicalCursorPos function
static t_mouse_event                    oMouseEvent;            // Original mouse_event function
static t_GetCursorPos                   oGetCursorPos;          // Original GetCursorPos function
static t_SendInput                      oSendInput;             // Original SendInput function
static t_SendMessageW                   oSendMessageW;          // Original SendMessageW function
static t_SwapBuffers                    oSwapBuffers;           // Original SwapBuffers function (OpenGL)
static t_Present                        oPresent;               // Original Present function (DX)
static t_ResizeBuffers                  oResizeBuffers;         // Original ResizeBuffers function (DX)
#ifdef _VULKAN
static t_vkAcquireNextImageKHR          oVkAcquireNextImageKHR; // Original vkAcquireNextImageKHR function (Vulkan)
static t_vkAcquireNextImage2KHR         oVkAcquireNextImage2KHR;// Original vkAcquireNextImage2KHR function (Vulkan)
static t_vkQueuePresentKHR              oVkQueuePresentKHR;     // Original vkQueuePresentKHR function (Vulkan)
static t_vkCreateSwapchainKHR           oVkCreateSwapchainKHR;  // Original vkCreateSwapchainKHR function (Vulkan)
#endif
static bool                             g_MenuVisible = true;   // Menu visibility flag
static bool                             g_MenuDisabled = false;
static int                              g_MenuHotkey = 0xC0;    // Hotkey to toggle menu (~ key by default)
static float                            g_ScaleFactor = 1.f;    // UI scaling factor
static HWND                             g_GameHwnd;             // Game window handle
static HANDLE                           g_CheatHandle;          // Handle to cheat thread
static std::function<void()>            g_FnMenuRender;         // Menu rendering callback
static std::function<void()>            g_FnBackground;         // Background rendering callback
static std::function<void()>            g_FnInitialize;         // Initialization callback
static unsafe::Thread*                  g_VMThread;             // Unity VM thread

static HANDLE                           hRenderSemaphore;       // Semaphore for render thread synchronization
constexpr DWORD                         MAX_RENDER_THREAD_COUNT = 5; // Maximum concurrent render threads

namespace dx9 
{
    static t_Reset                      oReset;                 // Original Reset function (DX9)
    static t_EndScene                   oEndScene;              // Original EndScene function (DX9)
}
namespace dx10 
{
    static ID3D10RenderTargetView*      g_pRenderTarget;        // DX10 render target view
    static ID3D10Device*                g_pDevice;              // DX10 device
}
namespace dx11 
{
    static ID3D11RenderTargetView*      g_pRenderTarget;        // DX11 render target view
    static ID3D11Device*                g_pDevice;              // DX11 device
    static ID3D11DeviceContext*         g_pDeviceContext;       // DX11 device context
}
namespace dx12 
{
    static int const                    NUM_BACK_BUFFERS = 3;   // Number of back buffers
    static IDXGIFactory4*               g_dxgiFactory = NULL;   // DXGI factory
    static ID3D12Device*                g_pd3dDevice = NULL;    // DX12 device
    static ID3D12DescriptorHeap*        g_pd3dRtvDescHeap = NULL; // Render target view descriptor heap
    static ID3D12DescriptorHeap*        g_pd3dSrvDescHeap = NULL; // Shader resource view descriptor heap
    static ID3D12CommandQueue*          g_pd3dCommandQueue = NULL;// DX12 command queue
    static ID3D12GraphicsCommandList*   g_pd3dCommandList = NULL; // DX12 command list
    static IDXGISwapChain3*             g_pSwapChain = NULL;    // DX12 swap chain
    static ID3D12CommandAllocator*      g_commandAllocators[NUM_BACK_BUFFERS] = { }; // Command allocators for each back buffer
    static ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS] = { }; // Back buffer resources
    static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = { }; // Back buffer descriptors
    static t_ExecuteCommandLists        oExecuteCommandLists; // Original ExecuteCommandLists function (DX12)
}
#ifdef _VULKAN
namespace vulkan
{
    static VkAllocationCallbacks*       g_Allocator = NULL;         // Vulkan memory allocator
    static VkInstance                   g_Instance = VK_NULL_HANDLE;    // Vulkan instance
    static VkPhysicalDevice             g_PhysicalDevice = VK_NULL_HANDLE; // Vulkan physical device
    static VkDevice                     g_FakeDevice = VK_NULL_HANDLE, g_Device = VK_NULL_HANDLE; // Vulkan logical devices

    static uint32_t                     g_QueueFamily = (uint32_t)-1; // Graphics queue family index
    static std::vector<VkQueueFamilyProperties> g_QueueFamilies; // Available queue families

    static VkPipelineCache              g_PipelineCache = VK_NULL_HANDLE; // Pipeline cache
    static VkDescriptorPool             g_DescriptorPool = VK_NULL_HANDLE; // Descriptor pool
    static uint32_t                     g_MinImageCount = 2; // Minimum swapchain image count
    static VkRenderPass                 g_RenderPass = VK_NULL_HANDLE; // Render pass
    static ImGui_ImplVulkanH_Frame      g_Frames[8] = { }; // Per-frame data
    static ImGui_ImplVulkanH_FrameSemaphores g_FrameSemaphores[8] = { }; // Per-frame semaphores
    static VkExtent2D                   g_ImageExtent = {}; // Swapchain image dimensions
}
#endif

// ImGui window message handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//-------------------------------------------------------------------------
// [SECTION] GUI General functions
//-------------------------------------------------------------------------

// GUI module entry point
void HaxSdk::ImplementImGui(GraphicsApi graphics, std::function<void()> fnInit, std::function<void()> fnBackground, std::function<void()> fnRender) {
    g_GraphicsApi = graphics;
    g_FnInitialize = fnInit;
    g_FnBackground = fnBackground;
    g_FnMenuRender = fnRender;

    // Finding game HWND
    WNDENUMPROC enumProc = [](HWND hwnd, LPARAM lParam) -> BOOL
    {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        
        if (pid != GetCurrentProcessId())
            return TRUE;

        wchar_t buff[64]{0};
        GetClassNameW(hwnd, buff, sizeof(buff) / sizeof(wchar_t));
        if (wcscmp(buff, L"UnityWndClass") == 0)
        {
            g_GameHwnd = hwnd;
            return FALSE;
        }
        return TRUE;
    };
    EnumWindows(enumProc, 0);

    // Making dummy window for creating device, etc. 
    // Deprecated since we find game hwnd that can be used instead.
    WNDCLASSEX dummyWindow{};
    dummyWindow.cbSize = 0;
    g_DummyHWND = GetConsoleWindow();
    if (!g_DummyHWND) 
    {
        dummyWindow.cbSize = sizeof(WNDCLASSEX);
        dummyWindow.style = CS_HREDRAW | CS_VREDRAW;
        dummyWindow.lpfnWndProc = DefWindowProc;
        dummyWindow.cbClsExtra = 0;
        dummyWindow.cbWndExtra = 0;
        dummyWindow.hInstance = GetModuleHandle(NULL);
        dummyWindow.hIcon = NULL;
        dummyWindow.hCursor = NULL;
        dummyWindow.hbrBackground = NULL;
        dummyWindow.lpszMenuName = nullptr;
        dummyWindow.lpszClassName = "Dummy window";
        dummyWindow.hIconSm = NULL;

        RegisterClassEx(&dummyWindow);
        g_DummyHWND = CreateWindow(dummyWindow.lpszClassName, "Dummy window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, dummyWindow.hInstance, nullptr);
    }

    // Find all graphics api used by the game and hook them all.
    // Idea: only one of the Render functions is working. Thats why we can hook all of them.
    DWORD processId = GetProcessId(GetCurrentProcess());
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

    MODULEENTRY32 me{};
    me.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(snapshot, &me)) 
    {
        do 
        {
            std::string moduleName(me.szModule);
            std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::tolower);
            if (moduleName == "opengl32.dll" && (g_GraphicsApi & GraphicsApi_OpenGL))
            {
                opengl::Hook();
            }
            if (moduleName == "d3d9.dll" && (g_GraphicsApi & GraphicsApi_DirectX9)) 
            {
                dx9::Hook();
            }
            if (moduleName == "d3d10.dll" && (g_GraphicsApi & GraphicsApi_DirectX10)) 
            {
                dx10::Setup();
            }
            if (moduleName == "d3d11.dll" && (g_GraphicsApi & GraphicsApi_DirectX11)) 
            {
                dx11::Setup();
            }
            if (moduleName == "d3d12.dll" && (g_GraphicsApi & GraphicsApi_DirectX12)) 
            {
                dx12::Setup();
            }
#ifdef _VULKAN
            if (moduleName == "vulkan-1.dll" && (g_GraphicsApi & GraphicsApi_Vulkan)) 
            {
                vulkan::Setup();
            }
#endif
        } while (Module32Next(snapshot, &me));
    }
    CloseHandle(snapshot);

    // Getting user screen resolution and calculating UI scale factor.
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    g_ScaleFactor = std::round((float)desktop.bottom / 1080.f);

    // We dont need dummy window anymore
    if (dummyWindow.cbSize > 0) {
        DestroyWindow(g_DummyHWND);
        UnregisterClass(dummyWindow.lpszClassName, dummyWindow.hInstance);
    }

    // If DXGI Present is used by DirectX 10 / 11 / 12, we hook it.
    if (oPresent) 
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)oPresent, HookedPresent);
        DetourTransactionCommit();
    }
}

// API function to create image from an embedded resource
HaxTexture HaxSdk::LoadTextureFromResource(int32_t id) 
{
    if (g_GraphicsApi == GraphicsApi_DirectX11)
        return dx11::LoadTextureFromResource(id);
    if (g_GraphicsApi == GraphicsApi_OpenGL)
        return opengl::LoadTextureFromResource(id);
    assert(false && "Not implemented");
    return HaxTexture();
}

// Settings cheat handleю Used from the entry point (dllmain)
void HaxSdk::SetCheatHandle(HANDLE handle)
{
    g_CheatHandle = handle;
}

// Getter for menu key binding
int HaxSdk::GetMenuHotkey()
{
    return g_MenuHotkey;
}

// Setter for menu key binding
void HaxSdk::SetMenuHotkey(int hotkey)
{
    g_MenuHotkey = hotkey;
}

// Getter for UI scale factor
float HaxSdk::GetScaleFactor()
{
    return g_ScaleFactor;
}

//-------------------------------------------------------------------------
// [SECTION] ImGui widgets extention
//-------------------------------------------------------------------------

void HaxSdk::Widgets::HelpMarker(const char* tip)
{
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    Tooltip(tip);
}

void HaxSdk::Widgets::Tooltip(const char* tip)
{
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) 
    {
        ImGui::BeginTooltip();
        ImGui::Text(tip);
        ImGui::EndTooltip();
    }
}

void HaxSdk::Widgets::WarningMarker(const char* tip)
{
    ImGui::SameLine();
    ImGui::TextDisabled("(!)");
    Tooltip(tip);
}

bool HaxSdk::Widgets::DisabledButton(const char* label, bool enabled)
{
    ImGui::BeginDisabled(!enabled);
    bool pressed = ImGui::Button(label);
    ImGui::EndDisabled();
    return pressed;
}

bool HaxSdk::Widgets::DisabledCheckbox(const char* label, bool* value, bool enabled)
{
    bool tmp = false;
    enabled &= value != nullptr;

    if (value && !enabled)
        *value = false;

    ImGui::BeginDisabled(!enabled);
    bool pressed = ImGui::Checkbox(label, enabled ? value : &tmp);
    ImGui::EndDisabled();
    return pressed;
}

bool HaxSdk::Widgets::PremiumButton(const char* label, bool enabled, HaxTexture& watermark, const char* wmText)
{
    static float wmSize = ImGui::CalcTextSize("Text").y + ImGui::GetStyle().FramePadding.y * 2.f;
#ifdef _FREE
    ImGui::BeginDisabled(true);
    ImGui::Button(label);
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::Image(watermark.m_pTexture, ImVec2(wmSize, wmSize));
    Tooltip(wmText);
    return false;
#else
    return DisabledButton(label, enabled);
#endif
}

bool HaxSdk::Widgets::PremiumCheckbox(const char* label, bool* value, bool enabled, HaxTexture& watermark, const char* wmText)
{
    static float wmSize = ImGui::CalcTextSize("Text").y + ImGui::GetStyle().FramePadding.y * 2.f;
#ifdef _FREE
    bool tmp = false;
    ImGui::BeginDisabled();
    ImGui::Checkbox(label, &tmp);
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::Image(watermark.m_pTexture, ImVec2(wmSize, wmSize));
    Tooltip(wmText);
    return false;
#else
    return DisabledCheckbox(label, value, enabled);
#endif
}

//-------------------------------------------------------------------------
// [SECTION] ESP supportive functions
//-------------------------------------------------------------------------

void HaxSdk::ESP::CornerBox(float x, float y, float width, float height, ImU32 color, float cornerProp)
{
    float yOffset = height * cornerProp;
    float xOffset = width * cornerProp;

    ImVec2 topLeft[] = { ImVec2(x, y + yOffset), ImVec2(x, y), ImVec2(x + xOffset, y) };
    ImGui::GetBackgroundDrawList()->AddPolyline(topLeft, 3, 0xFF000000, 0, 3.f);
    ImGui::GetBackgroundDrawList()->AddPolyline(topLeft, 3, color, 0, 1.f);
    ImVec2 topRight[] = { ImVec2(x + width - xOffset, y), ImVec2(x + width, y), ImVec2(x + width, y + yOffset) };
    ImGui::GetBackgroundDrawList()->AddPolyline(topRight, 3, 0xFF000000, 0, 3.f);
    ImGui::GetBackgroundDrawList()->AddPolyline(topRight, 3, color, 0, 1.f);
    ImVec2 bottomRight[] = { ImVec2(x + width - xOffset, y + height), ImVec2(x + width, y + height), ImVec2(x + width, y + height - yOffset) };
    ImGui::GetBackgroundDrawList()->AddPolyline(bottomRight, 3, 0xFF000000, 0, 3.f);
    ImGui::GetBackgroundDrawList()->AddPolyline(bottomRight, 3, color, 0, 1.f);
    ImVec2 bottomLeft[] = { ImVec2(x, y + height - yOffset), ImVec2(x, y + height), ImVec2(x + xOffset, y + height) };
    ImGui::GetBackgroundDrawList()->AddPolyline(bottomLeft, 3, 0xFF000000, 0, 3.f);
    ImGui::GetBackgroundDrawList()->AddPolyline(bottomLeft, 3, color, 0, 1.f);
}

void HaxSdk::ESP::Box(float x, float y, float width, float height, ImU32 col, float rounding, float thickness)
{
    ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x, y) + ImVec2(width, height), col, rounding, 0, thickness);
}

void HaxSdk::ESP::Text(ImFont* font,
                           const char* text,
                           const ImVec2& pos,
                           ImU32 col,
                           float fontSize,
                           VerticalAlignment vAlign,
                           HorizontalAlignment hAlign)
{
    ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);
    float xShift = hAlign == HorizontalAlignment_Right ? 0.f : textSize.x / (float)hAlign;
    float yShift = vAlign == VerticalAlignment_Bottom ? 0.f : textSize.y / (float)vAlign;

    ImDrawList* pDrawList = ImGui::GetBackgroundDrawList();
    pDrawList->AddText(font, fontSize, ImVec2(pos.x - xShift + 1.F, pos.y - yShift - 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(font, fontSize, ImVec2(pos.x - xShift + 1.F, pos.y - yShift + 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(font, fontSize, ImVec2(pos.x - xShift - 1.F, pos.y - yShift + 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(font, fontSize, ImVec2(pos.x - xShift - 1.F, pos.y - yShift - 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(font, fontSize, ImVec2(pos.x - xShift, pos.y - yShift), col, text);
}

void HaxSdk::ESP::VertHealthBar(float x, float y, float width, float height, float cur, float max)
{
    float coef = cur / max;
    ImU32 lerped = ImGui::ColorConvertFloat4ToU32(ImLerp(ImVec4(1.f, 0.f, 0.f, 1.f), ImVec4(0.f, 1.f, 0.f, 1.f), coef));
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + height), 0xFF000000, width + 2.f);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + height - height * coef), ImVec2(x, y + height), lerped, width);
}

void HaxSdk::ESP::VertHealthBar(float x, float y, float width, float height, int cur, int max)
{
    VertHealthBar(x, y, width, height, (float)cur, (float)max);
}

//-------------------------------------------------------------------------
// [SECTION] General API implementation
//-------------------------------------------------------------------------

// DirectX 10-12 use the same DXGI Present function to render frames.
// To distinguish between them, we check the render device interface
static LRESULT WINAPI HookedPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) 
{
    static bool s_Initialized = false;
    if (!s_Initialized)
    {
        s_Initialized = true;
         ID3D10Device* pDevice10;
         ID3D11Device* pDevice11;
         ID3D12Device* pDevice12;
         DetourTransactionBegin();
         DetourUpdateThread(GetCurrentThread());
         if ((g_GraphicsApi & GraphicsApi_DirectX10) && pSwapChain->GetDevice(__uuidof(pDevice10), (void**)&pDevice10) == S_OK) 
         {
             g_GraphicsApi = GraphicsApi_DirectX10;
             HAX_LOG("GAME USES DIRECTX10");
             DetourAttach(&(PVOID&)oResizeBuffers, dx10::HookedResizeBuffers);
         }
         else if ((g_GraphicsApi & GraphicsApi_DirectX11) && pSwapChain->GetDevice(__uuidof(pDevice11), (void**)&pDevice11) == S_OK) 
         {
             g_GraphicsApi = GraphicsApi_DirectX11;
             HAX_LOG("GAME USES DIRECTX11");
             DetourAttach(&(PVOID&)oResizeBuffers, dx11::HookedResizeBuffers);
         }
         else if ((g_GraphicsApi & GraphicsApi_DirectX12) && pSwapChain->GetDevice(__uuidof(pDevice12), (void**)&pDevice12) == S_OK) 
         {
             g_GraphicsApi = GraphicsApi_DirectX12;
             HAX_LOG("GAME USES DIRECTX12");
             DetourAttach(&(PVOID&)oResizeBuffers, dx12::HookedResizeBuffers);
             DetourAttach(&(PVOID&)dx12::oExecuteCommandLists, dx12::HookedExecuteCommandLists);
         }
         else
             g_GraphicsApi = GraphicsApi_None;
         DetourTransactionCommit();
    }

    WaitForSingleObject(hRenderSemaphore, INFINITE);
    if (g_GraphicsApi & GraphicsApi_DirectX10) {
        dx10::Render(pSwapChain);
    }
    else if (g_GraphicsApi & GraphicsApi_DirectX11) {
        dx11::Render(pSwapChain);
    }
    else if (g_GraphicsApi & GraphicsApi_DirectX12) {
        dx12::Render((dx12::IDXGISwapChain3*)pSwapChain);
    }

    HRESULT result = oPresent(pSwapChain, syncInterval, flags);
    ReleaseSemaphore(hRenderSemaphore, 1, NULL);
    return result;
}

static void InitImGuiContext(const ImGuiContextParams& params) 
{
    //HWND hwnd = 0;
    if (params.m_GraphicsApi & GraphicsApi_OpenGL) 
    {
        //hwnd = WindowFromDC(params.hdc);
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
        ImGui_ImplOpenGL3_Init();
    }
    if (params.m_GraphicsApi & GraphicsApi_DirectX9) 
    {
        D3DDEVICE_CREATION_PARAMETERS creationParams;
        params.m_pDevice9->GetCreationParameters(&creationParams);
        //hwnd = creationParams.hFocusWindow;
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
        ImGui_ImplDX9_Init(params.m_pDevice9);
    }
    if (params.m_GraphicsApi & GraphicsApi_DirectX10) 
    {
        /*DXGI_SWAP_CHAIN_DESC swapChainDesc;
        params.m_pSwapChain->GetDevice(IID_PPV_ARGS(&dx10::g_pDevice));
        params.m_pSwapChain->GetDesc(&swapChainDesc);
        hwnd = swapChainDesc.OutputWindow;*/
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
        ImGui_ImplDX10_Init(dx10::g_pDevice);
    }
    if (params.m_GraphicsApi & GraphicsApi_DirectX11) 
    {
        //DXGI_SWAP_CHAIN_DESC swapChainDesc;
        params.m_pSwapChain->GetDevice(IID_PPV_ARGS(&dx11::g_pDevice));
        //params.m_pSwapChain->GetDesc(&swapChainDesc);
        dx11::g_pDevice->GetImmediateContext(&dx11::g_pDeviceContext);
        //hwnd = swapChainDesc.OutputWindow;
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
        ImGui_ImplDX11_Init(dx11::g_pDevice, dx11::g_pDeviceContext);
    }
    if (params.m_GraphicsApi & GraphicsApi_DirectX12) 
    {
        /*DXGI_SWAP_CHAIN_DESC swapChainDesc;
        params.m_pSwapChain->GetDesc(&swapChainDesc);
        hwnd = swapChainDesc.OutputWindow;*/
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
    }
    if (params.m_GraphicsApi & GraphicsApi_Vulkan)
    {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(g_GameHwnd);
    }
    
    // Attach rendering thread to Unity virtual machine
    g_VMThread = unsafe::Thread::Attach();

    // We use custom ini file thats why we dont need ImGui one.
    ImGui::GetIO().IniFilename = nullptr;

    // Initialize user data
    if (g_FnInitialize)
        g_FnInitialize();

    // ImGui customization
    ImGuiIO& io = ImGui::GetIO();
    io.WantCaptureMouse = true;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos; 
    ImGui::GetStyle().ScaleAllSizes(g_ScaleFactor);

    // Hooking WndProc
    oWndproc = (WNDPROC)SetWindowLongPtr(g_GameHwnd, GWLP_WNDPROC, (LONG_PTR)HookedWndproc);

    // Hooking all WinAPI functions.
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    if (HMODULE hModule = GetModuleHandleA("user32.dll")) 
    {
        if (oClipCursor = (t_ClipCursor)GetProcAddress(hModule, "ClipCursor"))
            ::DetourAttach(&(PVOID&)oClipCursor, HookedClipCursor);
        else
            HAX_LOG_ERROR("Unable to hook ClipCursor");

        if (oSetPhysicalCursorPos = (t_SetPhysicalCursorPos)GetProcAddress(hModule, "SetPhysicalCursorPos"))
            ::DetourAttach(&(PVOID&)oSetPhysicalCursorPos, HookedSetPhysicalCursorPos);
        else
            HAX_LOG_ERROR("Unable to hook SetPhysicalCursorPos");

        if (oSetCursorPos = (t_SetCursorPos)GetProcAddress(hModule, "SetCursorPos"))
            ::DetourAttach(&(PVOID&)oSetCursorPos, HookedSetCursorPos);
        else
            HAX_LOG_ERROR("Unable to hook SetCursorPos");

        if (oMouseEvent = (t_mouse_event)GetProcAddress(hModule, "mouse_event"))
            ::DetourAttach(&(PVOID&)oMouseEvent, HookedMouseEvent);
        else
            HAX_LOG_ERROR("Unable to hook mouse_event");

        if (oSendInput = (t_SendInput)GetProcAddress(hModule, "SendInput"))
            ::DetourAttach(&(PVOID&)oSendInput, HookedSendInput);
        else
            HAX_LOG_ERROR("Unable to hook SendInput");

        if (oSendMessageW = (t_SendMessageW)GetProcAddress(hModule, "SendMessageW"))
            ::DetourAttach(&(PVOID&)oSendMessageW, HookedSendMessageW);
        else
            HAX_LOG_ERROR("Unable to hook SendMessageW");
    }
    DetourTransactionCommit();

    hRenderSemaphore = CreateSemaphore(
        NULL,                                 // default security attributes
        MAX_RENDER_THREAD_COUNT,              // initial count
        MAX_RENDER_THREAD_COUNT,              // maximum count
        NULL                                  // unnamed semaphore);
    );
}

static LRESULT WINAPI HookedWndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    // Setting correct mouse position
    ImGuiIO& io = ImGui::GetIO();
    POINT position;
    GetCursorPos(&position);
    ScreenToClient(hWnd, &position);
    io.MousePos.x = (float)position.x;
    io.MousePos.y = (float)position.y;

    // Handle menu keybinding is pressed
    if (uMsg == WM_KEYUP && wParam == g_MenuHotkey) 
    {
        g_MenuVisible = !g_MenuVisible;
        io.MouseDrawCursor = g_MenuVisible;
    }

    // [TO-BE-DONE] Shutdown the cheat when game is closing
    if (uMsg == WM_CLOSE) 
    {
        // Detach rendering thread from unity as it may block the game from closing.
        if (g_VMThread)
        {
            /*g_MenuDisabled = true;
            g_VMThread->Detach();*/
        }
        HAX_LOG_DEBUG("Got WM_QUIT message. Cheat should shut down");
    }

    // If the menu is visible, handle all incoming window messages
    if (g_MenuVisible) 
    {
        RECT rect;
        GetWindowRect(hWnd, &rect);
        HookedClipCursor(&rect);

        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        switch (uMsg) {
        case WM_KEYUP: 
            if (wParam != g_MenuHotkey) break;
        case WM_MOUSEMOVE:
        case WM_MOUSEACTIVATE:
        case WM_MOUSEHOVER:
        case WM_NCHITTEST:
        case WM_NCMOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_INPUT:
        case WM_TOUCH:
        case WM_POINTERDOWN:
        case WM_POINTERUP:
        case WM_POINTERUPDATE:
        case WM_NCMOUSELEAVE:
        case WM_MOUSELEAVE:
        case WM_SETCURSOR:
        case WM_SIZE:
        case WM_MOVE:
            return true;
        }
    }
    return CallWindowProc(oWndproc, hWnd, uMsg, wParam, lParam);
}

static BOOL WINAPI HookedSetCursorPos(int X, int Y) 
{
    return g_MenuVisible ? true : oSetCursorPos(X, Y);
}

static BOOL WINAPI HookedClipCursor(const RECT* lpRect) 
{
    return oClipCursor(g_MenuVisible ? NULL : lpRect);
}

static BOOL WINAPI HookedSetPhysicalCursorPos(int x, int y) 
{
    return g_MenuVisible ? true : oSetPhysicalCursorPos(x, y);
}

static UINT WINAPI HookedSendInput(UINT cInputs, LPINPUT pInputs, int cbSize) 
{
    return g_MenuVisible ? TRUE : oSendInput(cInputs, pInputs, cbSize);
}

static void WINAPI HookedMouseEvent(DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, ULONG_PTR dwExtraInfo) 
{
    if (!g_MenuVisible) { oMouseEvent(dwFlags, dx, dy, dwData, dwExtraInfo); }
}

static LRESULT WINAPI HookedSendMessageW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) 
{
    return (g_MenuVisible && Msg == 0x20) ? TRUE : oSendMessageW(hWnd, Msg, wParam, lParam);
}

//-------------------------------------------------------------------------
// [SECTION] Graphics API implementation [OPENGL]
//-------------------------------------------------------------------------

namespace opengl
{
    // Installs a hook on the wglSwapBuffers function, which is responsible for finalizing frame rendering in OpenGL.
    // This allows injecting ImGui rendering at the end of each frame.
    static void Hook()
    {
        HMODULE module = GetModuleHandleA("opengl32.dll");

        // Get the address of the original wglSwapBuffers function
        oSwapBuffers = (t_SwapBuffers)GetProcAddress(module, "wglSwapBuffers");
        if (!oSwapBuffers)
        {
            HAX_LOG_ERROR("[OPENGL] Unable to find wglSwapBuffers. Hook not installed");
            return;
        }

        HAX_LOG_DEBUG("[OPENGL] wglSwapBuffers address is {}", (void*)oSwapBuffers);

        // Attach our hooked function to the original one using MS Detours
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)oSwapBuffers, HookedSwapBuffers);
        DetourTransactionCommit();
    }

    // Replacement for the original wglSwapBuffers.
    // This function is called every frame and is used to render ImGui overlays.
    static bool WINAPI HookedSwapBuffers(HDC hdc)
    {
        // Initialize ImGui context if it hasn't been done yet
        if (!ImGui::GetCurrentContext())
        {
            ImGuiContextParams params = { GraphicsApi_OpenGL, nullptr, hdc, nullptr };
            InitImGuiContext(params);
            HAX_LOG("GAME USES OPENGL");
        }

        // Start a new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        try
        {
            // Optional background render callback
            if (!g_MenuDisabled && g_FnBackground)
                g_FnBackground();

            // Render the menu if it is visible
            if (!g_MenuDisabled && g_MenuVisible && g_FnMenuRender)
                g_FnMenuRender();
        }
        catch (System::Exception& ex)
        {
            // Handle and log exceptions thrown from user callbacks
            System::String message = ex.GetMessage();
            HaxSdk::LogError(std::format("Line {}: {}", System::Exception::s_Line, message ? message.UTF8() : "Exception without message"));
        }

        // Finalize and render ImGui draw data
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Call the original wglSwapBuffers to complete frame presentation
        return oSwapBuffers(hdc);
    }

    // Loads an image resource from the module and uploads it as an OpenGL texture.
    static HaxTexture LoadTextureFromResource(int32_t id)
    {
        LPVOID pointerToResource = nullptr;
        DWORD sizeOfResource;
        HMODULE hCheatModule = (HMODULE)g_CheatHandle;

        // Locate and load PNG resource from the current module
        HRSRC hResInfo = FindResourceW(hCheatModule, MAKEINTRESOURCEW(id), L"PNG");
        if (hResInfo)
        {
            HGLOBAL hResData = LoadResource(hCheatModule, hResInfo);
            if (hResData)
            {
                pointerToResource = LockResource(hResData);
                sizeOfResource = SizeofResource(hCheatModule, hResInfo);
            }
        }

        assert(pointerToResource && "Invalid image");

        HaxTexture result{};
        int width, height;

        // Decode image using stb_image
        auto image_data = stbi_load_from_memory((stbi_uc*)pointerToResource, sizeOfResource, &width, &height, NULL, 4);
        result.m_height = static_cast<float>(height);
        result.m_width = static_cast<float>(width);

        // Generate and configure OpenGL texture
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int32_t>(result.m_width), static_cast<int32_t>(result.m_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

        // Free image data and return result
        stbi_image_free(image_data);
        result.m_pTexture = (void*)(intptr_t)image_texture;

        return result;
    }
} // opengl


//-------------------------------------------------------------------------
// [SECTION] Graphics API implementation [DIRECTX9]
//-------------------------------------------------------------------------

namespace dx9
{
    // Initializes the hook for DirectX 9 by creating a dummy device and accessing its vtable.
    static void Hook()
    {
        HMODULE module = ::GetModuleHandle("d3d9.dll");

        // Create dummy Direct3D9 object to access vtable
        LPDIRECT3D9 d3d9 = ::Direct3DCreate9(D3D_SDK_VERSION);
        if (d3d9 == NULL)
        {
            HAX_LOG_ERROR("[D3D9] Direct3DCreate9 failed. Hook not installed");
            return;
        }

        // Create a dummy Direct3D device using a hidden dummy window
        LPDIRECT3DDEVICE9 dummyDev = NULL;
        D3DPRESENT_PARAMETERS d3dpp = {};
        d3dpp.Windowed = false;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = g_DummyHWND;
        HRESULT result = d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dummyDev);
        if (result != D3D_OK)
        {
            HAX_LOG_ERROR("[D3D9] IDirect3D9::CreateDevice returned {}. Hook not installed", result);
            d3d9->Release();
            return;
        }

        // Extract EndScene and Reset function addresses from the device's virtual table
        void** pVTable = *(void***)(dummyDev);
        oEndScene = (t_EndScene)pVTable[42];
        oReset = (t_Reset)pVTable[16];

        HAX_LOG_DEBUG("[D3D9] IDirect3DDevice9::EndScene address is {}", (void*)oEndScene);
        HAX_LOG_DEBUG("[D3D9] IDirect3DDevice9::Reset address is {}", (void*)oReset);

        dummyDev->Release();
        d3d9->Release();

        // Hook both EndScene and Reset functions using MS Detours
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        ::DetourAttach(&(PVOID&)oEndScene, HookedEndScene);
        ::DetourAttach(&(PVOID&)oReset, HookedReset);
        DetourTransactionCommit();
    }

    // Called instead of the original EndScene every frame.
    // This is where the ImGui menu is rendered.
    static HRESULT WINAPI HookedEndScene(LPDIRECT3DDEVICE9 pDevice)
    {
        static bool s_Initialized = false;
        if (!s_Initialized)
        {
            s_Initialized = true;

            // Initialize ImGui context with DirectX9 parameters
            ImGuiContextParams params = { GraphicsApi_DirectX9, pDevice, 0, nullptr };
            InitImGuiContext(params);
            HAX_LOG("GAME USES DIRECTX9");
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Execute menu and background callbacks
        try
        {
            if (!g_MenuDisabled && g_FnBackground)
                g_FnBackground();

            if (!g_MenuDisabled && g_MenuVisible && g_FnMenuRender)
                g_FnMenuRender();
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            HaxSdk::LogError(message ? message.UTF8() : "Exception without message");
        }

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

        return oEndScene(pDevice);
    }

    // Called when the game resets the DirectX device (e.g., after resolution change).
    // This invalidates and re-creates ImGui's device-dependent resources.
    static HRESULT WINAPI HookedReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        HRESULT result = oReset(pDevice, pPresentationParameters);
        ImGui_ImplDX9_CreateDeviceObjects();
        return result;
    }
}

//-------------------------------------------------------------------------
// [SECTION] Graphics API implementation [DIRECTX10]
//-------------------------------------------------------------------------

namespace dx10 
{
    // Grabs pointers to Present and ResizeBuffers via dummy swap chain to allow hooking.
    static void Setup() 
    {
        if (oPresent && oResizeBuffers)
            return;

        HMODULE module = GetModuleHandle("d3d10.dll");

        // Create a dummy DirectX 10 device and swap chain to retrieve vtable
        DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = g_DummyHWND;
        swapChainDesc.SampleDesc.Count = 1;

        IDXGISwapChain* pSwapChain;
        ID3D10Device* pDevice;
        HRESULT result = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_NULL, NULL, 0, D3D10_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice);
        if (result != S_OK) {
            HAX_LOG_ERROR("[D3D10] D3D10CreateDeviceAndSwapChain returned {}. Hook not installed", result);
            return;
        }

        // Get function pointers from the swap chain's vtable
        void** pVTable = *reinterpret_cast<void***>(pSwapChain);
        oPresent = (t_Present)pVTable[8];
        oResizeBuffers = (t_ResizeBuffers)pVTable[13];

        HAX_LOG_DEBUG("[D3D10] Present address is {}", (void*)oPresent);
        HAX_LOG_DEBUG("[D3D10] ResizeBuffers address is {}", (void*)oResizeBuffers);

        pSwapChain->Release();
        pDevice->Release();
    }

    // Called every frame via Present hook.
    // Handles rendering of the ImGui menu.
    static void Render(IDXGISwapChain* pSwapChain) 
    {
        if (!ImGui::GetCurrentContext()) {
            ImGuiContextParams params;
            params.m_GraphicsApi = GraphicsApi_DirectX10;
            params.m_pSwapChain = pSwapChain;
            InitImGuiContext(params);
        }

        // Create render target once for ImGui to draw onto
        if (!g_pRenderTarget) {
            CreateRenderTarget(pSwapChain);
        }

        ImGui_ImplDX10_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Execute user-defined rendering logic
        try
        {
            if (!g_MenuDisabled && g_FnBackground)
                g_FnBackground();

            if (!g_MenuDisabled && g_MenuVisible && g_FnMenuRender)
                g_FnMenuRender();
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            HaxSdk::LogError(message ? message.UTF8() : "Exception without message");
        }

        ImGui::EndFrame();
        ImGui::Render();

        // Bind render target and draw ImGui output
        g_pDevice->OMSetRenderTargets(1, &g_pRenderTarget, nullptr);
        ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());
    }

    // Hooked ResizeBuffers handles screen size changes and destroys old render target
    static HRESULT WINAPI HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width,
        UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) 
    {
        if (g_pRenderTarget) {
            g_pRenderTarget->Release();
            g_pRenderTarget = nullptr;
        }
        return oResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
    }

    // Creates a render target view from the current swap chain's back buffer
    static void CreateRenderTarget(IDXGISwapChain* pSwapChain) 
    {
        ID3D10Texture2D* pBackBuffer = NULL;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer) {
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTarget);
            pBackBuffer->Release();
        }
    }
}

//-------------------------------------------------------------------------
// [SECTION] Graphics API implementation [DIRECTX11]
//-------------------------------------------------------------------------

namespace dx11
{
    // Initializes the hook for DirectX 11 by creating a dummy device and swap chain.
    // This allows retrieving the vtable of IDXGISwapChain to hook Present and ResizeBuffers.
    static void Setup()
    {
        if (oPresent && oResizeBuffers)
            return;

        HMODULE module = GetModuleHandle("d3d11.dll");

        // Create a dummy swap chain and device using a hidden window
        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = g_DummyHWND;
        swapChainDesc.SampleDesc.Count = 1;

        IDXGISwapChain* pSwapChain;
        ID3D11Device* pDevice;
        const D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_0
        };

        // Create device and swap chain (null driver, for introspection only)
        HRESULT result = D3D11CreateDeviceAndSwapChain(
            NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, featureLevels, 2,
            D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, nullptr, &g_pDeviceContext
        );

        if (result != S_OK)
        {
            HAX_LOG_ERROR("[D3D11] D3D11CreateDeviceAndSwapChain returned {}. Hook not installed", result);
            return;
        }

        // Retrieve Present and ResizeBuffers addresses from swap chain's vtable
        void** pVTable = *(void***)(pSwapChain);
        oPresent = (t_Present)pVTable[8];
        oResizeBuffers = (t_ResizeBuffers)pVTable[13];

        pSwapChain->Release();
        pDevice->Release();

        HAX_LOG_DEBUG("[D3D11] Present address is {}", (void*)oPresent);
        HAX_LOG_DEBUG("[D3D11] ResizeBuffers address is {}", (void*)oResizeBuffers);
    }

    // Creates the render target view from the swap chain's back buffer.
    // This is needed for ImGui to render into the frame.
    static void CreateRenderTarget(IDXGISwapChain* pSwapChain)
    {
        ID3D11Texture2D* pBackBuffer = NULL;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer && g_pDevice)
        {
            // Configure render target view (with brightness fix)
            D3D11_RENDER_TARGET_VIEW_DESC desc = {};
            memset(&desc, 0, sizeof(desc));
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            g_pDevice->CreateRenderTargetView(pBackBuffer, &desc, &g_pRenderTarget);
            pBackBuffer->Release();
        }
    }

    // Called each frame to render the ImGui overlay.
    // Hooked via the Present function of IDXGISwapChain.
    static void Render(IDXGISwapChain* pSwapChain)
    {
        // Initialize ImGui context for DirectX 11 if not already created
        if (!ImGui::GetCurrentContext())
        {
            ImGuiContextParams params;
            params.m_GraphicsApi = GraphicsApi_DirectX11;
            params.m_pSwapChain = pSwapChain;
            InitImGuiContext(params);
        }

        // Ensure render target is created
        if (!g_pRenderTarget)
            CreateRenderTarget(pSwapChain);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Execute user-defined rendering callbacks
        try
        {
            if (!g_MenuDisabled && g_FnBackground)
                g_FnBackground();

            if (!g_MenuDisabled && g_MenuVisible && g_FnMenuRender)
                g_FnMenuRender();
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.ToString();
            HaxSdk::LogError(std::format("", message.UTF8()));
        }

        ImGui::EndFrame();
        ImGui::Render();

        // Bind render target and render ImGui draw data
        g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTarget, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    // Called when the swap chain is resized (e.g., resolution change).
    // Frees the render target so it can be recreated.
    static HRESULT WINAPI HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width,
        UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
    {
        if (g_pRenderTarget)
        {
            g_pRenderTarget->Release();
            g_pRenderTarget = nullptr;
        }
        return oResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
    }

    // Loads an embedded PNG texture from module resources and uploads it to a DirectX 11 GPU texture.
    static HaxTexture LoadTextureFromResource(int32_t id)
    {
        LPVOID pointerToResource = nullptr;
        DWORD sizeOfResource;
        HMODULE hCheatModule = (HMODULE)g_CheatHandle;

        if (HRSRC hResInfo = FindResourceW(hCheatModule, MAKEINTRESOURCEW(id), L"PNG"))
        {
            if (HGLOBAL hResData = LoadResource(hCheatModule, hResInfo))
            {
                pointerToResource = LockResource(hResData);
                sizeOfResource = SizeofResource(hCheatModule, hResInfo);
            }
        }

        assert(pointerToResource && "Invalid image");

        HaxTexture result{};
        int width, height;

        // Decode PNG image from memory using stb_image
        auto image_data = stbi_load_from_memory((stbi_uc*)pointerToResource, sizeOfResource, &width, &height, NULL, 4);
        result.m_height = static_cast<float>(height);
        result.m_width = static_cast<float>(width);

        // Define and create a 2D texture
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = static_cast<UINT>(result.m_width);
        desc.Height = static_cast<UINT>(result.m_height);
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D* pTexture = NULL;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = image_data;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        dx11::g_pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

        // Create a shader resource view for the texture
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        dx11::g_pDevice->CreateShaderResourceView(pTexture, &srvDesc, (ID3D11ShaderResourceView**)&result.m_pTexture);
        pTexture->Release();
        stbi_image_free(image_data);

        return result;
    }
} // dx11

//-------------------------------------------------------------------------
// [SECTION] Graphics API implementation [DIRECTX12]
//-------------------------------------------------------------------------

namespace dx12
{
    // Initializes DirectX 12 device and swap chain to retrieve vtable functions for hooking.
    static void Setup()
    {
        // Describe dummy swap chain (used for vtable inspection only)
        DXGI_SWAP_CHAIN_DESC1 sd = { };
        sd.BufferCount = NUM_BACK_BUFFERS;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        // Create Direct3D 12 device
        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        if (D3D12CreateDevice(NULL, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK) { return; }

        // Create command queue
        D3D12_COMMAND_QUEUE_DESC desc = { };
        if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK) { return; }

        // Create swap chain for dummy window
        IDXGISwapChain1* swapChain1 = NULL;
        if (CreateDXGIFactory1(IID_PPV_ARGS(&g_dxgiFactory)) != S_OK) { return; }
        if (g_dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, g_DummyHWND, &sd, NULL, NULL, &swapChain1) != S_OK) { return; }
        if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK) { return; }

        // Create command allocators for all back buffers
        for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
        {
            if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocators[i])) != S_OK)
            {
                HAX_LOG_DEBUG("CreateCommandAllocator failed");
                return;
            }
        }

        // Create initial command list
        if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocators[0], NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK)
            return;

        swapChain1->Release();

        // Retrieve function pointers from swap chain and command queue vtables
        void** pVTable = *reinterpret_cast<void***>(g_pSwapChain);
        void** pCommandQueueVTable = *reinterpret_cast<void***>(g_pd3dCommandQueue);

        oPresent = (t_Present)pVTable[8];
        oResizeBuffers = (t_ResizeBuffers)pVTable[13];
        oExecuteCommandLists = (t_ExecuteCommandLists)pCommandQueueVTable[10];

        HAX_LOG_DEBUG("[D3D12] oPresent address is {}", (void*)oPresent);
        HAX_LOG_DEBUG("[D3D12] oResizeBuffers is {}", (void*)oResizeBuffers);
        HAX_LOG_DEBUG("[D3D12] oExecuteCommandLists is {}", (void*)oExecuteCommandLists);

        // Cleanup temporary resources used for setup
        if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = NULL; }
        for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
        {
            if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = NULL; }
        }
        if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
        for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
        {
            if (g_commandAllocators[i]) { g_commandAllocators[i]->Release(); g_commandAllocators[i] = NULL; }
        }
        if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = NULL; }
        if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = NULL; }
        if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = NULL; }
        if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
        if (g_dxgiFactory) { g_dxgiFactory->Release(); g_dxgiFactory = NULL; }
    }

    // Called every frame from the Present hook to render the ImGui menu.
    static void Render(IDXGISwapChain3* pSwapChain)
    {
        // Initialize ImGui context if not created
        if (!ImGui::GetCurrentContext())
        {
            ImGuiContextParams params;
            params.m_GraphicsApi = GraphicsApi_DirectX12;
            params.m_pSwapChain = pSwapChain;
            InitImGuiContext(params);
        }

        // Set up ImGui backend for DX12 if not already done
        if (!ImGui::GetIO().BackendRendererUserData)
        {
            if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&g_pd3dDevice))))
            {
                // Create RTV descriptor heap
                {
                    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
                    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                    desc.NumDescriptors = NUM_BACK_BUFFERS;
                    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                    desc.NodeMask = 1;
                    if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK) return;

                    SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
                    for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i) {
                        g_mainRenderTargetDescriptor[i] = rtvHandle;
                        rtvHandle.ptr += rtvDescriptorSize;
                    }
                }

                // Create SRV descriptor heap
                {
                    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
                    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                    desc.NumDescriptors = 1;
                    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                    if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK) return;
                }

                // Create command allocators for rendering
                for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
                    if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocators[i])) != S_OK)
                        return;

                // Create command list
                if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocators[0], NULL, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK || g_pd3dCommandList->Close() != S_OK)
                    return;

                // Initialize ImGui DX12 backend
                ImGui_ImplDX12_Init(g_pd3dDevice, NUM_BACK_BUFFERS,
                    DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
                    g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                    g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
            }
        }

        // Ensure render targets are created
        if (!g_mainRenderTargetResource[0])
            CreateRenderTarget(pSwapChain);

        if (ImGui::GetCurrentContext() && g_pd3dCommandQueue && g_mainRenderTargetResource[0])
        {
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // Execute user-defined render logic
            try
            {
                if (!g_MenuDisabled && g_FnBackground)
                    g_FnBackground();

                if (!g_MenuDisabled && g_MenuVisible && g_FnMenuRender)
                    g_FnMenuRender();
            }
            catch (System::Exception& ex)
            {
                System::String message = ex.GetMessage();
                HaxSdk::LogError(message ? message.UTF8() : "Exception without message");
            }

            ImGui::Render();

            // Record GPU commands to draw ImGui onto backbuffer
            UINT backBufferIdx = pSwapChain->GetCurrentBackBufferIndex();
            ID3D12CommandAllocator* commandAllocator = g_commandAllocators[backBufferIdx];
            commandAllocator->Reset();

            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

            g_pd3dCommandList->Reset(commandAllocator, NULL);
            g_pd3dCommandList->ResourceBarrier(1, &barrier);
            g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, NULL);
            g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);

            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);

            // Transition back to present state
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            g_pd3dCommandList->ResourceBarrier(1, &barrier);
            g_pd3dCommandList->Close();

            // Submit draw commands to the GPU
            g_pd3dCommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&g_pd3dCommandList));
        }
    }

    // Hooked resize handler: frees render targets to allow re-creation after size change
    static HRESULT WINAPI HookedResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height,
        DXGI_FORMAT newFormat, UINT swapChainFlags)
    {
        for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
        {
            if (g_mainRenderTargetResource[i])
            {
                g_mainRenderTargetResource[i]->Release();
                g_mainRenderTargetResource[i] = NULL;
            }
        }
        return oResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
    }

    // Hook used to capture the real command queue (needed for rendering)
    static void WINAPI HookedExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists)
    {
        if (!g_pd3dCommandQueue)
            g_pd3dCommandQueue = pCommandQueue;
        return oExecuteCommandLists(pCommandQueue, NumCommandLists, ppCommandLists);
    }

    // Creates render targets (RTVs) for all back buffers in the swap chain
    static void CreateRenderTarget(IDXGISwapChain* pSwapChain)
    {
        for (UINT i = 0; i < NUM_BACK_BUFFERS; ++i)
        {
            ID3D12Resource* pBackBuffer = NULL;
            pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
            if (pBackBuffer)
            {
                DXGI_SWAP_CHAIN_DESC sd;
                pSwapChain->GetDesc(&sd);

                g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, g_mainRenderTargetDescriptor[i]);
                g_mainRenderTargetResource[i] = pBackBuffer;
            }
        }
    }
} // dx12

//-------------------------------------------------------------------------
// [SECTION] Graphics API implementation [VULKAN]
//-------------------------------------------------------------------------

#ifdef _VULKAN
namespace vulkan
{
    static VkResult VKAPI_CALL HookedVkAcquireNextImageKHR( VkDevice device, 
                                                            VkSwapchainKHR swapchain,
                                                            uint64_t timeout,
                                                            VkSemaphore semaphore,
                                                            VkFence fence,
                                                            uint32_t* pImageIndex) 
    {
        g_Device = device;
        return oVkAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
    }

    static VkResult VKAPI_CALL HookedVkAcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex) {
        g_Device = device;
        return oVkAcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
    }

    static VkResult VKAPI_CALL HookedVkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) 
    {
        if (!ImGui::GetCurrentContext()) 
        {
            ImGuiContextParams params = { GraphicsApi_OpenGL, nullptr, nullptr, nullptr };
            InitImGuiContext(params);
            HAX_LOG("GAME USES VULKAN");
        }

        RenderImGui_Vulkan(queue, pPresentInfo);
        return oVkQueuePresentKHR(queue, pPresentInfo);
    }

    static VkResult VKAPI_CALL HookedVkCreateSwapchainKHR(VkDevice device,
                                                    const VkSwapchainCreateInfoKHR* pCreateInfo,
                                                    const VkAllocationCallbacks* pAllocator,
                                                    VkSwapchainKHR* pSwapchain) 
    {
        CleanupRenderTarget();
        g_ImageExtent = pCreateInfo->imageExtent;
        return oVkCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
    }

    static void Setup()
    {
        if (!CreateDevice()) 
        {
            HaxSdk::LogError("[!] CreateDeviceVK() failed.\n");
            return;
        }
            
        oVkAcquireNextImageKHR = (t_vkAcquireNextImageKHR)vkGetDeviceProcAddr(g_FakeDevice, "vkAcquireNextImageKHR");
        oVkAcquireNextImage2KHR = (t_vkAcquireNextImage2KHR)vkGetDeviceProcAddr(g_FakeDevice, "vkAcquireNextImage2KHR");
        oVkQueuePresentKHR = (t_vkQueuePresentKHR)vkGetDeviceProcAddr(g_FakeDevice, "vkQueuePresentKHR");
        oVkCreateSwapchainKHR = (t_vkCreateSwapchainKHR)vkGetDeviceProcAddr(g_FakeDevice, "vkCreateSwapchainKHR");

        if (g_FakeDevice) 
        {
            vkDestroyDevice(g_FakeDevice, g_Allocator);
            g_FakeDevice = NULL;
        }

        if (oVkAcquireNextImageKHR) 
        {
            printf("[+] Vulkan: fnAcquireNextImageKHR: 0x%p\n", oVkAcquireNextImageKHR);
            printf("[+] Vulkan: fnAcquireNextImage2KHR: 0x%p\n", oVkAcquireNextImage2KHR);
            printf("[+] Vulkan: fnQueuePresentKHR: 0x%p\n", oVkQueuePresentKHR);
            printf("[+] Vulkan: fnCreateSwapchainKHR: 0x%p\n", oVkCreateSwapchainKHR);

            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&oVkAcquireNextImageKHR, HookedVkAcquireNextImageKHR);
            DetourAttach(&oVkAcquireNextImage2KHR, HookedVkAcquireNextImage2KHR);
            DetourAttach(&oVkQueuePresentKHR, HookedVkQueuePresentKHR);
            DetourAttach(&oVkCreateSwapchainKHR, HookedVkCreateSwapchainKHR);
            DetourTransactionCommit();
        }
    }

    static void RenderImGui_Vulkan(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) 
    {
        if (!g_Device)
            return;

        VkQueue graphicQueue = VK_NULL_HANDLE;
        const bool queueSupportsGraphic = DoesQueueSupportGraphic(queue, &graphicQueue);


        for (uint32_t i = 0; i < pPresentInfo->swapchainCount; ++i) 
        {
            VkSwapchainKHR swapchain = pPresentInfo->pSwapchains[i];
            if (g_Frames[0].Framebuffer == VK_NULL_HANDLE) {
                CreateRenderTarget(g_Device, swapchain);
            }

            ImGui_ImplVulkanH_Frame* fd = &g_Frames[pPresentInfo->pImageIndices[i]];
            ImGui_ImplVulkanH_FrameSemaphores* fsd = &g_FrameSemaphores[pPresentInfo->pImageIndices[i]];
            {
                vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, ~0ull);
                vkResetFences(g_Device, 1, &fd->Fence);
            }
            {
                vkResetCommandBuffer(fd->CommandBuffer, 0);

                VkCommandBufferBeginInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                vkBeginCommandBuffer(fd->CommandBuffer, &info);
            }
            {
                VkRenderPassBeginInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                info.renderPass = g_RenderPass;
                info.framebuffer = fd->Framebuffer;
                if (g_ImageExtent.width == 0 || g_ImageExtent.height == 0) {
                    // We don't know the window size the first time. So we just set it to 4K.
                    info.renderArea.extent.width = 3840;
                    info.renderArea.extent.height = 2160;
                }
                else {
                    info.renderArea.extent = g_ImageExtent;
                }

                vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
            }

            if (!ImGui::GetIO().BackendRendererUserData) 
            {
                ImGui_ImplVulkan_InitInfo init_info = { };
                init_info.Instance = g_Instance;
                init_info.PhysicalDevice = g_PhysicalDevice;
                init_info.Device = g_Device;
                init_info.QueueFamily = g_QueueFamily;
                init_info.Queue = graphicQueue;
                init_info.PipelineCache = g_PipelineCache;
                init_info.DescriptorPool = g_DescriptorPool;
                init_info.Subpass = 0;
                init_info.MinImageCount = g_MinImageCount;
                init_info.ImageCount = g_MinImageCount;
                init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
                init_info.Allocator = g_Allocator;
                ImGui_ImplVulkan_Init(&init_info, g_RenderPass);

                ImGui_ImplVulkan_CreateFontsTexture(fd->CommandBuffer);
            }

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            try
            {
                if (!g_MenuDisabled && g_FnBackground)
                    g_FnBackground();

                if (!g_MenuDisabled && g_MenuVisible && g_FnMenuRender)
                    g_FnMenuRender();
            }
            catch (System::Exception& ex)
            {
                System::String message = ex.GetMessage();
                HaxSdk::LogError(message ? message.UTF8() : "Exception without message");
            }

            ImGui::Render();

            // Record dear imgui primitives into command buffer
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), fd->CommandBuffer);

            // Submit command buffer
            vkCmdEndRenderPass(fd->CommandBuffer);
            vkEndCommandBuffer(fd->CommandBuffer);

            uint32_t waitSemaphoresCount = i == 0 ? pPresentInfo->waitSemaphoreCount : 0;
            if (waitSemaphoresCount == 0 && !queueSupportsGraphic) 
            {
                constexpr VkPipelineStageFlags stages_wait = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                {
                    VkSubmitInfo info = { };
                    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

                    info.pWaitDstStageMask = &stages_wait;

                    info.signalSemaphoreCount = 1;
                    info.pSignalSemaphores = &fsd->RenderCompleteSemaphore;

                    vkQueueSubmit(queue, 1, &info, VK_NULL_HANDLE);
                }
                {
                    VkSubmitInfo info = { };
                    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                    info.commandBufferCount = 1;
                    info.pCommandBuffers = &fd->CommandBuffer;

                    info.pWaitDstStageMask = &stages_wait;
                    info.waitSemaphoreCount = 1;
                    info.pWaitSemaphores = &fsd->RenderCompleteSemaphore;

                    info.signalSemaphoreCount = 1;
                    info.pSignalSemaphores = &fsd->ImageAcquiredSemaphore;

                    vkQueueSubmit(graphicQueue, 1, &info, fd->Fence);
                }
            }
            else 
            {
                std::vector<VkPipelineStageFlags> stages_wait(waitSemaphoresCount, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

                VkSubmitInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                info.commandBufferCount = 1;
                info.pCommandBuffers = &fd->CommandBuffer;

                info.pWaitDstStageMask = stages_wait.data();
                info.waitSemaphoreCount = waitSemaphoresCount;
                info.pWaitSemaphores = pPresentInfo->pWaitSemaphores;

                info.signalSemaphoreCount = 1;
                info.pSignalSemaphores = &fsd->ImageAcquiredSemaphore;

                vkQueueSubmit(graphicQueue, 1, &info, fd->Fence);
            }
        }
    }

    static bool CreateDevice() 
    {
        // Create Vulkan Instance
        {
            VkInstanceCreateInfo create_info = { };
            constexpr const char* instance_extension = "VK_KHR_surface";

            create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            create_info.enabledExtensionCount = 1;
            create_info.ppEnabledExtensionNames = &instance_extension;

            // Create Vulkan Instance without any debug feature
            vkCreateInstance(&create_info, g_Allocator, &g_Instance);
            HAX_LOG_DEBUG("[+] Vulkan: g_Instance: 0x {}\n", (void*)g_Instance);
        }

        // Select GPU
        {
            uint32_t gpu_count;
            vkEnumeratePhysicalDevices(g_Instance, &gpu_count, NULL);
            IM_ASSERT(gpu_count > 0);

            VkPhysicalDevice* gpus = new VkPhysicalDevice[sizeof(VkPhysicalDevice) * gpu_count];
            vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus);

            // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
            // most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
            // dedicated GPUs) is out of scope of this sample.
            int use_gpu = 0;
            for (int i = 0; i < (int)gpu_count; ++i) {
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(gpus[i], &properties);
                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    use_gpu = i;
                    break;
                }
            }

            g_PhysicalDevice = gpus[use_gpu];
            HAX_LOG_DEBUG("[+] Vulkan: g_PhysicalDevice: {}\n", (void*)g_PhysicalDevice);

            delete[] gpus;
        }

        // Select graphics queue family
        {
            uint32_t count;
            vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, NULL);
            g_QueueFamilies.resize(count);
            vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, g_QueueFamilies.data());
            for (uint32_t i = 0; i < count; ++i) {
                if (g_QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    g_QueueFamily = i;
                    break;
                }
            }
            IM_ASSERT(g_QueueFamily != (uint32_t)-1);

            HAX_LOG_DEBUG("[+] Vulkan: g_QueueFamily: {}\n", g_QueueFamily);
        }

        // Create Logical Device (with 1 queue)
        {
            constexpr const char* device_extension = "VK_KHR_swapchain";
            constexpr const float queue_priority = 1.0f;

            VkDeviceQueueCreateInfo queue_info = { };
            queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_info.queueFamilyIndex = g_QueueFamily;
            queue_info.queueCount = 1;
            queue_info.pQueuePriorities = &queue_priority;

            VkDeviceCreateInfo create_info = { };
            create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            create_info.queueCreateInfoCount = 1;
            create_info.pQueueCreateInfos = &queue_info;
            create_info.enabledExtensionCount = 1;
            create_info.ppEnabledExtensionNames = &device_extension;

            vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_FakeDevice);

            HAX_LOG_DEBUG("[+] Vulkan: g_FakeDevice: {}\n", (void*)g_FakeDevice);
        }

        return true;
    }

    static bool DoesQueueSupportGraphic(VkQueue queue, VkQueue* pGraphicQueue) 
    {
        for (uint32_t i = 0; i < g_QueueFamilies.size(); ++i) 
        {
            const VkQueueFamilyProperties& family = g_QueueFamilies[i];
            for (uint32_t j = 0; j < family.queueCount; ++j) 
            {
                VkQueue it = VK_NULL_HANDLE;
                vkGetDeviceQueue(g_Device, i, j, &it);

                if (pGraphicQueue && family.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
                    if (*pGraphicQueue == VK_NULL_HANDLE)
                        *pGraphicQueue = it;

                if (queue == it && family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    return true;
            }
        }

        return false;
    }

    static void CreateRenderTarget(VkDevice device, VkSwapchainKHR swapchain) 
    {
        uint32_t uImageCount;
        vkGetSwapchainImagesKHR(device, swapchain, &uImageCount, NULL);

        VkImage backbuffers[8] = { };
        vkGetSwapchainImagesKHR(device, swapchain, &uImageCount, backbuffers);

        for (uint32_t i = 0; i < uImageCount; ++i) 
        {
            g_Frames[i].Backbuffer = backbuffers[i];

            ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
            ImGui_ImplVulkanH_FrameSemaphores* fsd = &g_FrameSemaphores[i];
            {
                VkCommandPoolCreateInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
                info.queueFamilyIndex = g_QueueFamily;

                vkCreateCommandPool(device, &info, g_Allocator, &fd->CommandPool);
            }
            {
                VkCommandBufferAllocateInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                info.commandPool = fd->CommandPool;
                info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                info.commandBufferCount = 1;

                vkAllocateCommandBuffers(device, &info, &fd->CommandBuffer);
            }
            {
                VkFenceCreateInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
                vkCreateFence(device, &info, g_Allocator, &fd->Fence);
            }
            {
                VkSemaphoreCreateInfo info = { };
                info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                vkCreateSemaphore(device, &info, g_Allocator, &fsd->ImageAcquiredSemaphore);
                vkCreateSemaphore(device, &info, g_Allocator, &fsd->RenderCompleteSemaphore);
            }
        }

        // Create the Render Pass
        {
            VkAttachmentDescription attachment = { };
            attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
            attachment.samples = VK_SAMPLE_COUNT_1_BIT;
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference color_attachment = { };
            color_attachment.attachment = 0;
            color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass = { };
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &color_attachment;

            VkRenderPassCreateInfo info = { };
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            info.attachmentCount = 1;
            info.pAttachments = &attachment;
            info.subpassCount = 1;
            info.pSubpasses = &subpass;

            vkCreateRenderPass(device, &info, g_Allocator, &g_RenderPass);
        }

        // Create The Image Views
        {
            VkImageViewCreateInfo info = { };
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = VK_FORMAT_B8G8R8A8_UNORM;

            info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            info.subresourceRange.baseMipLevel = 0;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.baseArrayLayer = 0;
            info.subresourceRange.layerCount = 1;

            for (uint32_t i = 0; i < uImageCount; ++i) 
            {
                ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
                info.image = fd->Backbuffer;

                vkCreateImageView(device, &info, g_Allocator, &fd->BackbufferView);
            }
        }

        // Create Framebuffer
        {
            VkImageView attachment[1];
            VkFramebufferCreateInfo info = { };
            info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            info.renderPass = g_RenderPass;
            info.attachmentCount = 1;
            info.pAttachments = attachment;
            info.layers = 1;

            for (uint32_t i = 0; i < uImageCount; ++i) {
                ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
                attachment[0] = fd->BackbufferView;

                vkCreateFramebuffer(device, &info, g_Allocator, &fd->Framebuffer);
            }
        }

        if (!g_DescriptorPool) // Create Descriptor Pool.
        {
            constexpr VkDescriptorPoolSize pool_sizes[] =
            {
                {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };

            VkDescriptorPoolCreateInfo pool_info = { };
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
            pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
            pool_info.pPoolSizes = pool_sizes;

            vkCreateDescriptorPool(device, &pool_info, g_Allocator, &g_DescriptorPool);
        }
    }

    static void CleanupRenderTarget() 
    {
        for (uint32_t i = 0; i < RTL_NUMBER_OF(g_Frames); ++i) {
            if (g_Frames[i].Fence) 
            {
                vkDestroyFence(g_Device, g_Frames[i].Fence, g_Allocator);
                g_Frames[i].Fence = VK_NULL_HANDLE;
            }
            if (g_Frames[i].CommandBuffer) 
            {
                vkFreeCommandBuffers(g_Device, g_Frames[i].CommandPool, 1, &g_Frames[i].CommandBuffer);
                g_Frames[i].CommandBuffer = VK_NULL_HANDLE;
            }
            if (g_Frames[i].CommandPool) 
            {
                vkDestroyCommandPool(g_Device, g_Frames[i].CommandPool, g_Allocator);
                g_Frames[i].CommandPool = VK_NULL_HANDLE;
            }
            if (g_Frames[i].BackbufferView) 
            {
                vkDestroyImageView(g_Device, g_Frames[i].BackbufferView, g_Allocator);
                g_Frames[i].BackbufferView = VK_NULL_HANDLE;
            }
            if (g_Frames[i].Framebuffer) 
            {
                vkDestroyFramebuffer(g_Device, g_Frames[i].Framebuffer, g_Allocator);
                g_Frames[i].Framebuffer = VK_NULL_HANDLE;
            }
        }

        for (uint32_t i = 0; i < RTL_NUMBER_OF(g_FrameSemaphores); ++i) 
        {
            if (g_FrameSemaphores[i].ImageAcquiredSemaphore) 
            {
                vkDestroySemaphore(g_Device, g_FrameSemaphores[i].ImageAcquiredSemaphore, g_Allocator);
                g_FrameSemaphores[i].ImageAcquiredSemaphore = VK_NULL_HANDLE;
            }
            if (g_FrameSemaphores[i].RenderCompleteSemaphore) 
            {
                vkDestroySemaphore(g_Device, g_FrameSemaphores[i].RenderCompleteSemaphore, g_Allocator);
                g_FrameSemaphores[i].RenderCompleteSemaphore = VK_NULL_HANDLE;
            }
        }
    }
} // vulkan
#endif