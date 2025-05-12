#pragma once

#include <cstdint>

typedef int GraphicsApi;

enum GraphicsApi_ {
    GraphicsApi_None      = 0,
    GraphicsApi_OpenGL    = 1 << 0,
    GraphicsApi_DirectX9  = 1 << 1,
    GraphicsApi_DirectX10 = 1 << 2,
    GraphicsApi_DirectX11 = 1 << 3,
    GraphicsApi_DirectX12 = 1 << 4,
    GraphicsApi_Vulkan    = 1 << 5,
    GraphicsApi_Any       = (1 << 6) - 1
};

struct HaxTexture {
    void*           m_pTexture;
    float           m_width;
    float           m_height;
};

namespace HaxSdk {
    void            DoOnceBeforeRendering();
    void            Shutdown();
    void            RenderMenu();
    void            RenderBackground();
    HaxTexture      LoadTextureFromResource(int32_t id);
    void            ImplementImGui(GraphicsApi);
}