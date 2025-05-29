/*

[Index of this file]
|
|- [SECTION] Headers
|- [SECTION] Supportive Structures
|- [SECTION] Global Variables
|- [SECTION] Forward Declarations
|- [SECTION] Cheat API
|- [SECTION] Tabs
|- [SECTION] ESP
|- [SECTION] Hooks
|- [SECTION] Supportive Functions

*/

//-----------------------------------------------------------------------------
// [SECTION] Headers
//-----------------------------------------------------------------------------

// File Header
#include "cheat.h"

// C Headers
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>

// STD Headers
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <map>

// HaxSdk Modules
#include "haxsdk_gui.h"    // GUI for rendering menu
#include "haxsdk_logger.h" // Logger for logging

// Local Includes
#include "game_classes.h"  // Game Classes
#include "locales.h"       // Translations

// Fonts
#include "fonts/Hack_Compressed.h"  // Font for menu
#include "fonts/Rubik_Compressed.h" // Font for ESP
#include "fonts/Icons.h"

// Images
#include "../resource.h"

namespace Cheat
{
    //-----------------------------------------------------------------------------
    // [SECTION] Supportive Structures
    //-----------------------------------------------------------------------------

    enum SpeedType : int
    {
        SpeedType_Crouch,   // Same speed as crouching
        SpeedType_Walk,     // Same speed as walking
        SpeedType_Sprint    // Same speed as sprinting
    };

    // Enemy ESP settings
    struct EspSettings
    {
        bool                    m_On;          // Enabled
        bool                    m_ShowBorder;  // Show border around enemy
        bool                    m_ShowName;    // Show name of an enemy
        bool                    m_ShowHp;      // Show enemy health bar
    };

    struct ValuablesEspSettings
    {
        bool                    m_On;          // Enabled
        bool                    m_ShowPrice;   // Show price of a valuable
        bool                    m_LimitDist;   // Bound esp distance
    };

    struct LevelBan
    {
        static inline size_t    s_TotalBans = 0; // Number of levels user banned. Must be less than total number of levels
        Level                   m_Level;         // Level to be banned
        std::string             m_Name;          // UTF8 name of the level
        bool                    m_Allowed = true;// Banned or not
    };

    // Switch dead / alive voice chat params
    struct SwitchChatParams
    {
                                SwitchChatParams() = default;
                                SwitchChatParams(bool sw, PlayerVoiceChat chat, bool val) 
                                    : m_ShouldSwitch(sw), m_Chat(chat), m_NewVal(val) {}

        bool                    m_ShouldSwitch; // User just toggled voice chat
        PlayerVoiceChat         m_Chat;         // New chat
        bool                    m_NewVal;       // New value to be set
    };

    //-----------------------------------------------------------------------------
    // [SECTION] Global Variables
    //-----------------------------------------------------------------------------
    
    static Matrix4x4                                            g_ViewMatrix;       // Unity WorldToScreen causes flickering when used outside Unity's rendering loop.
                                                                                    // We set this matrix during Unity's rendering and reuse it later for stability.
    static Matrix4x4                                            g_ProjMatrix;       // Same rationale as g_ViewMatrix - prevents rendering artifacts
    static ImFont*                                              g_Font;             // Primary font used for menu rendering
    static ImFont*                                              g_EspFont;          // Font specifically used for ESP text elements
    static ImFont*                                              g_IconsFont;        // Font containing game interface icons and symbols
    std::unordered_map<PhysGrabObject, std::vector<uint32_t>>   g_CachedColliders;  // Cache of collider handles to avoid per-frame lookups.
                                                                                    // Uses managed heap handles to track destroyed colliders.
    std::unordered_map<EnemyParent, std::vector<uint32_t>>      g_CachedMeshes;     // Mesh handle cache to optimize performance.
                                                                                    // Tracks mesh destruction via managed heap handles.
    static int                                                  g_Lang = 0;         // UI language setting (0=English, 1=Russian)

    static bool                                                 g_Godmode;          // Toggles invulnerability cheat state
    static bool                                                 g_HealToMax;        // When enabled, automatically restores player health to maximum
    static bool                                                 g_ShouldHealToMax;  // Flag indicating health should be restored next frame

    static bool                                                 g_InfStamina;       // Infinite stamina cheat toggle
    static bool                                                 g_InfJumps;         // Enables unlimited mid-air jumps
    static bool                                                 g_NeverTumble;      // Prevents player knockdown/stagger effects
    static float                                                g_SpeedMult;        // Global multiplier for player movement speed
    static int                                                  g_CrouchSpeedType = SpeedType_Crouch; // Crouch movement speed modifier
    static int                                                  g_WalkSpeedType = SpeedType_Walk;     // Walk movement speed modifier

    static EspSettings                                          g_EspSettings;      // Configuration container for enemy ESP visuals
    static bool                                                 g_OneHitKills;      // Enables instant kill mode for all enemies
    static int                                                  g_DamageMult = 1;   // Damage amplification multiplier against enemies
    static std::map<std::string, EnemySetup>                    g_EnemiesPool;      // Preloaded database of enemy configurations
    static EnemySetup                                           g_EnemyToSpawn;     // Enemy queued for spawning in next frame
    static bool                                                 g_ShouldKillEnemies;// Flag for executing enemy wipe in next frame
    static std::unordered_map<EnemyParent, EnemySlowMouth>      g_CachedSpewers;    // Cache of EnemySlowMouth components for performance

    static std::unordered_map<EnemyParent, uint32_t>            g_CachedEnemiesOnMap; // Tracks MapCustom components via handles to prevent null references
                                                                                      // when EnemyParent outlives MapCustom instance
    static bool                                                 g_ShowEnemiesOnMap; // Toggles enemy markers on in-game map
    static bool                                                 g_ShouldRevealRooms;// Flag for revealing all map areas next frame

    static bool                                                 g_ExtractionEsp;    // Toggles extraction point ESP visualization
    static float                                                g_ExtractionColor[4] = { 0.357f, 0.427f, 0.969f, 1.f }; // RGBA color for extraction markers
    static bool                                                 g_ShouldActivateNextPoint; // Queues next extraction point activation
    static bool                                                 g_ShouldFillQuota;   // Flag for automatically completing extraction requirements
    static bool                                                 g_ShouldUnlockAllPoints; // Enables all extraction points simultaneously
    static bool                                                 g_PretendMaster;     // Simulates host privileges in multiplayer

    static bool                                                 g_NoImpact;         // Disables weapon impact effects
    static ValuablesEspSettings                                 g_ValuablesEsp;     // Configuration for loot highlighting system
    static bool                                                 g_ShouldRevealValuables; // Flag for revealing all valuables next frame

    static bool                                                 g_InfiniteGrab;     // Enables unlimited object grabbing
    static bool                                                 g_HighGrabRange;    // Extends physics interaction distance
    static bool                                                 g_RaycastMaxDist;   // Forces maximum raycast distance

    static std::map<std::string, uint32_t>                      g_ItemsPool;        // Database of available in-game items
    static uint32_t                                             g_ItemToSpawn;      // Item queued for spawning
    static bool                                                 g_InfBattery;       // Infinite equipment power cheat
    static bool                                                 g_NoSpread;         // Removes weapon spread patterns
    static bool                                                 g_UseGunLaser;      // Toggles laser sights on all weapons
    static std::unordered_map<ItemGun, uint32_t>                g_CachedGunLasers;  // Cache of weapon laser components

    static bool                                                 g_ShouldChangeLevel;// Flag for level transition queuing
    static std::vector<LevelBan>                                g_LevelBans;        // List of restricted levels

    static SwitchChatParams                                     g_SwitchParams;     // Chat command processing parameters
    static PlayerAvatar                                         g_PlayerToKill;     // Player marked for termination
    static PlayerAvatar                                         g_PlayerToRevive;   // Player queued for revival
    static PlayerAvatar                                         g_PlayerToOvercharge;// Player marked for overcharge
    static bool                                                 g_PlayersEsp;       // Toggles player ESP visualization
    static bool                                                 g_ShouldBecomeGhost;// Flag for ghost mode activation

    static bool                                                 g_ShouldSetInfMoney;// Queues infinite money cheat activation
    static bool                                                 g_NoSaveDelete;     // Prevents save file modifications
    static bool                                                 g_SpawnEsp;         // Toggles spawn point visualization
    static float                                                g_SpawnColor[4] = { 0.357f, 0.427f, 0.969f, 1.f }; // RGBA color for spawn markers

    static HaxTexture                                           g_BoostyLink;       // Boosty promotion asset
    static HaxTexture                                           g_DiscordLink;      // Discord community link asset
    static HaxTexture                                           g_PatreonLink;      // Patreon sponsorship asset
    static HaxTexture                                           g_PatreonWM;        // Patreon watermark graphic
    static HaxTexture                                           g_BoostyWM;         // Boosty watermark graphic

    //-----------------------------------------------------------------------------
    // [SECTION] Forward Declarations
    //-----------------------------------------------------------------------------

                                                                /* Menu Tabs */
    static void                                                 SettingsTab();
    static void                                                 HealthTab(bool inGame);
    static void                                                 MovementTab();
    static void                                                 EnemiesTab(bool inGame);
    static void                                                 MapTab(bool inGame);
    static void                                                 PointsTab(bool inGame);
    static void                                                 ValuablesTab();
    static void                                                 GrabTab();
    static void                                                 ItemsTab(bool inGame);
    static void                                                 LevelTab(bool inGame);
    static void                                                 UpgradesTab(bool inGame);
    static void                                                 PlayersTab(bool inGame);
    static void                                                 MiscTab(bool inGame);
    static void                                                 LinksTab();

                                                                /* ESP Functions */
    static void                                                 DrawEnemiesEsp();
    static void                                                 DrawPointsEsp();
    static void                                                 DrawValuablesEsp();
    static void                                                 DrawPlayersEsp();
    static void                                                 DrawSpawnEsp();

