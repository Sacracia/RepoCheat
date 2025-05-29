#pragma once

#include <cstdint>
#include <functional>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "third_party/imgui/imgui.h"
#include "third_party/imgui/imgui_internal.h"

using HANDLE = void*;

typedef int GraphicsApi;

#undef DrawText

enum GraphicsApi_ 
{
    GraphicsApi_None      = 0,
    GraphicsApi_OpenGL    = 1 << 0,
    GraphicsApi_DirectX9  = 1 << 1,
    GraphicsApi_DirectX10 = 1 << 2,
    GraphicsApi_DirectX11 = 1 << 3,
    GraphicsApi_DirectX12 = 1 << 4,
    GraphicsApi_Vulkan    = 1 << 5,
    GraphicsApi_Any       = (1 << 6) - 1
};

enum VerticalAlignment : int 
{
    VerticalAlignment_Bottom,
    VerticalAlignment_Top,
    VerticalAlignment_Center
};

enum HorizontalAlignment : int
{
    HorizontalAlignment_Right,
    HorizontalAlignment_Left,
    HorizontalAlignment_Center
};

struct HaxTexture 
{
    void*           m_pTexture;
    float           m_width;
    float           m_height;
};

// RAII implementation of ImGui window.
// Purpose: support exception catching, since ImGui::Begin should end with ImGui::End
struct HaxWindow
{
    HaxWindow(const char* name, bool* open = nullptr, ImGuiWindowFlags flags = 0)
    {
        m_Flag = ImGui::Begin(name, open, flags);
    }

    ~HaxWindow()
    {
        if (m_Flag)
            ImGui::End();
    }

    explicit operator bool() const { return m_Flag; }

private:
    bool m_Flag;
};

// RAII implementation of ImGui table.
// Purpose: support exception catching, since ImGui::BeginTable should end with ImGui::EndTable
struct HaxTable
{
    HaxTable(const char* str_id, int columns, ImGuiTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0.0f, 0.0f), float inner_width = 0.0f)
    {
        m_Flag = ImGui::BeginTable(str_id, columns, flags, outer_size, inner_width);
    }

    ~HaxTable()
    {
        if (m_Flag)
            ImGui::EndTable();
    }

    inline operator bool() const { return m_Flag; }

private:
    bool m_Flag;
};

// RAII implementation of ImGui table.
// Purpose: support exception catching, since ImGui::BeginCombo should end with ImGui::EndCombo
struct HaxCombo
{
    HaxCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0)
    {
        m_Flag = ImGui::BeginCombo(label, preview_value, flags);
    }

    ~HaxCombo()
    {
        if (m_Flag)
            ImGui::EndCombo();
    }

    inline operator bool() const { return m_Flag; }

private:
    bool m_Flag;
};

// RAII implementation of ImGui disabled.
// Purpose: support exception catching, since ImGui::BeginDisabled should end with ImGui::EndDisabled
struct HaxDisabled
{
    HaxDisabled(bool disabled = true)
    {
        ImGui::BeginDisabled(disabled);
    }

    ~HaxDisabled()
    {
        ImGui::EndDisabled();
    }
};

// RAII implementation of ImGui tooltip.
// Purpose: support exception catching, since ImGui::BeginTooltip should end with ImGui::EndTooltip
struct HaxTooltip
{
    HaxTooltip()
    {
        m_Flag = ImGui::BeginTooltip();
    }

    ~HaxTooltip()
    {
        if (m_Flag)
            ImGui::EndTooltip();
    }

    inline operator bool() const { return m_Flag; }

private:
    bool m_Flag;
};

namespace HaxSdk 
{
    /**
     * @brief Converts image from resources into a texture of active graphics api
     * @param id - ID of the resource specified in resource.h
     * @return Converted texture pointer, width, height
     * @warning Only DirectX 11 is currently supported 
     */
    HaxTexture      LoadTextureFromResource(int32_t id);

     /**
      * @brief Entry point of gui module. Implements imgui menu into the game.
      * @param graphics - Graphics api which are used by the game.
      * @param fnInit - Function which is called once before imgui initialization. Here you can load fonts, textures, config and do other preparations.
      * @param fnBackground - Background worker. Perfect place for rendering out-of-menu stuff like ESP, radar. Also a good place for running scheduler.
      * @param fnRender - Menu rendering function. Menu visibility check is done automatically.
      * @note You can pass nullptr if you dont need any.
      */
    void            ImplementImGui(GraphicsApi graphics, std::function<void()> fnInit, std::function<void()> fnBackground, std::function<void()> fnRender);

    /**
     * @brief Setter of cheat handle. Use in dllmain when cheat is loaded.
     * @param handle - DLL handle.
     */
    void            SetCheatHandle(HANDLE handle);

    // Getter of close / open menu hotkey. 
    // Returns a virtual key. See more: https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

