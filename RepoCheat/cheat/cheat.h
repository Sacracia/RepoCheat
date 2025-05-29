#pragma once

// Typically, cheat should provide three functions for calling HaxSdk::ImplementImGui in dllmain.
// - "Menu" where all cheat menus (ImGui windows) are being rendered
// - "Background" where esp, radar and other stuff should be always rendered behind the menus
// - "Init" where all cheat data should be initialized before the menu started rendering

namespace Cheat
{
    void Menu();
    void Background();
    void Init();
}