                                                                /* Hooks */
    static void                                                 Hooked__Camera_FireOnPreRender(Camera cam);
    static void                                                 Hooked__EventSystem_Update(EventSystem __this);
    static void                                                 Hooked__PlayerHealth_Hurt(PlayerHealth __this, int dmg, bool savingGrace, int enemyIndex);
    static void                                                 Hooked__PlayerController_FixedUpdate(PlayerController __this);
    static void                                                 Hooked__PlayerController_Update(PlayerController __this);
    static void                                                 Hooked__PlayerTumble_TumbleRequest(PlayerTumble __this, bool isTumbling, bool playerInput);
    static void                                                 Hooked__PlayerAvatar_OnDestroy(PlayerAvatar __this);
    static void                                                 Hooked__EnemyHealth_Hurt(EnemyHealth __this, int damage, void* hurtDirection);
    static bool                                                 Hooked__PhotonNetwork_IsMasterClient();
    static void                                                 Hooked__PhysGrabObjectImpactDetector_FixedUpdate(PhysGrabObjectImpactDetector __this);
    static void                                                 Hooked__EnemyRigidbody_FixedUpdate(EnemyRigidbody __this);
    static void                                                 Hooked__PhysGrabber_PhysGrabLogic(PhysGrabber __this);
    static void                                                 Hooked__PhysGrabber_RayCheck(PhysGrabber __this, bool grab);
    static bool                                                 Hooked__Physics_Raycast(void* v1, void* v2, void* v3, float maxDistance, int v5, int v6);
    static void                                                 Hooked__ItemBattery_Update(ItemBattery __this);
    static void                                                 Hooked__ItemGun_Shoot(ItemGun __this);
    static void                                                 Hooked__ItemGun_Update(ItemGun __this);
    static void                                                 Hooked__RunManager_SetRunLevel(RunManager __this);
    static void                                                 Hooked__PhotonNetwork_ExecuteRpc(void* rpcData, Photon::Player sender);
    static void                                                 Hooked__PhotonMessageInfo_ctor(void* __this, Photon::Player player, int timestamp, void* view);
    static void                                                 Hooked_DataDirector__SaveDeleteCheck(DataDirector __this, bool leaveGame);

                                                                /* Auxiliary Functions */
    static void                                                 SaveConfig();   // Save cheat settings into INI file
    static void                                                 LoadConfig();   // Load cheat settings from INI file
    static bool                                                 IsInGame();     // Is player in game or in menu / loading
    static Rect                                                 CalcBoundsInScreenSpace(Bounds bigBounds, Camera cam); // Interpreted function from game code.
    static void                                                 ParseEnemies(List<EnemySetup> setups); // Get enemies database
    static void                                                 ParseItems();   // Get items database
    static void                                                 SpawnItem();    // Spawn chosen item
    static void                                                 ParseLevels();  // Get levels database

    //-----------------------------------------------------------------------------
    // [SECTION] Cheat API
    //-----------------------------------------------------------------------------

    void Init()
    {
        HAX_LOG("{}", "Cheat initialization...");
        LoadConfig();

        // Rate of saving settings
        auto& io = ImGui::GetIO();
        io.IniSavingRate = 60.f; // Once per 60 seconds

        ImFontConfig cfg;
        cfg.FontDataOwnedByAtlas = false; // Out fonts are from memory
        g_Font = io.Fonts->AddFontFromMemoryCompressedBase85TTF(g_HackCompressedBase85, std::roundf(HaxSdk::GetScaleFactor() * 13.f), &cfg, io.Fonts->GetGlyphRangesCyrillic());
        g_EspFont = io.Fonts->AddFontFromMemoryCompressedBase85TTF(g_RubikCompressedBase85, std::roundf(HaxSdk::GetScaleFactor() * 28.f), &cfg, io.Fonts->GetGlyphRangesCyrillic());
        static const ImWchar icon_ranges[] = { 0xE900, 0xE9B1, 0 }; // Range of my icons
        g_IconsFont = io.Fonts->AddFontFromMemoryCompressedTTF(icomoon_compressed_data, icomoon_compressed_size, std::roundf(HaxSdk::GetScaleFactor() * 32.f), &cfg, icon_ranges);

        // Install all hooks
        bool success = true;
        success &= PlayerHealth::s_Hurt.Hook(Hooked__PlayerHealth_Hurt);
        success &= PlayerController::s_Update.Hook(Hooked__PlayerController_Update);
        success &= PlayerController::s_FixedUpdate.Hook(Hooked__PlayerController_FixedUpdate);
        success &= PlayerTumble::s_TumbleRequest.Hook(Hooked__PlayerTumble_TumbleRequest);
        success &= PlayerAvatar::s_OnDestroy.Hook(Hooked__PlayerAvatar_OnDestroy);
        success &= EnemyHealth::s_Hurt.Hook(Hooked__EnemyHealth_Hurt);
        success &= PhysGrabObjectImpactDetector::s_FixedUpdate.Hook(Hooked__PhysGrabObjectImpactDetector_FixedUpdate);
        success &= EnemyRigidbody::s_FixedUpdate.Hook(Hooked__EnemyRigidbody_FixedUpdate);
        success &= PhysGrabber::s_PhysGrabLogic.Hook(Hooked__PhysGrabber_PhysGrabLogic);
        success &= PhysGrabber::s_RayCheck.Hook(Hooked__PhysGrabber_RayCheck);
        success &= ItemBattery::s_Update.Hook(Hooked__ItemBattery_Update);
        success &= ItemGun::s_Shoot.Hook(Hooked__ItemGun_Shoot);
        success &= ItemGun::s_Update.Hook(Hooked__ItemGun_Update);
        success &= RunManager::s_SetRunLevel.Hook(Hooked__RunManager_SetRunLevel);
        success &= DataDirector::s_SaveDeleteCheck.Hook(Hooked_DataDirector__SaveDeleteCheck);
        success &= Camera_FireOnPreRender.Hook(Hooked__Camera_FireOnPreRender);
        success &= EventSystem_Update.Hook(Hooked__EventSystem_Update);
        success &= PhotonNetwork__IsMasterClient.Hook(Hooked__PhotonNetwork_IsMasterClient);
        success &= Physics__Raycast.Hook(Hooked__Physics_Raycast);
        success &= PhotonNetwork__ExecuteRpc.Hook(Hooked__PhotonNetwork_ExecuteRpc);
        success &= PhotonMessageInfo__ctor.Hook(Hooked__PhotonMessageInfo_ctor);

        // Inform user that not all hooks were successfully installed
        if (!success)
            MessageBoxA(NULL, "Cheat wasn't properly loaded.\nPlease, restart the game.", "Caution", MB_OK | MB_ICONWARNING);

        // Load images into memory
        g_BoostyLink = HaxSdk::LoadTextureFromResource(IDB_PNG3);
        g_PatreonLink = HaxSdk::LoadTextureFromResource(IDB_PNG1);
        g_DiscordLink = HaxSdk::LoadTextureFromResource(IDB_PNG2);
        g_BoostyWM = HaxSdk::LoadTextureFromResource(IDB_PNG4);
        g_PatreonWM = HaxSdk::LoadTextureFromResource(IDB_PNG5);
    }

    void Background()
    {
        // Save settings into INI file
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantSaveIniSettings)
        {
            SaveConfig();
            ImGui::MarkIniSettingsDirty();
            io.WantSaveIniSettings = false;
        }

        // Dont draw ESP when user not playing
        if (!IsInGame())
            return;

        // Dont draw ESP when there is no camera
        Camera camera = SemiFunc::MainCamera();
        if (!camera)
            return;

        // Dont draw ESP when player pressed ESC
        MenuManager menuManager = MenuManager::instance();
        if (!menuManager || menuManager.currentMenuPage())
            return;