    /**
     * @brief Getter of open / close menu hotkey.
     * @return Virtual key of the hotkey
     * @see https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
     */
    int             GetMenuHotkey();

    /**
     * @brief Setter of open / close menu hotkey.
     * @param hotkey - Virtual key of the hotkey
     * @see https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
     */
    void            SetMenuHotkey(int hotkey);

    /**
     * @brief Getter of UI scale factor.
     * @return Scale factor, calculated as "user_screen_height / 1080"
     */
    float           GetScaleFactor();

    namespace Widgets
    {
        /**
         * @brief Hover text help
         * @param tip - Text of the help
         */
        void            Tooltip(const char* tip);

        /**
         * @brief Toooltip that looks like "(?)"
         * @param tip - Tooltip text
         */
        void            HelpMarker(const char* tip);

        /**
         * @brief Toooltip that looks like "(!)"
         * @param tip - Tooltip text
         */
        void            WarningMarker(const char* tip);

        /**
         * @brief Button that can be disabled
         * @param label - Text of the button
         * @param enabled - Condition when button is enabled
         * @return Whether button is clicked. Always false when disabled
         */
        bool            DisabledButton(const char* label, bool enabled);

        /**
         * @brief Checkbox that can be disabled
         * @param label - Text of the checkbox
         * @pararm value - Pointer to checkbox value
         * @param enabled - Condition when checkbox is enabled
         * @return Whether checkbox is toggled. Always false when disabled
         */
        bool            DisabledCheckbox(const char* label, bool* value, bool enabled);

        /**
         * @brief Button that is available only for premium users
         * @param label - Text of the button
         * @param enabled - Condition when button is enabled
         * @param watermark - Watermark image shown near the button
         * @param wmText - Text shown when watermark is hovered
         * @return Whether button is clicked. Always false when disabled
         */
        bool            PremiumButton(const char* label, bool enabled, HaxTexture& watermark, const char* wmText);

        /**
         * @brief Checkbox that is available only for premium users
         * @param label - Text of the button
         * @param value - Pointer to checkbox value
         * @param enabled - Condition when checkbox is enabled
         * @param watermark - Watermark image shown near the checkbox
         * @param wmText - Text shown when watermark is hovered
         * @return Whether checkbox is toggled. Always false when disabled
         */
        bool            PremiumCheckbox(const char* label, bool* value, bool enabled, HaxTexture& watermark, const char* wmText);
    }

    namespace ESP
    {
        /**
         * @brief Draws box (rectangle) on screen with only corners visible
         * @param x - X-coordinate of the upper left corner of the box
         * @param y - Y-coordinate of the upper left corner of the box
         * @param width - Width of the box
         * @param height - Height of the box
         * @param color - Color of the corners
         * @param cornerProp - Ыize of corners in relation to the box size . Corner width, height are calculated as "width * cornerProp", "height * cornerProp".
         */
        void        CornerBox(float x, float y, float width, float height, ImU32 color, float cornerProp);

        /**
         * @brief Draws box on screen
         * @param x - X-coordinate of the upper left corner of the box
         * @param y - Y-coordinate of the upper left corner of the box
         * @param width - Width of the box
         * @param height - Height of the box
         * @param color - Color of the corners
         * @param rounding - Rounding value of corners.
         * @param thickness - Thickness of the box.
         * @note Box has a black outline. You cant disable it yet.
         */
        void        Box(float x, float y, float width, float height, ImU32 color, float rounding, float thickness);

        /**
         * @brief Draws text on screen
         * @param font - ImGui font of the text
         * @param text - Content of the text
         * @param pos - Position of the text
         * @param color - Color of the text
         * @param fontSize - Size of the text
         * @param vAlign - Vertical positioning of text relative to "pos".
         * @param hAlign - Horizontal positioning of text relative to "pos".
         */
        void        Text(ImFont* font, 
                         const char* text, 
                         const ImVec2& pos, 
                         ImU32 color,
                         float fontSize, 
                         VerticalAlignment vAlign = VerticalAlignment_Bottom, 
                         HorizontalAlignment hAlign = HorizontalAlignment_Right);

        /**
         * @brief Vertical health bar
         * @param x - X coordinate of the upper-left corner
         * @param y - Y coordinate of the upper-left corner
         * @param width - Width of the bar
         * @param height - Height of the bar
         * @param cur - Current health
         * @param max - Maximum health
         */
        void VertHealthBar(float x, float y, float width, float height, float cur, float max);

        /**
         * @brief Vertical health bar
         * @param x - X coordinate of the upper-left corner
         * @param y - Y coordinate of the upper-left corner
         * @param width - Width of the bar
         * @param height - Height of the bar
         * @param cur - Current health
         * @param max - Maximum health
         */
        void VertHealthBar(float x, float y, float width, float height, int cur, int max);
    }
}