        // Draw ESP
        if (g_EspSettings.m_On)
            DrawEnemiesEsp();
        if (g_ExtractionEsp)
            DrawPointsEsp();
        if (g_ValuablesEsp.m_On)
            DrawValuablesEsp();
        if (g_PlayersEsp)
            DrawPlayersEsp();
        if (g_SpawnEsp)
            DrawSpawnEsp();
    }

    void Menu()
    {
        // Use HaxSdk RAII implementations if System.Exception can be thrown
        if (HaxWindow window("R.E.P.O. Cheat Menu (by Sacracia)", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse); window)
        {
            if (HaxTable main("main", 3, ImGuiTableFlags_SizingStretchProp); main)
            {
                bool inGame = IsInGame();

                if (ImGui::TableNextColumn())
                {
                    SettingsTab();
                    HealthTab(inGame);
                    MovementTab();
                    EnemiesTab(inGame);
                    MapTab(inGame);
                    PointsTab(inGame);
                }

                if (ImGui::TableNextColumn())
                {
                    ValuablesTab();
                    GrabTab();
                    ItemsTab(inGame);
                    LevelTab(inGame);
                    UpgradesTab(inGame);
                }

                if (ImGui::TableNextColumn())
                {
                    PlayersTab(inGame);
                    MiscTab(inGame);
#ifdef _FREE
                    LinksTab();
#endif
                }
            }
        }
    }

    //-----------------------------------------------------------------------------
    // [SECTION] Tabs
    //-----------------------------------------------------------------------------

    static void SettingsTab()
    {
        // Is map faster than unordered map in this case?
        static const std::map<int, const char*>  s_Hotkeys =
        {
            {VK_INSERT, "Insert"},
            {VK_F1, "F1"},
            {VK_OEM_3, "~"}
        };

        ImGui::SeparatorText(Locales::SETTINGS[g_Lang]);
        int hotkey = HaxSdk::GetMenuHotkey();

        // Hotkey selection
        if (ImGui::BeginCombo(Locales::HOTKEY[g_Lang], s_Hotkeys.at(hotkey), ImGuiComboFlags_WidthFitPreview))
        {
            for (const auto& p : s_Hotkeys)
            {
                bool isSelected = (hotkey == p.first);
                if (ImGui::Selectable(p.second, &isSelected))
                {
                    HaxSdk::SetMenuHotkey(p.first);
                    ImGui::GetIO().WantSaveIniSettings = true;
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
        HaxSdk::Widgets::HelpMarker(Locales::SHOWHIDE[g_Lang]);

        // Language selection
        if (HaxTable langTable("table1", 2, ImGuiTableFlags_SizingFixedSame); langTable)
        {
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text(Locales::LANGUAGE[g_Lang]);
            ImGui::SameLine();

            if (ImGui::RadioButton("ENG", &g_Lang, 0))
                ImGui::GetIO().WantSaveIniSettings = true;
            ImGui::TableNextColumn();
            if (ImGui::RadioButton(RUS("РУ"), &g_Lang, 1))
                ImGui::GetIO().WantSaveIniSettings = true;
        }
    }

    static void HealthTab(bool inGame)
    {
        ImGui::SeparatorText(Locales::HEALTH[g_Lang]);
        ImGui::Checkbox(Locales::GODMODE[g_Lang], &g_Godmode);
        if (g_Godmode)
        {
            ImGui::Indent();
            ImGui::Checkbox(Locales::HEAL_TO_MAX[g_Lang], &g_HealToMax);
            ImGui::Unindent();
        }

        bool enabled = inGame && PlayerAvatar::instance() && !PlayerAvatar::instance().deadSet();
        if (HaxSdk::Widgets::DisabledButton(Locales::HEAL_MAX[g_Lang], enabled))
            g_ShouldHealToMax = true;
    }

    static void MovementTab()
    {
        ImGui::SeparatorText(Locales::MOVEMENT[g_Lang]);
        ImGui::Checkbox(Locales::INF_STAMINA[g_Lang], &g_InfStamina);
        ImGui::Checkbox(Locales::INF_JUMPS[g_Lang], &g_InfJumps);
        ImGui::Checkbox(Locales::NEVER_TUMBLE[g_Lang], &g_NeverTumble);
        ImGui::SliderFloat(Locales::SPEED_MULT[g_Lang], &g_SpeedMult, 1.f, 5.f, "%.1f");
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text(Locales::CROUCH_SPEED[g_Lang]);
            ImGui::SameLine(); ImGui::RadioButton(Locales::DEFAULT_CROUCH[g_Lang], &g_CrouchSpeedType, SpeedType_Crouch);
            ImGui::SameLine(); ImGui::RadioButton(Locales::WALK_CROUCH[g_Lang], &g_CrouchSpeedType, SpeedType_Walk);
            ImGui::SameLine(); ImGui::RadioButton(Locales::SPRINT_CROUCH[g_Lang], &g_CrouchSpeedType, SpeedType_Sprint);

            ImGui::AlignTextToFramePadding();
            ImGui::Text(Locales::WALK_SPEED[g_Lang]);
            ImGui::SameLine(); ImGui::RadioButton(Locales::DEFAULT_WALK[g_Lang], &g_WalkSpeedType, SpeedType_Walk);
            ImGui::SameLine(); ImGui::RadioButton(Locales::SPRINT_WALK[g_Lang], &g_WalkSpeedType, SpeedType_Sprint);
        }
    }

    static void EnemiesTab(bool inGame)
    {
        ImGui::SeparatorText(Locales::ENEMIES[g_Lang]);
#ifndef _NEXUS
        HaxSdk::Widgets::PremiumCheckbox(Locales::ESP_ENEMIES[g_Lang], &g_EspSettings.m_On, true, g_Lang == 0 ? g_PatreonWM : g_BoostyWM, Locales::WATERMARK[g_Lang]);

        if (g_EspSettings.m_On)
        {
            ImGui::Indent();
            ImGui::Checkbox(Locales::SHOW_BORDERS[g_Lang], &g_EspSettings.m_ShowBorder);
            ImGui::Checkbox(Locales::SHOW_NAME[g_Lang], &g_EspSettings.m_ShowName);
            ImGui::Checkbox(Locales::SHOW_HEALTH[g_Lang], &g_EspSettings.m_ShowHp);
            ImGui::Unindent();
        }
#endif

        bool isHost = SemiFunc::IsMasterClientOrSingleplayer();
        EnemyDirector enemyDirector = EnemyDirector::instance();
        if (enemyDirector)
        {
            HaxSdk::Widgets::DisabledCheckbox(Locales::BLIND[g_Lang], &enemyDirector.debugNoVision(), isHost);
            HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY_2[g_Lang]);
        }

        ImGui::Checkbox(Locales::OHK[g_Lang], &g_OneHitKills);
        ImGui::SetNextItemWidth(200.f);
        ImGui::SliderInt(Locales::DAMAGE_MULT[g_Lang], &g_DamageMult, 1, 5);

        static int s_CurEnemy = 0;
        ImGui::Combo(Locales::ENEMY_SPAWN[g_Lang], &s_CurEnemy,
            [](void* data, int idx, const char** outText)
            {
                auto pool = static_cast<std::map<std::string, EnemySetup*>*>(data);
                auto it = pool->begin();
                std::advance(it, idx);
                *outText = it->first.c_str();
                return true;
            }, (void*)&g_EnemiesPool, (int)g_EnemiesPool.size());

        if (HaxSdk::Widgets::DisabledButton(Locales::SPAWN_ENEMY[g_Lang], inGame && isHost))
        {
            auto it = g_EnemiesPool.begin();
            std::advance(it, s_CurEnemy);
            g_EnemyToSpawn = it->second;
        };
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY[g_Lang]);

        if (HaxSdk::Widgets::DisabledButton(Locales::KILL_ALL[g_Lang], inGame))
            g_ShouldKillEnemies = true;
    }

    static void MapTab(bool inGame)
    {
        ImGui::SeparatorText(Locales::MAP[g_Lang]);
        ImGui::Checkbox(Locales::SHOW_ENEMIES[g_Lang], &g_ShowEnemiesOnMap);
        if (HaxSdk::Widgets::DisabledButton(Locales::REVEAL_ROOMS[g_Lang], inGame))
            g_ShouldRevealRooms = true;
    }

    static void PointsTab(bool inGame)
    {
        RoundDirector roundDirector = RoundDirector::instance();

        ImGui::SeparatorText(Locales::EXTRACTION_POINTS[g_Lang]);
        ImGui::Checkbox(Locales::SHOW_POINTS[g_Lang], &g_ExtractionEsp);
        ImGui::SameLine(), ImGui::ColorEdit4("Color##1", g_ExtractionColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        if (HaxSdk::Widgets::DisabledButton(Locales::ACTIVATE_NEXT[g_Lang], inGame && roundDirector && !roundDirector.extractionPointActive()))
            g_ShouldActivateNextPoint = true;

        if (HaxSdk::Widgets::DisabledButton(Locales::FILL_QUOTA[g_Lang], inGame && roundDirector && roundDirector.extractionPointActive()))
            g_ShouldFillQuota = true;

        if (HaxSdk::Widgets::DisabledButton(Locales::UNLOCK_POINTS[g_Lang], inGame))
            g_ShouldUnlockAllPoints = true;
    }

    static void ValuablesTab()
    {
        ImGui::SeparatorText(Locales::VALUABLES[g_Lang]);
        HaxSdk::Widgets::DisabledCheckbox(Locales::NO_IMPACT[g_Lang], &g_NoImpact, SemiFunc::IsMasterClientOrSingleplayer());
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY_2[g_Lang]);
        ImGui::Checkbox(Locales::ITEMS_ESP[g_Lang], &g_ValuablesEsp.m_On);
        if (g_ValuablesEsp.m_On)
        {
            ImGui::Indent();
            ImGui::Checkbox(Locales::SHOW_PRICE[g_Lang], &g_ValuablesEsp.m_ShowPrice);
            ImGui::Checkbox(Locales::LIMIT_DIST[g_Lang], &g_ValuablesEsp.m_LimitDist);
            ImGui::Unindent();
        }
        if (HaxSdk::Widgets::DisabledButton(Locales::REVEAL_ITEMS[g_Lang], IsInGame()))
            g_ShouldRevealValuables = true;
    }

    static void GrabTab()
    {
        ImGui::SeparatorText(Locales::GRAB[g_Lang]);
        HaxSdk::Widgets::DisabledCheckbox(Locales::GRAB_PERMANENT[g_Lang], &g_InfiniteGrab, SemiFunc::IsMasterClientOrSingleplayer());
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY_2[g_Lang]);

        ImGui::Checkbox(Locales::GRAB_RANGE[g_Lang], &g_HighGrabRange);
    }

    static void ItemsTab(bool inGame)
    {
        ImGui::SeparatorText(Locales::ITEMS[g_Lang]);

        static int s_CurItem = 0;
        ImGui::Combo(Locales::ITEM_SPAWN[g_Lang], &s_CurItem,
            [](void* data, int idx, const char** outText)
            {
                auto pool = static_cast<std::map<std::string, uint32_t>*>(data);
                auto it = pool->begin();
                std::advance(it, idx);
                *outText = it->first.c_str();
                return true;
            }, (void*)&g_ItemsPool, static_cast<int>(g_ItemsPool.size()));

        bool isHost = SemiFunc::IsMasterClientOrSingleplayer();
        if (HaxSdk::Widgets::DisabledButton(Locales::SPAWN_ITEM[g_Lang], inGame && isHost))
        {
            auto it = g_ItemsPool.begin();
            std::advance(it, s_CurItem);
            g_ItemToSpawn = it->second;
        };
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY[g_Lang]);

        HaxSdk::Widgets::DisabledCheckbox(Locales::MAX_BATTERY[g_Lang], &g_InfBattery, isHost);
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY_2[g_Lang]);
        HaxSdk::Widgets::DisabledCheckbox(Locales::NO_SPREAD[g_Lang], &g_NoSpread, isHost);
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY_2[g_Lang]);
        ImGui::Checkbox(Locales::LASER_CROSSHAIR[g_Lang], &g_UseGunLaser);
    }

    static void LevelTab(bool inGame)
    {
        ImGui::SeparatorText(Locales::LEVELS[g_Lang]);

        bool isHost = SemiFunc::IsMasterClientOrSingleplayer();
        if (HaxSdk::Widgets::DisabledButton(Locales::NEXT_LEVEL[g_Lang], inGame && isHost))
            g_ShouldChangeLevel = true;
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY[g_Lang]);

        ImGui::Text(Locales::ALLOWED_LEVELS[g_Lang]);
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY[g_Lang]);
        for (LevelBan& levelBan : g_LevelBans)
        {
            if (ImGui::Selectable(levelBan.m_Name.c_str(), levelBan.m_Allowed))
            {
                if (levelBan.m_Allowed && LevelBan::s_TotalBans + 1 < g_LevelBans.size())
                {
                    levelBan.m_Allowed = false;
                    ++LevelBan::s_TotalBans;
                }
                else if (!levelBan.m_Allowed)
                {
                    levelBan.m_Allowed = true;
                    --LevelBan::s_TotalBans;
                }
            }
        }

        ImGui::SetNextItemWidth(50.f);
        static char lvlBuff[4] = ""; ImGui::InputText(Locales::LEVELS_COMPLETED[g_Lang], lvlBuff, sizeof(lvlBuff), ImGuiInputTextFlags_CharsDecimal);
        static bool showError = false;
        ImGui::SameLine();

        RunManager runManager = RunManager::instance();
        if (HaxSdk::Widgets::DisabledButton(Locales::APPLY[g_Lang], inGame && isHost && runManager))
        {
            unsigned int res;
            auto [ptr, ec] = std::from_chars(lvlBuff, lvlBuff + sizeof(lvlBuff), res);
            showError = !(ec == std::errc() && ptr == lvlBuff + strlen(lvlBuff));
            if (!showError)
                runManager.levelsCompleted() = res;
        }
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY[g_Lang]);
        if (showError)
        {
            ImGui::SameLine();
            ImGui::Text(Locales::INVALID_INPUT[g_Lang]);
        }
    }

    static void UpgradesTab(bool inGame)
    {
        ImGui::SeparatorText(Locales::UPGRADES[g_Lang]);

        PlayerAvatar playerAvatar = PlayerAvatar::instance();
        PunManager punManager = PunManager::instance();
        if (!playerAvatar || !punManager)
            return;

        StatsManager stats = punManager.statsManager();
        String steamId = SemiFunc::PlayerGetSteamID(playerAvatar);

        if (HaxTable statsTable("Stats_Upgrades", 2, ImGuiTableFlags_SizingStretchProp); statsTable)
        {
            char buff[64];

            if (stats.playerUpgradeStrength().ContainsKey(steamId))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                int& statValue = stats.playerUpgradeStrength().GetItem(steamId);
                memset(buff, 0, sizeof(buff));
                sprintf_s(buff, Locales::STRENGTH[g_Lang], statValue);
                ImGui::Text(buff);

                ImGui::TableNextColumn();
                if (HaxSdk::Widgets::DisabledButton("-##strength", inGame && statValue > 0))
                {
                    g_PretendMaster = true;
                    statValue -= 2;
                    punManager.UpgradePlayerGrabStrength(steamId);
                }

                ImGui::SameLine();

                if (HaxSdk::Widgets::DisabledButton("+##strength", inGame))
                {
                    g_PretendMaster = true;
                    punManager.UpgradePlayerGrabStrength(steamId);
                }
            }

            if (stats.playerUpgradeThrow().ContainsKey(steamId))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                int& statValue = stats.playerUpgradeThrow().GetItem(steamId);
                memset(buff, 0, sizeof(buff));
                sprintf_s(buff, Locales::THROW[g_Lang], statValue);
                ImGui::Text(buff);

                ImGui::TableNextColumn();
                if (HaxSdk::Widgets::DisabledButton("-##throw", inGame && statValue > 0))
                {
                    g_PretendMaster = true;
                    statValue -= 2;
                    punManager.UpgradePlayerThrowStrength(steamId);
                }

                ImGui::SameLine();

                if (HaxSdk::Widgets::DisabledButton("+##throw", inGame))
                {
                    g_PretendMaster = true;
                    punManager.UpgradePlayerThrowStrength(steamId);
                }
            }

            if (stats.playerUpgradeStamina().ContainsKey(steamId))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                int& statValue = stats.playerUpgradeStamina().GetItem(steamId);
                memset(buff, 0, sizeof(buff));
                sprintf_s(buff, Locales::STAMINA[g_Lang], statValue);
                ImGui::Text(buff);

                ImGui::TableNextColumn();
                if (HaxSdk::Widgets::DisabledButton("-##stamina", inGame && statValue > 0))
                {
                    g_PretendMaster = true;
                    statValue -= 2;
                    punManager.UpgradePlayerEnergy(steamId);
                }

                ImGui::SameLine();

                if (HaxSdk::Widgets::DisabledButton("+##stamina", inGame))
                {
                    g_PretendMaster = true;
                    punManager.UpgradePlayerEnergy(steamId);
                }
            }

            if (stats.playerUpgradeSpeed().ContainsKey(steamId))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                int& statValue = stats.playerUpgradeSpeed().GetItem(steamId);
                memset(buff, 0, sizeof(buff));
                sprintf_s(buff, Locales::SPEED[g_Lang], statValue);
                ImGui::Text(buff);

                ImGui::TableNextColumn();
                if (HaxSdk::Widgets::DisabledButton("-##speed", inGame && statValue > 0))
                {
                    g_PretendMaster = true;
                    statValue -= 2;
                    punManager.UpgradePlayerSprintSpeed(steamId);
                }

                ImGui::SameLine();

                if (HaxSdk::Widgets::DisabledButton("+##speed", inGame))
                {
                    g_PretendMaster = true;
                    punManager.UpgradePlayerSprintSpeed(steamId);
                }
            }

            if (stats.playerUpgradeLaunch().ContainsKey(steamId))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                int& statValue = stats.playerUpgradeLaunch().GetItem(steamId);
                memset(buff, 0, sizeof(buff));
                sprintf_s(buff, Locales::TUMBLE_LAUNCH[g_Lang], statValue);
                ImGui::Text(buff);

                ImGui::TableNextColumn();
                if (HaxSdk::Widgets::DisabledButton("-##tumble", inGame && statValue > 0))
                {
                    statValue -= 2;
                    punManager.UpgradePlayerTumbleLaunch(steamId);
                }

                ImGui::SameLine();

                if (HaxSdk::Widgets::DisabledButton("+##tumble", inGame))
                    punManager.UpgradePlayerTumbleLaunch(steamId);
            }

            if (stats.playerUpgradeHealth().ContainsKey(steamId))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                int& statValue = stats.playerUpgradeHealth().GetItem(steamId);
                memset(buff, 0, sizeof(buff));
                sprintf_s(buff, Locales::HEALTH_STAT[g_Lang], statValue);
                ImGui::Text(buff);

                ImGui::TableNextColumn();
                if (HaxSdk::Widgets::DisabledButton("-##health", inGame && statValue > 0))
                {
                    statValue -= 2;
                    punManager.UpgradePlayerHealth(steamId);
                }

                ImGui::SameLine();

                if (HaxSdk::Widgets::DisabledButton("+##health", inGame))
                    punManager.UpgradePlayerHealth(steamId);
            }

            if (stats.playerUpgradeMapPlayerCount().ContainsKey(steamId))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                int& statValue = stats.playerUpgradeMapPlayerCount().GetItem(steamId);
                memset(buff, 0, sizeof(buff));
                sprintf_s(buff, Locales::PLAYERS_COUNT[g_Lang], statValue);
                ImGui::Text(buff);

                ImGui::TableNextColumn();
                if (HaxSdk::Widgets::DisabledButton("-##players", inGame && statValue > 0))
                {
                    statValue -= 2;
                    punManager.UpgradeMapPlayerCount(steamId);
                }

                ImGui::SameLine();

                if (HaxSdk::Widgets::DisabledButton("+##players", inGame))
                    punManager.UpgradeMapPlayerCount(steamId);
            }

            if (stats.playerUpgradeExtraJump().ContainsKey(steamId))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                int& statValue = stats.playerUpgradeExtraJump().GetItem(steamId);
                memset(buff, 0, sizeof(buff));
                sprintf_s(buff, Locales::PLAYERS_COUNT[g_Lang], statValue);
                ImGui::Text(buff);

                ImGui::TableNextColumn();
                if (HaxSdk::Widgets::DisabledButton("-##jumps", inGame && statValue > 0))
                {
                    statValue -= 2;
                    punManager.UpgradePlayerExtraJump(steamId);
                }

                ImGui::SameLine();

                if (HaxSdk::Widgets::DisabledButton("+##jumps", inGame))
                    punManager.UpgradePlayerExtraJump(steamId);
            }
        }
    }

    static void PlayersTab(bool inGame)
    {
        ImGui::SeparatorText(Locales::PLAYERS[g_Lang]);

        if (!IsInGame())
            return;

        static int s_PlayerIndex = 0;
        List<PlayerAvatar> players = GameDirector::instance().PlayerList();
        if (s_PlayerIndex >= players.GetCount() || s_PlayerIndex < 0)
            s_PlayerIndex = players.GetCount() - 1;

        PlayerAvatar player = s_PlayerIndex < 0 ? PlayerAvatar(nullptr) : players[s_PlayerIndex];
        String name = player.playerName();
        if (!name)
            player = PlayerAvatar(nullptr);

        if (HaxCombo combo(Locales::PLAYER[g_Lang], player ? name.UTF8() : "", ImGuiComboFlags_WidthFitPreview); combo)
        {
            for (int n = 0; n < players.GetCount(); n++)
            {
                const bool is_selected = (s_PlayerIndex == n);
                if (ImGui::Selectable(players[n].playerName().UTF8(), is_selected))
                    s_PlayerIndex = n;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
        }

        if (PlayerVoiceChat chat = PlayerAvatar::instance().voiceChat())
        {
            bool isSpectator = chat.inLobbyMixer();
            ImGui::AlignTextToFramePadding();
            ImGui::Text(Locales::VOICE_CHAT[g_Lang], isSpectator ? Locales::SPECTATOR[g_Lang] : Locales::ALIVE[g_Lang]);
            ImGui::SameLine();
            if (HaxSdk::Widgets::DisabledButton(Locales::SWITCH[g_Lang], inGame))
                g_SwitchParams = SwitchChatParams(true, chat, !isSpectator);
        }

        bool isHost = SemiFunc::IsMasterClientOrSingleplayer();

        if (!player || !player.isLocal())
        {
            if (HaxSdk::Widgets::DisabledButton(Locales::KILL[g_Lang], inGame && isHost && player && !player.deadSet()))
                g_PlayerToKill = player;
            HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY[g_Lang]);
        }

        if (player && player.isLocal())
        {
            if (HaxSdk::Widgets::DisabledButton(Locales::KILL[g_Lang], inGame && isHost && !player.deadSet()))
                g_PlayerToKill = player;
            HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY[g_Lang]);
        }

        if (HaxSdk::Widgets::DisabledButton(Locales::REVIVE[g_Lang], inGame && player && isHost && player.deadSet()))
            g_PlayerToRevive = player;
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY[g_Lang]);

        if (HaxSdk::Widgets::DisabledButton(Locales::OVERCHARGE[g_Lang], inGame && player && !player.deadSet()))
            g_PlayerToOvercharge = player;

        ImGui::Checkbox(Locales::SHOW_PLAYERS[g_Lang], &g_PlayersEsp);
    }

    static void MiscTab(bool inGame)
    {
        bool isHost = SemiFunc::IsMasterClientOrSingleplayer();

        ImGui::SeparatorText(Locales::MISC[g_Lang]);
        bool fog = !RenderSettings::GetFog();
        if (ImGui::Checkbox(Locales::IMPROVE_VISION[g_Lang], &fog))
            RenderSettings::SetFog(!fog);

        RunManager runManager = RunManager::instance();
        if (HaxSdk::Widgets::DisabledButton(Locales::INF_MONEY[g_Lang], inGame && isHost && runManager.levelCurrent() == runManager.levelShop()))
            g_ShouldSetInfMoney = true;
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY[g_Lang]);

        ImGui::Checkbox(Locales::DELETE_SAVE[g_Lang], &g_NoSaveDelete);
        HaxSdk::Widgets::WarningMarker(Locales::HOST_ONLY[g_Lang]);

        ImGui::Checkbox(Locales::SHOW_SPAWN[g_Lang], &g_SpawnEsp);
        ImGui::SameLine();
        ImGui::ColorEdit4("Color##2", g_SpawnColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    }

    static void LinksTab()
    {
        ImGui::SeparatorText(Locales::LINKS[g_Lang]);
        HaxTexture img = g_Lang > 0 ? g_BoostyLink : g_PatreonLink;
        if (ImGui::ImageButton(img.m_pTexture, ImVec2(img.m_width, img.m_height) * HaxSdk::GetScaleFactor(), ImVec2(0, 0), ImVec2(1, 1), 0))
            ShellExecute(0, 0, g_Lang == 1 ? "https://boosty.to/sacraciamods" : "https://www.patreon.com/sacracia", 0, 0, SW_SHOW);
        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    //-----------------------------------------------------------------------------
    // [SECTION] ESP
    //-----------------------------------------------------------------------------

    static void DrawEnemiesEsp()
    {
        if (!g_EspSettings.m_On)
            return;

        Camera cam = SemiFunc::MainCamera();
        if (!cam)
            return;

        float screenHeight = (float)Screen::GetHeight();

        EnemyDirector enemyDirector = EnemyDirector::instance();
        if (!enemyDirector)
            return;

        for (EnemyParent enemyParent : enemyDirector.enemiesSpawned())
        {
            if (!enemyParent || !enemyParent.Spawned())
                continue;

            Enemy enemy = enemyParent.enemy();
            if (!enemy)
                continue;

            // Dont draw if spewer is on player's face
            String enemyName = enemyParent.enemyName();
            if (enemyName.StartsWith(L"Spewer"))
            {
                if (!g_CachedSpewers.contains(enemyParent))
                {
                    EnemySlowMouth slowMouth = enemyParent.GetComponentInChildren<EnemySlowMouth>();
                    g_CachedSpewers[enemyParent] = slowMouth;
                    HAX_LOG_DEBUG("Cached spewer. Cache size {}", g_CachedSpewers.size());
                }

                EnemySlowMouth slowMouth = g_CachedSpewers[enemyParent];
                if (slowMouth && slowMouth.attachedTimer() > 0.f)
                    return;
            }

            Vector3 enemyPos = enemy.GetTransform().GetPosition();
            float dist = Vector3::Distance(enemyPos, PlayerAvatar::instance().GetTransform().GetPosition());
            // Dont draw when enemies are too close
            if (dist < 2.f)
                continue;

            Bounds bigBounds = Bounds(enemy.CenterTransform().GetPosition(), Vector3::zero());
            if (!enemyName.Contains(L"Peeper"))
            {
                PhysGrabObject grabObject = enemy.Rigidbody().physGrabObject();
                if (!g_CachedColliders.contains(grabObject))
                {
                    Array<Collider> colliders = enemy.Rigidbody().physGrabObject().GetComponentsInChildren<Collider>();

                    std::vector<uint32_t> v;
                    v.reserve(colliders.GetLength());

                    for (Collider collider : colliders)
                        v.push_back(unsafe::GCHandle::NewWeak(collider.GetPointer(), false));

                    g_CachedColliders[grabObject] = std::move(v);
                    HAX_LOG_DEBUG("Cached colliders. Cache size {}", g_CachedColliders.size());
                }

                for (uint32_t handle : g_CachedColliders[grabObject])
                {
                    Collider collider(unsafe::GCHandle::GetTarget(handle));
                    if (collider)
                        bigBounds.Encapsulate(collider.GetBounds());
                }
            }
            else
            {
                if (!g_CachedMeshes.contains(enemyParent))
                {
                    Array<MeshRenderer> meshes = enemyParent.GetComponentsInChildren<MeshRenderer>();

                    std::vector<uint32_t> v;
                    v.reserve(meshes.GetLength());

                    for (MeshRenderer mesh : meshes)
                        v.push_back(unsafe::GCHandle::NewWeak(mesh.GetPointer(), false));

                    g_CachedMeshes[enemyParent] = std::move(v);
                    HAX_LOG_DEBUG("Cached meshes. Cache size {}", g_CachedMeshes.size());
                }

                for (uint32_t handle : g_CachedMeshes[enemyParent])
                {
                    MeshRenderer mesh(unsafe::GCHandle::GetTarget(handle));
                    if (mesh)
                        bigBounds.Encapsulate(mesh.GetBounds());
                }
            }

            if (SemiFunc::OnScreen(bigBounds.m_Center, 0.5f, 0.5f))
            {
                Rect rect = CalcBoundsInScreenSpace(bigBounds, cam);

                if (g_EspSettings.m_ShowBorder)
                    HaxSdk::ESP::CornerBox(rect.x, rect.y, rect.width, rect.height, 0xFF0000FF, 0.25f);
                if (g_EspSettings.m_ShowName)
                    HaxSdk::ESP::Text(g_EspFont, enemyName.UTF8(), ImVec2(rect.GetCenter().x, rect.y - 2.f), 0xFFFFFFFF, 15.f, VerticalAlignment_Top, HorizontalAlignment_Center);
                if (g_EspSettings.m_ShowName)
                {
                    EnemyHealth enemyHealth = enemy.Health();
                    int curHp = enemyHealth.healthCurrent();
                    int maxHp = enemyHealth.health();
                    HaxSdk::ESP::VertHealthBar(rect.x - 7.f, rect.y, 2.f, rect.height, curHp, maxHp);

                    char buff[8] = {0};
                    sprintf_s(buff, "%d", curHp);
                    ImU32 lerped = ImGui::ColorConvertFloat4ToU32(ImLerp(ImVec4(1.f, 0.f, 0.f, 1.f), ImVec4(0.f, 1.f, 0.f, 1.f), (float)curHp / maxHp));
                    HaxSdk::ESP::Text(g_EspFont, buff, ImVec2(rect.x - 9.f, rect.y), lerped, 13.f);
                }
            }
        }
    }

    static void DrawPointsEsp()
    {
        RoundDirector roundDirector = RoundDirector::instance();
        if (g_ExtractionEsp && roundDirector) 
        {
            Camera cam = SemiFunc::MainCamera();
            if (!cam)
                return;

            float screenHeight = (float)Screen::GetHeight();
            float screenWidth = (float)Screen::GetWidth();

            for (GameObject pointGo : roundDirector.extractionPointList()) 
            {
                if (!pointGo)
                    continue;

                ExtractionPoint point = pointGo.GetComponent<ExtractionPoint>();
                if (!point)
                    continue;

                Unity::Vector3 worldPos = pointGo.GetTransform().GetPosition();
                Unity::Vector3 screenPos = cam.WorldToScreenPoint(worldPos, g_ViewMatrix, g_ProjMatrix, screenWidth, screenHeight);
                if (screenPos.z > 0.f) 
                {
                    if (point.currentState() != ExtractionPoint_State::Complete()) 
                    {
                        ImU32 color = point.currentState() == ExtractionPoint_State::Active() ? ImGui::ColorConvertFloat4ToU32(ImVec4(g_ExtractionColor[0], g_ExtractionColor[1], g_ExtractionColor[2], g_ExtractionColor[3])) : 0xFF3D3D3D;
                        HaxSdk::ESP::Text(g_IconsFont, "\uE900", ImVec2(screenPos.x, screenPos.y), color, HaxSdk::GetScaleFactor() * 32.f);
                    }
                }
            }
        }
    }

    static void DrawValuablesEsp()
    {
        if (!g_ValuablesEsp.m_On)
            return;

        ValuableDirector valuablesDirector = ValuableDirector::instance();
        if (!valuablesDirector)
            return;

        for (ValuableObject valuableObject : valuablesDirector.valuableList()) 
        {
            if (!valuableObject)
                continue;

            PhysGrabObject grabObject = valuableObject.physGrabObject();
            if (!grabObject)
                continue;

            GameObject gameObject = valuableObject.GetGameObject();
            if (!gameObject || !gameObject.GetActiveInHierarchy())
                continue;

            PlayerAvatar player = PlayerAvatar::instance();
            if (!player)
                continue;

            Unity::Vector3& worldPos = grabObject.centerPoint();
            if (g_ValuablesEsp.m_LimitDist && Vector3::Distance(worldPos, player.GetTransform().GetPosition()) > 16.f)
                continue;

            Unity::Vector3 pos = SemiFunc::MainCamera().WorldToScreenPoint(worldPos, g_ViewMatrix, g_ProjMatrix, (float)Screen::GetWidth(), (float)Screen::GetHeight());

            if (pos.z > 0) 
            {
                String valuableName = gameObject.GetName();
                if (valuableName.GetLength() == 0)
                    continue;
                char* utf8 = valuableName.UTF8();
                if (valuableName.EndsWith(L"(Clone)"))
                    *(utf8 + strlen(utf8) - 7) = '\0';
                if (valuableName.StartsWith(L"Valuable "))
                    utf8 += 9;
                if (g_ValuablesEsp.m_ShowPrice) 
                {
                    char buff2[64] = {0};
                    if (sprintf_s(buff2, "%s (%.0f$)", utf8, valuableObject.dollarValueCurrent()) > 0)
                    {
                        HaxSdk::ESP::Text(g_EspFont, buff2, ImVec2(pos.x, pos.y), 0xFF00FFFF, HaxSdk::GetScaleFactor() * 13.f, VerticalAlignment_Bottom, HorizontalAlignment_Center);
                        continue;
                    }
                }
                HaxSdk::ESP::Text(g_EspFont, utf8, ImVec2(pos.x, pos.y), 0xFF00FFFF, HaxSdk::GetScaleFactor() * 13.f, VerticalAlignment_Bottom, HorizontalAlignment_Center);
            }
        }
    }

    static void DrawPlayersEsp()
    {
        if (!g_PlayersEsp)
            return;

        for (PlayerAvatar playerAvatar : GameDirector::instance().PlayerList())
        {
            if (!playerAvatar || playerAvatar.isLocal())
                continue;

            bool isDead = playerAvatar.deadSet();
            if (isDead && !playerAvatar.playerDeathHead())
                continue;

            Transform transform = isDead ? playerAvatar.playerDeathHead().GetTransform() : playerAvatar.GetTransform();
            Vector3 worldPos = transform.GetPosition();
            Vector3 screenPos = SemiFunc().MainCamera().WorldToScreenPoint(worldPos, g_ViewMatrix, g_ProjMatrix, (float)Screen::GetWidth(), (float)Screen::GetHeight());
            if (screenPos.z > 0.f)
                HaxSdk::ESP::Text(g_EspFont, playerAvatar.playerName().UTF8(), ImVec2(screenPos.x, screenPos.y), isDead ? 0xFF3D3D3D : 0xFF009C0A, HaxSdk::GetScaleFactor() * 13.f, VerticalAlignment_Bottom, HorizontalAlignment_Center);
        }
    }

    static void DrawSpawnEsp()
    {
        TruckSafetySpawnPoint track = TruckSafetySpawnPoint::instance();
        if (track) 
        {
            Unity::Vector3 worldPos = track.GetTransform().GetPosition();
            Unity::Vector3 screenPos = SemiFunc::MainCamera().WorldToScreenPoint(worldPos, g_ViewMatrix, g_ProjMatrix, (float)Screen::GetWidth(), (float)Screen::GetHeight());
            if (screenPos.z > 0.f)
                HaxSdk::ESP::Text(g_IconsFont, "\uE9B0", ImVec2(screenPos.x, screenPos.y), ImGui::ColorConvertFloat4ToU32(ImVec4(g_SpawnColor[0], g_SpawnColor[1], g_SpawnColor[2], g_SpawnColor[3])), 32);
        }
    }
   
    //-----------------------------------------------------------------------------
    // [SECTION] Hooks
    //-----------------------------------------------------------------------------

    static void Hooked__Camera_FireOnPreRender(Camera cam)
    {
        if (cam == SemiFunc::MainCamera())
        {
            cam.GetWorldToCameraMatrix_Injected(&g_ViewMatrix);
            cam.GetProjectionMatrix_Injected(&g_ProjMatrix);
        }
        ((void(*)(Camera))Camera_FireOnPreRender.m_Orig)(cam);
    }

    static void Hooked__PlayerHealth_Hurt(PlayerHealth __this, int dmg, bool savingGrace, int enemyIndex)
    {
        try
        {
            if (g_Godmode && __this == PlayerAvatar::instance().playerHealth())
            {
                if (g_HealToMax)
                    __this.HealOther(99999, true);
                return;
            }
        }
        catch (System::Exception& ex)
        {
            HAX_LOG_ERROR("{}", ex.GetMessage().UTF8());
        }

        ((void(*)(PlayerHealth, int, bool, int))PlayerHealth::s_Hurt.m_Orig)(__this, dmg, savingGrace, enemyIndex);
    }

    static void Hooked__PlayerController_FixedUpdate(PlayerController __this)
    {
        float& moveSpeed = __this.MoveSpeed();
        float& sprintSpeed = __this.SprintSpeed();
        float& crouchSpeed = __this.CrouchSpeed();

        float cachedMovespeed = moveSpeed;
        float cachedSprintSpeed = sprintSpeed;
        float cachedCrouchSpeed = crouchSpeed;

        if (g_InfStamina)
            __this.EnergyCurrent() = __this.EnergyStart();

        if (g_SpeedMult > 1.f)
        {
            moveSpeed *= g_SpeedMult;
            sprintSpeed *= g_SpeedMult;
            crouchSpeed *= g_SpeedMult;
        }

        if (g_CrouchSpeedType == SpeedType_Walk)
            crouchSpeed = moveSpeed;
        if (g_CrouchSpeedType == SpeedType_Sprint)
            crouchSpeed = sprintSpeed;
        if (g_WalkSpeedType == SpeedType_Sprint)
            moveSpeed = sprintSpeed;

        ((void(*)(PlayerController))PlayerController::s_FixedUpdate.m_Orig)(__this);

        moveSpeed = cachedMovespeed;
        sprintSpeed = cachedSprintSpeed;
        crouchSpeed = cachedCrouchSpeed;
    }

    static void Hooked__PlayerController_Update(PlayerController __this)
    {
        int& jumps = __this.JumpExtra();
        int cachedJumps = jumps;
        if (g_InfJumps)
            jumps = 9999;

        ((void(*)(PlayerController))PlayerController::s_Update.m_Orig)(__this);

        jumps = cachedJumps;
    }

    static void Hooked__PlayerTumble_TumbleRequest(PlayerTumble __this, bool isTumbling, bool playerInput)
    {
        if (!g_NeverTumble || !isTumbling)
            ((void(*)(PlayerTumble, bool, bool))PlayerTumble::s_TumbleRequest.m_Orig)(__this, isTumbling, playerInput);
    }

    // Safe hook where we can call functions inside Unity PlayerLoop.
    // EventSystem can be found in all games and no mods use it.
    static void Hooked__EventSystem_Update(EventSystem __this)
    {
        if (__this != EventSystem::GetCurrent())
            return;

        try
        {
            PlayerAvatar player = PlayerAvatar::instance();
            EnemyDirector enemyDirector = EnemyDirector::instance();
            LevelGenerator levelGenerator = LevelGenerator::Instance();
            RunManager runManager = RunManager::instance();

            if (g_ShouldHealToMax && player && player.playerHealth())
            {
                g_ShouldHealToMax = false;
                player.playerHealth().HealOther(99999, true);
                HAX_LOG_DEBUG("Player {} healed to max", (void*)player.GetPointer());
            }

            if (g_EnemiesPool.size() == 0 && enemyDirector)
            {
                ParseEnemies(enemyDirector.enemiesDifficulty1());
                ParseEnemies(enemyDirector.enemiesDifficulty2());
                ParseEnemies(enemyDirector.enemiesDifficulty3());
            }

            if (g_ItemsPool.size() == 0)
                ParseItems();

            if (g_LevelBans.size() == 0)
                ParseLevels();

            if (g_EnemyToSpawn && levelGenerator && levelGenerator.Generated())
            {
                List<LevelPoint> points = levelGenerator.LevelPathPoints();
                if (points.GetCount() > 0 && points[0])
                    levelGenerator.EnemySpawn(g_EnemyToSpawn, points[0].GetTransform().GetPosition());
                else
                    HAX_LOG_WARN("Enemy wasn\'t spawned as no points found");
                g_EnemyToSpawn = null;
            }

            if (g_ShouldKillEnemies)
            {
                g_ShouldKillEnemies = false;
                if (EnemyDirector enemyDirector = EnemyDirector::instance())
                {
                    for (EnemyParent enemyParent : enemyDirector.enemiesSpawned())
                    {
                        if (enemyParent && enemyParent.enemy() && enemyParent.enemy().Health())
                            enemyParent.enemy().Health().Hurt(9999, Vector3::one());
                    }
                }
            }

            if (IsInGame() && enemyDirector)
            {
                for (EnemyParent enemyParent : enemyDirector.enemiesSpawned())
                {
                    if (!enemyParent || !enemyParent.enemy())
                        continue;

                    Enemy enemy = enemyParent.enemy();
                    int enemyState = enemy.CurrentState();
                    if (enemyState == EnemyState::None() || enemyState == EnemyState::Spawn() || enemyState == EnemyState::Despawn())
                        continue;

                    uint32_t handle = g_CachedEnemiesOnMap[enemyParent];
                    MapCustom mapCustom = MapCustom(handle > 0 ? unsafe::GCHandle::GetTarget(handle) : nullptr);

                    if (!mapCustom)
                    {
                        if (handle > 0)
                            unsafe::GCHandle::Free(handle);

                        mapCustom = enemyParent.GetComponent<MapCustom>();
                        if (!mapCustom)
                        {
                            mapCustom = enemy.GetGameObject().AddComponent<MapCustom>();
                            mapCustom.SetName(String::Concat(enemyParent.GetName(), String::New("_Point")));
                            mapCustom.color() = Color::red();
                            mapCustom.sprite() = PlayerAvatar::instance().playerDeathHead().mapCustom().sprite();
                        }
                        g_CachedEnemiesOnMap[enemyParent] = unsafe::GCHandle::NewWeak(mapCustom.GetPointer(), false);
                        HAX_LOG_DEBUG("Cached enemy on map. Cache size {}", g_CachedEnemiesOnMap.size());
                    }

                    if (g_ShowEnemiesOnMap && !enemy.Health().dead())
                        continue;

                    mapCustom.Hide();
                }
            }

            if (g_ShouldRevealRooms)
            {
                g_ShouldRevealRooms = false;
                for (LevelPoint point : levelGenerator.LevelPathPoints())
                    if (point && point.Room())
                        point.Room().SetExplored();
            }

            if (g_ShouldFillQuota)
            {
                g_ShouldFillQuota = false;
                if (RoundDirector roundDirector = RoundDirector::instance())
                    if (ExtractionPoint point = roundDirector.extractionPointCurrent())
                        point.HaulGoalSet(0);
            }

            if (g_ShouldActivateNextPoint)
            {
                g_ShouldActivateNextPoint = false;
                if (RoundDirector roundDirector = RoundDirector::instance())
                {
                    for (GameObject go : roundDirector.extractionPointList())
                    {
                        if (!go)
                            continue;

                        ExtractionPoint point = go.GetComponent<ExtractionPoint>();
                        if (point && point.currentState() == ExtractionPoint_State::Idle() && !point.isLocked())
                            point.OnClick();
                    }
                }
            }

            if (g_ShouldUnlockAllPoints && RoundDirector::instance())
            {
                g_ShouldUnlockAllPoints = false;
                RoundDirector::instance().ExtractionPointsUnlock();
            }

            if (g_ShouldRevealValuables)
            {
                g_ShouldRevealValuables = false;
                if (ValuableDirector valuableDirector = ValuableDirector::instance())
                    for (ValuableObject valuableObject : valuableDirector.valuableList())
                        if (valuableObject)
                            valuableObject.Discover(ValuableDiscoverGraphic_State::Discover());
            }

            if (g_ItemToSpawn)
                SpawnItem();

            if (g_ShouldChangeLevel && runManager)
            {
                g_ShouldChangeLevel = false;
                runManager.ChangeLevel(true, false);
            }

            if (g_PlayerToKill)
            {
                PlayerAvatar copy = g_PlayerToKill;
                g_PlayerToKill = null;
                copy.PlayerDeath(0);
            }

            if (g_PlayerToRevive)
            {
                PlayerAvatar copy = g_PlayerToRevive;
                g_PlayerToRevive = null;
                copy.Revive();
            }

            if (g_PlayerToOvercharge)
            {
                PlayerAvatar copy = g_PlayerToOvercharge;
                g_PlayerToOvercharge = null;

                if (Photon::PhotonView photonView = copy.photonView())
                    if (PhysGrabber grabber = photonView.GetComponent<PhysGrabber>())
                        grabber.PhysGrabOverChargeImpact();
            }

            if (g_SwitchParams.m_ShouldSwitch)
            {
                g_SwitchParams.m_ShouldSwitch = false;
                g_SwitchParams.m_Chat.ToggleLobby(g_SwitchParams.m_NewVal);
            }

            if (g_ShouldSetInfMoney)
            {
                g_ShouldSetInfMoney = false;
                SemiFunc::StatSetRunCurrency(99999);
            }
        }
        catch (System::Exception& ex)
        {
            HAX_LOG_ERROR("{}", ex.GetMessage().UTF8());
        }

        ((void(*)(EventSystem))EventSystem_Update.m_Orig)(__this);
    }

    static void Hooked__PlayerAvatar_OnDestroy(PlayerAvatar __this)
    {
        ((void(*)(PlayerAvatar))PlayerAvatar::s_OnDestroy.m_Orig)(__this);

        HAX_LOG_DEBUG("Clearing cached enemy-colliders: size was {}", g_CachedColliders.size());
        for (auto& p : g_CachedColliders)
            for (uint32_t handle : p.second)
                unsafe::GCHandle::Free(handle);
        g_CachedColliders.clear();

        HAX_LOG_DEBUG("Clearing cached enemy-meshes: size was {}", g_CachedMeshes.size());
        for (auto& p : g_CachedMeshes)
            for (uint32_t handle : p.second)
                unsafe::GCHandle::Free(handle);
        g_CachedMeshes.clear();

        HAX_LOG_DEBUG("Clearing cached enemy-spewers: size was {}", g_CachedSpewers.size());
        g_CachedSpewers.clear();

        HAX_LOG_DEBUG("Clearing cached enemies on map: size was {}", g_CachedEnemiesOnMap.size());
        for (auto& p : g_CachedEnemiesOnMap)
            unsafe::GCHandle::Free(p.second);
        g_CachedEnemiesOnMap.clear();

        HAX_LOG_DEBUG("Clearing cached lasers: size was {}", g_CachedGunLasers.size());
        for (auto& p : g_CachedGunLasers)
            unsafe::GCHandle::Free(p.second);
        g_CachedGunLasers.clear();
    }

    static void Hooked__EnemyHealth_Hurt(EnemyHealth __this, int damage, void* hurtDirection)
    {
        damage *= g_DamageMult;
        if (g_OneHitKills)
            damage = __this.health();

        ((void(*)(EnemyHealth, int, void*))EnemyHealth::s_Hurt.m_Orig)(__this, damage, hurtDirection);
    }

    static bool Hooked__PhotonNetwork_IsMasterClient()
    {
        if (g_PretendMaster)
        {
            g_PretendMaster = false;
            return true;
        }
        return ((bool(*)())PhotonNetwork__IsMasterClient.m_Orig)();
    }

    static void Hooked__PhysGrabObjectImpactDetector_FixedUpdate(PhysGrabObjectImpactDetector __this)
    {
        float& timer = __this.impactDisabledTimer();
        if (g_NoImpact && __this.isValuable()) 
            timer = (std::max)(1.f, timer);

        ((void(*)(PhysGrabObjectImpactDetector))PhysGrabObjectImpactDetector::s_FixedUpdate.m_Orig)(__this);
    }

    static void Hooked__EnemyRigidbody_FixedUpdate(EnemyRigidbody __this)
    {
        EnemyDirector enemyDirector = EnemyDirector::instance();
        if (g_InfiniteGrab && enemyDirector)
        {
            __this.grabShakeReleaseTimer() = 0.f;
            enemyDirector.debugEasyGrab() = true;
            enemyDirector.debugNoGrabMaxTime() = true;
        }

        ((void(*)(EnemyRigidbody))EnemyRigidbody::s_FixedUpdate.m_Orig)(__this);

        if (g_InfiniteGrab && enemyDirector)
        {
            enemyDirector.debugEasyGrab() = false;
            enemyDirector.debugNoGrabMaxTime() = false;
        }
    }

    static void Hooked__PhysGrabber_PhysGrabLogic(PhysGrabber __this)
    {
        float& range = __this.grabRange();
        float cached = range;
        PlayerAvatar playerAvatar = PlayerAvatar::instance();

        if (g_HighGrabRange && playerAvatar && __this == playerAvatar.physGrabber())
            range = 999999.f;

        ((void(*)(PhysGrabber))PhysGrabber::s_PhysGrabLogic.m_Orig)(__this);

        range = cached;
    }

    static void Hooked__PhysGrabber_RayCheck(PhysGrabber __this, bool grab)
    {
        float& range = __this.grabRange();
        float cached = range;
        PlayerAvatar playerAvatar = PlayerAvatar::instance();

        if (g_HighGrabRange && playerAvatar && __this == playerAvatar.physGrabber()) 
        {
            range = 999999.f;
            g_RaycastMaxDist = true;
        }

        ((void(*)(PhysGrabber, bool))PhysGrabber::s_RayCheck.m_Orig)(__this, grab);

        range = cached;
        g_RaycastMaxDist = false;
    }

    static bool Hooked__Physics_Raycast(void* v1, void* v2, void* v3, float maxDistance, int v5, int v6)
    {
        if (g_RaycastMaxDist)
            maxDistance = 999999.f;

        return ((bool(*)(void*, void*, void*, float, int, int))Physics__Raycast.m_Orig)(v1, v2, v3, maxDistance, v5, v6);
    }

    static void Hooked__ItemBattery_Update(ItemBattery __this)
    {
        if (g_InfBattery && __this.batteryLife() < 100.f)
            __this.SetBatteryLife(100);

        ((void(*)(ItemBattery))ItemBattery::s_Update.m_Orig)(__this);
    }

    static void Hooked__ItemGun_Shoot(ItemGun __this)
    {
        float& spread = __this.gunRandomSpread();
        float cached = spread;
        if (g_NoSpread)
            spread = 0.f;

        ((void(*)(ItemGun))ItemGun::s_Shoot.m_Orig)(__this);

        spread = cached;
    }

    static void Hooked__ItemGun_Update(ItemGun __this)
    {
        uint32_t handle = g_CachedGunLasers[__this];
        LineRenderer laser = LineRenderer(handle > 0 ? unsafe::GCHandle::GetTarget(handle) : nullptr);

        if (!laser)
        {
            if (handle > 0)
                unsafe::GCHandle::Free(handle);

            laser = __this.GetComponent<LineRenderer>();
            if (!laser)
            {
                laser = __this.GetGameObject().AddComponent<LineRenderer>();
                laser.SetStartWidth(0.02f);
                laser.SetEndWidth(0.02f);
                laser.SetSortingOrder(1);
                laser.SetPositionCount(2);
                Material mat = Material::New(Shader::Find(String::New("Sprites/Default")));
                mat.SetColor(Color::red());
                laser.SetMaterial(mat);
            }
            g_CachedGunLasers[__this] = unsafe::GCHandle::NewWeak(laser.GetPointer(), false);
            HAX_LOG_DEBUG("Cached laser. Cache size {}", g_CachedGunLasers.size());
        }

        PhysGrabObject grabObject = __this.physGrabObject();
        if (g_UseGunLaser && grabObject && grabObject.grabbedLocal())
        {
            Transform transform = __this.gunMuzzle();
            laser.SetEnabled(true);
            laser.SetPosition(0, transform.GetPosition());
            laser.SetPosition(1, transform.GetPosition() + transform.GetForward() * __this.gunRange());
        }
        else
            laser.SetEnabled(false);

        ((void(*)(ItemGun))ItemGun::s_Update.m_Orig)(__this);
    }

    static void Hooked__RunManager_SetRunLevel(RunManager __this)
    {
        if (g_LevelBans.size() > 0)
        {
            LevelBan* levelBan = &g_LevelBans[0];
            Level prevLevel = __this.previousRunLevel();
            size_t totalAllowed = g_LevelBans.size() - LevelBan::s_TotalBans;
            while (!levelBan->m_Allowed || (levelBan->m_Level == prevLevel && totalAllowed > 1))
                levelBan = &g_LevelBans[rand() % g_LevelBans.size()];
            __this.levelCurrent() = levelBan->m_Level;
            return;
        }
        ((void(*)(RunManager))RunManager::s_SetRunLevel.m_Orig)(__this);
    }

    static void Hooked__PhotonNetwork_ExecuteRpc(void* rpcData, Photon::Player sender)
    {
        Photon::Player master = Photon::PhotonNetwork::GetMasterClient();
        ((void(*)(void*,Photon::Player))PhotonNetwork__ExecuteRpc.m_Orig)(rpcData, g_PretendMaster ? master : sender);
    }

    static void Hooked__PhotonMessageInfo_ctor(void* __this, Photon::Player player, int timestamp, void* view)
    {
        if (g_PretendMaster)
            player = Photon::PhotonNetwork::GetMasterClient();

        ((void(*)(void*, Photon::Player, int, void*))PhotonMessageInfo__ctor.m_Orig)(__this, player, timestamp, view);
    }

    static void Hooked_DataDirector__SaveDeleteCheck(DataDirector __this, bool leaveGame)
    {
        if (!g_NoSaveDelete)
            ((void(*)(DataDirector, bool))DataDirector::s_SaveDeleteCheck.m_Orig)(__this, leaveGame);
    }

    //-----------------------------------------------------------------------------
    // [SECTION] Supportive Functions
    //-----------------------------------------------------------------------------

    static void SaveConfig()
    {
        HAX_LOG_DEBUG("Saving ini file");
        std::ofstream file("haxsdk.ini", std::ios::out);
        file << std::format(
            "Key={}\n"
            "Language={}\n"
            "SpawnCol={},{},{},{}\n"
            "PointCol={},{},{},{}",
            HaxSdk::GetMenuHotkey(),
            g_Lang,
            g_SpawnColor[0], g_SpawnColor[1], g_SpawnColor[2], g_SpawnColor[3],
            g_ExtractionColor[0], g_ExtractionColor[1], g_ExtractionColor[2], g_ExtractionColor[3]
        ).c_str();
        file.close();
    }

    static void LoadConfig()
    {
        const std::filesystem::path ini{ "haxsdk.ini" };
        if (std::filesystem::exists(ini)) {
            std::ifstream file("haxsdk.ini");
            std::string line;
            if (file.is_open())
            {
                int val;
                float r, g, b, a;
                while (getline(file, line))
                {
                    if (sscanf_s(line.c_str(), "Language=%d", &val) == 1)
                    {
                        g_Lang = val;
                        HAX_LOG("[INI] Lang = {}", g_Lang);
                    }
                    if (sscanf_s(line.c_str(), "Key=%d", &val) == 1)
                    {
                        HaxSdk::SetMenuHotkey(val);
                        HAX_LOG("[INI] Key = {:X}", val);
                    }
                    if (sscanf_s(line.c_str(), "SpawnCol=%f,%f,%f,%f", &r, &g, &b, &a) == 4)
                    {
                        g_SpawnColor[0] = r;
                        g_SpawnColor[1] = g;
                        g_SpawnColor[2] = b;
                        g_SpawnColor[3] = a;
                        ImU32 hexCol = ImGui::ColorConvertFloat4ToU32(ImVec4(g_SpawnColor[0], g_SpawnColor[1], g_SpawnColor[2], g_SpawnColor[3]));
                        HAX_LOG("[INI] SpawnCol = {:X},", hexCol);
                    }
                    if (sscanf_s(line.c_str(), "PointCol=%f,%f,%f,%f", &r, &g, &b, &a) == 4)
                    {
                        g_ExtractionColor[0] = r;
                        g_ExtractionColor[1] = g;
                        g_ExtractionColor[2] = b;
                        g_ExtractionColor[3] = a;
                        ImU32 hexCol = ImGui::ColorConvertFloat4ToU32(ImVec4(g_ExtractionColor[0], g_ExtractionColor[1], g_ExtractionColor[2], g_ExtractionColor[3]));
                        HAX_LOG("[INI] PointCol = {:X},", hexCol);
                    }
                }
                file.close();
            }
        }
        else
            HAX_LOG("[INI] ini file not found");
    }

    static bool IsInGame()
    {
        RunManager runManager = RunManager::instance();
        GameDirector gameDirector = GameDirector::instance();
        return runManager && PlayerAvatar::instance() && gameDirector
            && runManager.levelCurrent() != runManager.levelLobbyMenu()
            && runManager.levelCurrent() != runManager.levelMainMenu()
            && gameDirector.currentState() == GameDirector_gameState::Main();
    }

    static Rect CalcBoundsInScreenSpace(Bounds bigBounds, Camera cam)
    {
        float screenWidth = (float)Screen::GetWidth();
        float screenHeight = (float)Screen::GetHeight();
        float ScaleX = screenWidth / (float)cam.GetPixelWidth();
        float ScaleY = screenHeight / (float)cam.GetPixelHeight();

        Vector3 screenSpaceCorners[8] =
        {
            Vector3(bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z),
            Vector3(bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z),
            Vector3(bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z),
            Vector3(bigBounds.m_Center.x + bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z),
            Vector3(bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z),
            Vector3(bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y + bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z),
            Vector3(bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z + bigBounds.m_Extents.z),
            Vector3(bigBounds.m_Center.x - bigBounds.m_Extents.x, bigBounds.m_Center.y - bigBounds.m_Extents.y, bigBounds.m_Center.z - bigBounds.m_Extents.z)
        };

        for (int i = 0; i < 8; ++i)
            screenSpaceCorners[i] = Camera::WorldToScreenPoint(screenSpaceCorners[i], g_ViewMatrix, g_ProjMatrix, screenWidth, screenHeight);

        float x = FLT_MAX;
        float y = FLT_MAX;
        float x2 = FLT_MIN;
        float y2 = FLT_MIN;

        for (const Vector3& v : screenSpaceCorners)
        {
            if (v.z > 0.f)
            {
                x = (std::min)(v.x, x);
                y = (std::min)(v.y, y);
                x2 = (std::max)(v.x, x2);
                y2 = (std::max)(v.y, y2);
            }
        }

        if (y2 - y < 15.f)
            y2 = y + 15.f;

        return Rect::MinMaxRect(x, y, x2, y2);
    }

    static void ParseEnemies(List<EnemySetup> setups)
    {
        for (EnemySetup enemySetup : setups)
        {
            for (GameObject enemyGo : enemySetup.spawnObjects())
            {
                GameObject go = Unity::Object::Instantiate<GameObject>(enemyGo, Unity::Vector3(0.f, -1000.f, 0.f), Unity::Quaternion::identity());
                EnemyParent enemyParent = go.GetComponent<EnemyParent>();
                if (enemyParent)
                {
                    std::string enemyName{ enemyParent.enemyName().UTF8() };
                    if (enemyName != "Banger" && enemyName != "Gnome" && !g_EnemiesPool.contains(enemyName))
                    {
                        g_EnemiesPool[enemyName] = enemySetup;
                        HAX_LOG_DEBUG("Added enemy {}", enemyName.c_str());
                    }
                }
                Unity::Object::Destroy(go);
            }
        }
    }

    static void ParseItems()
    {
        if (g_ItemsPool.size() > 0)
            return;

        if (StatsManager statsManager = StatsManager::instance())
        {
            Dictionary<String, Item> itemDict = statsManager.itemDictionary();
            for (auto& entry : itemDict)
            {
                const char* utf8 = entry.m_Key.UTF8();
                std::string itemName = std::string(utf8 + (strncmp(utf8, "Item ", 5) == 0 ? 5 : 0));
                if (!g_ItemsPool.contains(itemName))
                {
                    if (entry.m_Value)
                    {
                        uint32_t handle = unsafe::GCHandle::New(entry.m_Value.prefab().GetPointer(), false);
                        g_ItemsPool[itemName] = handle;
                        HAX_LOG_DEBUG("Item parsed {}", itemName.c_str());
                    }
                    else
                        HAX_LOG_ERROR("Unable to parse item {}", itemName.c_str());
                }
            }
        }
    }

    static void SpawnItem()
    {
        if (!g_ItemToSpawn)
            return;

        Camera camera = SemiFunc::MainCamera();
        if (!camera)
            return;

        GameObject item = GameObject(unsafe::GCHandle::GetTarget(g_ItemToSpawn));
        g_ItemToSpawn = 0;

        if (SemiFunc::IsMasterClient())
        {
            String name = item.GetName();
            String newName = String::Concat(String::New("Items/"), name);
            Transform transform = camera.GetTransform();
            Vector3 pos = transform.GetPosition() + transform.GetForward() * 2.f;
            Photon::PhotonNetwork::InstantiateRoomObject(newName, pos, Quaternion::identity());
        }
        if (!SemiFunc::IsMultiplayer())
        {
            Transform transform = camera.GetTransform();
            Vector3 pos = transform.GetPosition() + transform.GetForward() * 2.f;
            Unity::Object::Instantiate(item, pos, Quaternion::identity());
        }
    }

    static void ParseLevels()
    {
        if (g_LevelBans.size() > 0)
            return;

        RunManager runManager = RunManager::instance();
        if (!runManager)
            return;

        for (Level level : runManager.levels())
        {
            String levelName = level.NarrativeName();
            if (levelName && levelName.GetLength() > 0)
            {
                const char* utf8 = levelName.UTF8();
                g_LevelBans.emplace_back(level, std::string(utf8));
                HAX_LOG_DEBUG("Parsed level {}", utf8);
            }
            else
                HAX_LOG_ERROR("Unable to parse level. Name is nullptr");
        }
    }
}