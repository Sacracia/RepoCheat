#include "cheat.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <string_view>
#include <format>
#include <vector>
#include <charconv>
#include <exception>

#include "haxsdk/haxsdk_unity.h"
#include "haxsdk/haxsdk_gui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "haxsdk/third_party/imgui/imgui.h"
#include "haxsdk/third_party/imgui/imgui_internal.h"
#include "haxsdk/third_party/detours/x64/detours.h"

#include "locales.h"
#include "resource.h"
#include "fonts/Hack_Compressed.h"
#include "fonts/Rubik_Regular.ttf.h"

struct PlayerAvatar;
struct PlayerController;
struct PlayerHealth;
struct PlayerTumble;
struct EnemyDirector;
struct EnemyParent;
struct Enemy;
struct EnemyRigidbody;
struct EnemySlowMouth;
struct EnemyHealth;
struct PhysGrabObject;
struct EnemySetup;
struct LevelGenerator;
struct LevelPoint;
struct MapCustom;
struct PlayerDeathHead;
struct MapCustomEntity;
struct GameDirector;
struct NetworkManager;
struct ValuableObject;
struct ValuableDirector;
struct RoundDirector;
struct ExtractionPoint;
struct Item;
struct StatsManager;
struct PhysGrabber;
struct ItemBattery;
struct ItemGun;
struct Level;
struct RunManager;
struct PunManager;
struct MapModule;
struct Map;
struct PhysGrabObjectImpactDetector;
struct DataDirector;
struct TruckSafetySpawnPoint;
struct MenuManager;
struct PlayerVoiceChat;
struct RoomVolume;

enum SpeedType : int {
    SpeedType_Crouch,
    SpeedType_Walk,
    SpeedType_Sprint
};

enum TextShift {
    TextShift_Right = 0,
    TextShift_Left = 1,
    TextShift_Center = 2
};

enum RenderDistance : int {
    RenderDistance_Default = 0,
    RenderDistance_Medium = 32,
    RenderDistance_High = 64
};

struct EspSettings {
    bool on = false;
    bool showBorder = false;
    bool showName = false;
    bool showHp = false;
};

struct MeshMaterial {
    Unity::MeshRenderer* pMesh;
    Unity::Material* pMaterial;
};

struct LevelBan {
    static inline size_t totalBans = 0;
    Level* pLevel;
    std::string name;
    bool allowed = true;
};

struct ItemsEspSettings {
    bool on;
    bool showPrice;
    bool limitDist;
};

struct SwitchChatParams {
    bool shouldSwitch = false;
    PlayerVoiceChat* pChat;
    bool newValue = false;
};

#define COMMA ,
static const char* MAIN_ASSEMBLY = "Assembly-CSharp";
#define HAX_STATIC_FIELD(c, t, f, a, ns, cn, fn) static t* c ## __ ## f
#define HAX_FIELD(c, t, f, a, ns, cn, fn) static int c ## __ ## f;
#define HAX_METHOD(c, r, m, p, a, ns, cn, mn, s) static HaxMethod<r(__fastcall*)p> c ## __ ## m;
#include "game_data.h"

static ImFont* g_espFont;
static bool g_godmode = false;
static bool g_infStamina = false;
static float g_speedMult = 1.f;
static int g_crouchSpeedType = SpeedType_Crouch;
static int g_walkSpeedType = SpeedType_Walk;
static bool g_infJumps = false;
static bool g_neverTumble = false;
static bool g_enemiesEsp = false;
static EspSettings g_espSettings;
static std::map<std::string, EnemySetup*> g_enemiesPool;
static std::map<std::string, Item*> g_itemsPool;
static EnemySetup* g_enemyToSpawn = nullptr;
static bool g_invisible = false;
static Int32 g_damageMult = 1;
static bool g_ohk = false;
static bool g_infiniteGrab = false;
static std::unordered_map<EnemyParent*, MapCustom*> g_enemiesOnMap;
static bool g_showEnemiesOnMap = false;
static Unity::Material* g_pChamsMaterial;
static float g_validFarClipPlane;
static int g_drawDistance = RenderDistance_Default;
static Item* g_itemToSpawn = nullptr;
static bool g_highGrabRange = false;
static bool g_raycastMaxDist = false;
static bool g_infBattery = false;
static bool g_noSpread = false;
static bool g_useGunLaser = false;
static std::unordered_map<ItemGun*, Unity::LineRenderer*> g_cachedGunLasers;
static std::vector<LevelBan> g_levelBans;
static bool g_pretendMaster = false;
static PlayerAvatar* g_pPlayerToKill;
static PlayerAvatar* g_pPlayerToRevive;
static bool g_validFog = false;
static bool g_removeFog = false;
static bool g_noImpact = false;
static bool g_noSaveDelete = false;
static bool g_extractionEsp = false;
static bool g_spawnEsp = false;
static SwitchChatParams switchParams;
static bool g_playersEsp = false;
static Unity::Matrix4x4 g_projMatrix;
static Unity::Matrix4x4 g_viewMatrix;
static bool g_healToMax = true;
static std::unordered_map<PhysGrabObject*, std::vector<Unity::Collider*>> g_cachedEnemyColliders;
static HaxTexture                   g_boostyLink;
static HaxTexture                   g_discordLink;
static HaxTexture                   g_patreonLink;
static HaxTexture                   g_patreonWM;
static HaxTexture                   g_boostyWM;
static float                        g_scaleFactor = 1.f;
static ItemsEspSettings             g_itemsEspSettings;

static bool g_shouldHealToMax = false;
static bool g_shouldDespawnEnemies = false;
static bool g_shouldUpdateEnemiesOnMap = false;
static bool g_shouldRevealValuables = false;
static bool g_shouldActivateNextExtr = false;
static bool g_shouldChangeLevel = false;
static bool g_shouldRevealRooms = false;
static bool g_shouldFillQuota = false;
static bool g_shouldUnlockAllPoints = false;
static bool g_shouldSetInfMoney = false;
static bool g_shouldKillEnemies = false;
static bool g_shouldBecomeGhost = false;
static bool g_shouldTeleportItems;

static std::pair<const char*, int>  g_hotkeys[3] = {
    std::make_pair("~", VK_OEM_3),
    std::make_pair("F1", VK_F1),
    std::make_pair("Insert", VK_INSERT)
};

static void AddText(const char* text, const ImVec2& pos, ImU32 col, float fontSize, TextShift shift);
static void AddEnemyOnMap(EnemyParent* pEnemyParent);
static Unity::Rect CalcBoundsInScreenSpace(Unity::Bounds bigBounds, Unity::Camera* mainCamera);
static bool DisabledButton(const char* label, bool condition);
static bool DisabledCheckbox(const char* label, bool* pFlag, bool condition);
static void ParseEnemies(System::List<EnemySetup*>* enemies);
static void ParseItems();
static void ParseLevels();
static bool RemoveEnemyFromMap(EnemyParent* pEnemyParent);
static void ShowTooltip(const char* tip);
static void ShowHelpMarker(const char* tip);
static void ShowWarningMarker(const char* tip);
static Unity::Vector3 WorldToScreen(Unity::Vector3& worldPos, Unity::Matrix4x4& viewMatrix, Unity::Matrix4x4& projMatrix, float screenWidth, float screenHeight);
static bool                         PremiumButton(const char* label, bool enabled, HaxTexture& watermark);
static bool                         PremiumCheckbox(const char* label, bool* pFlag, bool condition, HaxTexture& watermark);

static void Hooked_PlayerHealth__Update(PlayerHealth* __this);
static void Hooked_PlayerHealth__Hurt(PlayerHealth* __this, Int32 dmg, bool savingGrace, Int32 enemyIndex);
static void Hooked_PlayerController__FixedUpdate(PlayerController* __this);
static void Hooked_PlayerController__Update(PlayerController* __this);
static void Hooked_PlayerTumble__TumbleRequest(PlayerTumble* __this, bool _isTumbling, bool _playerInput);
static void Hooked_EnemyDirector__Update(EnemyDirector* __this);
static void Hooked_EnemyHealth__Hurt(EnemyHealth* __this, Int32 _damage, void* _hurtDirection);
static void Hooked_EnemyRigidbody__FixedUpdate(EnemyRigidbody* __this);
static void Hooked_EnemyParent__Update(EnemyParent* __this);
static void Hooked_PlayerAvatar__OnDestroy(PlayerAvatar* __this);
static void Hooked_GameDirector__Update(GameDirector* __this);
static void Hooked_PhysGrabber__PhysGrabLogic(PhysGrabber* __this);
static void Hooked_PhysGrabber__RayCheck(PhysGrabber* __this, bool _grab);
static bool Hooked_Physics__Raycast(void* v1, void* v2, void* v3, float maxDistance, Int32 v5, Int32 v6);
static void Hooked_ItemBattery__Update(ItemBattery* __this);
static void Hooked_ItemGun__Shoot(ItemGun* __this);
static void Hooked_ItemGun__Update(ItemGun* __this);
static void Hooked_RunManager__SetRunLevel(RunManager* __this);
static bool Hooked_PhotonNetwork__IsMasterClient();
static void Hooked_RenderSettings__set_fog(bool value);
static void Hooked_PhysGrabObjectImpactDetector__FixedUpdate(PhysGrabObjectImpactDetector* __this);
static void Hooked_DataDirector__SaveDeleteCheck(DataDirector* __this, bool _leaveGame);
static void Hooked_PlayerAvatar__SpawnRPC(PlayerAvatar* __this, void* position, void* rotation);
static void Hooked_Camera__FireOnPreRender(Unity::Camera* pCam);
static void Hooked_PlayerAvatar__PlayerDeathRPC(PlayerAvatar* __this, Int32 enemyIndex);

static Int32 STATE_IDLE = -1;
static Int32 NORMAL = -1;
static Int32 MAIN = -1;
static Int32 STATE_ACTIVE = -1;
static Int32 STATE_COMPLETE = -1;

#define FIELD(c, t, f) t& f() { return *(t*)((char*)this + c ## __ ## f); }
#define STATIC_FIELD(c, t, f) static t& f() { return *c ## __ ## f; }
#define TYPE(a, n, c) static System::Type* GetType() { static System::Type* pType = Class::Find(a, n, c)->GetSystemType(); return pType; }

struct PlayerAvatar : Unity::Component {
    STATIC_FIELD(PlayerAvatar, PlayerAvatar*, instance);

    FIELD(PlayerAvatar, PlayerHealth*, playerHealth);
    FIELD(PlayerAvatar, bool, isLocal);
    FIELD(PlayerAvatar, bool, deadSet);
    FIELD(PlayerAvatar, PlayerDeathHead*, playerDeathHead);
    FIELD(PlayerAvatar, PhysGrabber*, physGrabber);
    FIELD(PlayerAvatar, System::String*, playerName);
    FIELD(PlayerAvatar, PlayerVoiceChat*, voiceChat);
    FIELD(PlayerAvatar, bool, spawned);
};

struct PlayerController {
    FIELD(PlayerController, float, EnergyCurrent);
    FIELD(PlayerController, float, EnergyStart);
    FIELD(PlayerController, float, MoveSpeed);
    FIELD(PlayerController, float, SprintSpeed);
    FIELD(PlayerController, float, CrouchSpeed);
    FIELD(PlayerController, Int32, JumpExtra);
};

struct PlayerHealth {
    FIELD(PlayerHealth, PlayerAvatar*, playerAvatar);
};

struct EnemyDirector {
    STATIC_FIELD(EnemyDirector, EnemyDirector*, instance);

    FIELD(EnemyDirector, System::List<EnemyParent*>*, enemiesSpawned);
    FIELD(EnemyDirector, System::List<EnemySetup*>*, enemiesDifficulty1);
    FIELD(EnemyDirector, System::List<EnemySetup*>*, enemiesDifficulty2);
    FIELD(EnemyDirector, System::List<EnemySetup*>*, enemiesDifficulty3);
    FIELD(EnemyDirector, bool, debugNoVision);
    FIELD(EnemyDirector, bool, debugNoGrabMaxTime);
    FIELD(EnemyDirector, bool, debugEasyGrab);
};

struct EnemyParent : Unity::Component {
    TYPE(MAIN_ASSEMBLY, "", "EnemyParent");

    FIELD(EnemyParent, bool, Spawned);
    FIELD(EnemyParent, Enemy*, enemy);
    FIELD(EnemyParent, System::String*, enemyName);
};

struct Enemy : Unity::Component {
    FIELD(Enemy, EnemyRigidbody*, Rigidbody);
    FIELD(Enemy, Unity::Transform*, CenterTransform);
    FIELD(Enemy, EnemyHealth*, Health);
    FIELD(Enemy, Int32, CurrentState);
};

struct EnemyRigidbody {
    FIELD(EnemyRigidbody, PhysGrabObject*, physGrabObject);
    FIELD(EnemyRigidbody, float, grabShakeReleaseTimer);
    FIELD(EnemyRigidbody, float, grabStrengthTimer);
    FIELD(EnemyRigidbody, float, grabStrengthTime);
};

struct EnemySlowMouth {
    TYPE("Assembly-CSharp", "", "EnemySlowMouth");

    FIELD(EnemySlowMouth, float, attachedTimer);
};

struct EnemyHealth {
    FIELD(EnemyHealth, Int32, healthCurrent);
    FIELD(EnemyHealth, Int32, health);
    FIELD(EnemyHealth, bool, dead);
};

struct EnemySetup {
    FIELD(EnemySetup, System::List<Unity::GameObject*>*, spawnObjects);
};

struct LevelGenerator {
    STATIC_FIELD(LevelGenerator, LevelGenerator*, Instance);

    FIELD(LevelGenerator, System::List<LevelPoint*>*, LevelPathPoints);
    FIELD(LevelGenerator, bool, Generated);
};

struct LevelPoint : Unity::Component {
    FIELD(LevelPoint, RoomVolume*, Room);
};

struct PhysGrabObject : Unity::Component {
    FIELD(PhysGrabObject, bool, grabbedLocal);
    FIELD(PhysGrabObject, Unity::Vector3, centerPoint);
};

struct GameDirector {
    STATIC_FIELD(GameDirector, GameDirector*, instance);

    FIELD(GameDirector, Unity::Camera*, MainCamera);
    FIELD(GameDirector, System::List<PlayerAvatar*>*, PlayerList);
    FIELD(GameDirector, Int32, currentState);
};

struct MapCustom : Unity::Component {
    TYPE(MAIN_ASSEMBLY, "", "MapCustom");

    FIELD(MapCustom, Unity::Sprite*, sprite);
    FIELD(MapCustom, Unity::Color, color);
    FIELD(MapCustom, MapCustomEntity*, mapCustomEntity);
};

struct PlayerDeathHead : Unity::Component {
    FIELD(PlayerDeathHead, MapCustom*, mapCustom);
};

struct MapCustomEntity : Unity::Component {
    STATIC_FIELD(EnemyDirector, EnemyDirector*, instance);
};

struct ValuableObject : Unity::Component {
    FIELD(ValuableObject, PhysGrabObject*, physGrabObject);
    FIELD(ValuableObject, float, dollarValueCurrent);
};

struct ValuableDirector {
    STATIC_FIELD(ValuableDirector, ValuableDirector*, instance);

    FIELD(ValuableDirector, System::List<ValuableObject*>*, valuableList);
};

struct RoundDirector {
    STATIC_FIELD(RoundDirector, RoundDirector*, instance);

    FIELD(RoundDirector, bool, extractionPointActive);
    FIELD(RoundDirector, ExtractionPoint*, extractionPointCurrent);
    FIELD(RoundDirector, System::List<Unity::GameObject*>*, extractionPointList);
};

struct ExtractionPoint {
    TYPE(MAIN_ASSEMBLY, "", "ExtractionPoint");

    FIELD(ExtractionPoint, bool, isLocked);
    FIELD(ExtractionPoint, Int32, currentState);
};

struct StatsManager {
    STATIC_FIELD(StatsManager, StatsManager*, instance);

    FIELD(StatsManager, System::Dictionary<System::String* COMMA Item*>*, itemDictionary);
    FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeStrength);
    FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeThrow);
    FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeStamina);
    FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeSpeed);
    FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeLaunch);
    FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeHealth);
    FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeMapPlayerCount);
    FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeExtraJump);
};

struct Item : Unity::Component {
    FIELD(Item, Unity::GameObject*, prefab);
};

struct PhysGrabber {
    FIELD(PhysGrabber, float, grabRange);
};

struct ItemBattery {
    FIELD(ItemBattery, float, batteryLife);
};

struct ItemGun : Unity::Component {
    FIELD(ItemGun, float, gunRandomSpread);
    FIELD(ItemGun, PhysGrabObject*, physGrabObject);
    FIELD(ItemGun, Unity::Transform*, gunMuzzle);
    FIELD(ItemGun, float, gunRange);
};

struct Level {
    FIELD(Level, System::String*, NarrativeName);
};

struct RunManager {
    STATIC_FIELD(RunManager, RunManager*, instance);

    FIELD(RunManager, System::List<Level*>*, levels);
    FIELD(RunManager, Level*, levelCurrent);
    FIELD(RunManager, Level*, levelMainMenu);
    FIELD(RunManager, Level*, levelLobbyMenu);
    FIELD(RunManager, Level*, levelShop);
    FIELD(RunManager, Level*, previousRunLevel);
    FIELD(RunManager, Int32, levelsCompleted);
};

struct PunManager {
    STATIC_FIELD(PunManager, PunManager*, instance);

    FIELD(PunManager, StatsManager*, statsManager);
};

struct MapModule : Unity::Component {

};

struct Map {
    STATIC_FIELD(Map, Map*, Instance);

    FIELD(Map, System::List<MapModule*>*, MapModules);
};

struct PhysGrabObjectImpactDetector {
    FIELD(PhysGrabObjectImpactDetector, bool, isValuable);
    FIELD(PhysGrabObjectImpactDetector, float, impactDisabledTimer);
};

struct TruckSafetySpawnPoint : Unity::Component {
    STATIC_FIELD(TruckSafetySpawnPoint, TruckSafetySpawnPoint*, instance);
};

struct MenuManager {
    STATIC_FIELD(MenuManager, MenuManager*, instance);

    FIELD(MenuManager, Unity::Component*, currentMenuPage);
};

struct PlayerVoiceChat {
    FIELD(PlayerVoiceChat, bool, inLobbyMixer);
};

void HaxSdk::DoOnceBeforeRendering() {
    HaxSdk::GetGlobals().Load();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.IniSavingRate = 10.f;

    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    g_scaleFactor = std::round((float)desktop.bottom / 1080.f);

    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    builder.BuildRanges(&ranges);

    ImFontConfig cfg;
    cfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(hack_compressed_compressed_data_base85, std::roundf(13.f * g_scaleFactor), &cfg, io.Fonts->GetGlyphRangesCyrillic());
    g_espFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(Rubik_Regular_ttf, sizeof(Rubik_Regular_ttf), 28.0f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    auto& style = ImGui::GetStyle();
    style.ScaleAllSizes(g_scaleFactor);

    g_boostyLink = HaxSdk::LoadTextureFromResource(IDB_PNG1);
    g_patreonLink = HaxSdk::LoadTextureFromResource(IDB_PNG4);
    g_discordLink = HaxSdk::LoadTextureFromResource(IDB_PNG3);
    g_boostyWM = HaxSdk::LoadTextureFromResource(IDB_PNG2);
    g_patreonWM = HaxSdk::LoadTextureFromResource(IDB_PNG5);
}

static bool InGame() {
    RunManager* pRunManager = RunManager::instance();
    GameDirector* pGameDirector = GameDirector::instance();
    bool inGame = pRunManager && pRunManager->levelCurrent() != pRunManager->levelLobbyMenu()
        && pRunManager->levelCurrent() != pRunManager->levelMainMenu() && pGameDirector->currentState() == MAIN;
    return inGame;
}

void HaxSdk::RenderMenu() {
    int& lang = HaxSdk::GetGlobals().locale;
    HaxTexture& wm = lang == 1 ? g_boostyWM : g_patreonWM;

    EnemyDirector* pEnemyDirector = EnemyDirector::instance();
    RoundDirector* pRoundDirector = RoundDirector::instance();
    RunManager* pRunManager = RunManager::instance();
    PunManager* pPunManager = PunManager::instance();
    GameDirector* pGameDirector = GameDirector::instance();
    PlayerAvatar* pPlayerAvatar = PlayerAvatar::instance();
    bool isHost = SemiFunc__IsMasterClientOrSingleplayer.ptr();
    bool isMultiplayer = SemiFunc__IsMultiplayer.ptr();
    bool inGame = InGame();

    ImGui::Begin("R.E.P.O. Cheat Menu (by Sacracia)", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
    if (ImGui::BeginTable("main", 4, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableNextColumn();
        {
            // Settings
            ImGui::SeparatorText(Locales::SETTINGS[lang]);
            int curHotkey = 0;
            int& key = HaxSdk::GetGlobals().hotkey;
            for (; curHotkey < 3 && g_hotkeys[curHotkey].second != key; ++curHotkey) {}
            if (ImGui::BeginCombo(Locales::HOTKEY[lang], g_hotkeys[curHotkey].first)) {
                for (int i = 0; i < 3; ++i) {
                    bool isSelected = (curHotkey == i);
                    if (ImGui::Selectable(g_hotkeys[i].first, &isSelected)) {
                        key = g_hotkeys[i].second;
                        HaxSdk::GetGlobals().Save();
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ShowHelpMarker(Locales::SHOWHIDE[lang]);

            if (ImGui::BeginTable("table1", 2, ImGuiTableFlags_SizingFixedSame)) {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text(Locales::LANGUAGE[lang]);
                ImGui::SameLine();
                if (ImGui::RadioButton("ENG", &lang, 0))
                    HaxSdk::GetGlobals().Save();
                ImGui::TableNextColumn();
                if (ImGui::RadioButton(RUS("РУ"), &lang, 1))
                    HaxSdk::GetGlobals().Save();
                ImGui::EndTable();
            }

            // Health
            ImGui::SeparatorText(Locales::HEALTH[lang]);
            ImGui::Checkbox(Locales::GODMODE[lang], &g_godmode);
            if (g_godmode) {
                ImGui::Indent(); 
                ImGui::Checkbox(Locales::HEAL_TO_MAX[lang], &g_healToMax); 
                ImGui::Unindent();
            }
            if (DisabledButton(Locales::HEAL_MAX[lang], inGame && pPlayerAvatar && !pPlayerAvatar->deadSet())) { g_shouldHealToMax = true; }

            // Movement
            ImGui::SeparatorText(Locales::MOVEMENT[lang]);
            ImGui::Checkbox(Locales::INF_STAMINA[lang], &g_infStamina);
            ImGui::Checkbox(Locales::INF_JUMPS[lang], &g_infJumps);
            ImGui::Checkbox(Locales::NEVER_TUMBLE[lang], &g_neverTumble);
            ImGui::SliderFloat(Locales::SPEED_MULT[lang], &g_speedMult, 1.f, 5.f, "%.1f");
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text(Locales::CROUCH_SPEED[lang]);
                ImGui::SameLine(); ImGui::RadioButton(Locales::DEFAULT_CROUCH[lang], &g_crouchSpeedType, SpeedType_Crouch);
                ImGui::SameLine(); ImGui::RadioButton(Locales::WALK_CROUCH[lang], &g_crouchSpeedType, SpeedType_Walk);
                ImGui::SameLine(); ImGui::RadioButton(Locales::SPRINT_CROUCH[lang], &g_crouchSpeedType, SpeedType_Sprint);

                ImGui::AlignTextToFramePadding();
                ImGui::Text(Locales::WALK_SPEED[lang]);
                ImGui::SameLine(); ImGui::RadioButton(Locales::DEFAULT_WALK[lang], &g_walkSpeedType, SpeedType_Walk);
                ImGui::SameLine(); ImGui::RadioButton(Locales::SPRINT_WALK[lang], &g_walkSpeedType, SpeedType_Sprint);
            }

            // Enemies
            ImGui::SeparatorText(Locales::ENEMIES[lang]);
#ifndef NEXUS
            PremiumCheckbox(Locales::ESP_ENEMIES[lang], &g_espSettings.on, true, wm);
            if (g_espSettings.on) {
                ImGui::Indent();
                ImGui::Checkbox(Locales::SHOW_BORDERS[lang], &g_espSettings.showBorder);
                ImGui::Checkbox(Locales::SHOW_NAME[lang], &g_espSettings.showName);
                ImGui::Checkbox(Locales::SHOW_HEALTH[lang], &g_espSettings.showHp);
                ImGui::Unindent();
            }
#endif
            if (pEnemyDirector) {
                DisabledCheckbox(Locales::BLIND[lang], &pEnemyDirector->debugNoVision(), isHost);
                ShowWarningMarker(Locales::HOST_ONLY_2[lang]);
            }
            ImGui::Checkbox(Locales::OHK[lang], &g_ohk);
            ImGui::SetNextItemWidth(200.f);
            ImGui::SliderInt(Locales::DAMAGE_MULT[lang], &g_damageMult, 1, 5);
            static int curEnemy = 0;
            ImGui::Combo(Locales::ENEMY_SPAWN[lang], &curEnemy, [](void* data, int idx, const char** outText) {
                auto pMap = static_cast<std::map<std::string, EnemySetup*>*>(data);
                auto it = pMap->begin();
                std::advance(it, idx);
                *outText = it->first.c_str();
                return true; }, (void*)&g_enemiesPool, static_cast<int>(g_enemiesPool.size()), -1);
            if (DisabledButton(Locales::SPAWN_ENEMY[lang], inGame && isHost)) {
                auto it = g_enemiesPool.begin();
                std::advance(it, curEnemy);
                g_enemyToSpawn = it->second;
            };
            ShowWarningMarker(Locales::HOST_ONLY[lang]);

            if (DisabledButton(Locales::KILL_ALL[lang], inGame)) {
                g_shouldKillEnemies = true;
            }

            // Map
            ImGui::SeparatorText(Locales::MAP[lang]);
            ImGui::Checkbox(Locales::SHOW_ENEMIES[lang], &g_showEnemiesOnMap);
            if (DisabledButton(Locales::REVEAL_ROOMS[lang], inGame)) g_shouldRevealRooms = true;

            // Items
            ImGui::SeparatorText(Locales::EXTRACTION_POINTS[lang]);
            ImGui::Checkbox(Locales::SHOW_POINTS[lang], &g_extractionEsp);
            ImGui::SameLine(), ImGui::ColorEdit4("Color##1", HaxSdk::GetGlobals().extractionColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            if (DisabledButton(Locales::ACTIVATE_NEXT[lang], inGame && pRoundDirector && !pRoundDirector->extractionPointActive())) g_shouldActivateNextExtr = true;
#ifndef NEXUS
            if (PremiumButton(Locales::FILL_QUOTA[lang], inGame && pRoundDirector && pRoundDirector->extractionPointActive(), wm)) g_shouldFillQuota = true;
#endif
            if (DisabledButton(Locales::UNLOCK_POINTS[lang], inGame)) g_shouldUnlockAllPoints = true;

            // Valuables
            ImGui::SeparatorText(Locales::VALUABLES[lang]);
            DisabledCheckbox(Locales::NO_IMPACT[lang], &g_noImpact, isHost); ShowWarningMarker(Locales::HOST_ONLY_2[lang]);
            ImGui::Checkbox(Locales::ITEMS_ESP[lang], &g_itemsEspSettings.on);
            if (g_itemsEspSettings.on) {
                ImGui::Indent();
                ImGui::Checkbox(Locales::SHOW_PRICE[lang], &g_itemsEspSettings.showPrice);
                ImGui::Checkbox(Locales::LIMIT_DIST[lang], &g_itemsEspSettings.limitDist);
                ImGui::Unindent();
            }
            if (DisabledButton(Locales::REVEAL_ITEMS[lang], inGame)) { g_shouldRevealValuables = true; }
            if (ImGui::Button("Teleport valuables")) g_shouldTeleportItems = true;
        }

        ImGui::TableNextColumn();
        {   
            // Grab
            ImGui::SeparatorText(Locales::GRAB[lang]);
            DisabledCheckbox(Locales::GRAB_PERMANENT[lang], &g_infiniteGrab, isHost); ShowWarningMarker(Locales::HOST_ONLY_2[lang]);
            ImGui::Checkbox(Locales::GRAB_RANGE[lang], &g_highGrabRange);

            // Loot
            ImGui::SeparatorText(Locales::ITEMS[lang]);
            static int curItem = 0;
            ImGui::Combo(Locales::ITEM_SPAWN[lang], &curItem, [](void* data, int idx, const char** outText) {
                auto pMap = static_cast<std::map<std::string, Item*>*>(data);
                auto it = pMap->begin();
                std::advance(it, idx);
                *outText = it->first.c_str();
                return true; 
            }, (void*)&g_itemsPool, static_cast<int>(g_itemsPool.size()), -1);
            if (DisabledButton(Locales::SPAWN_ITEM[lang], inGame && isHost)) {
                auto it = g_itemsPool.begin();
                std::advance(it, curItem);
                g_itemToSpawn = it->second;
            };
            ShowWarningMarker(Locales::HOST_ONLY[lang]);

            DisabledCheckbox(Locales::MAX_BATTERY[lang], &g_infBattery, isHost); ShowWarningMarker(Locales::HOST_ONLY_2[lang]);
            DisabledCheckbox(Locales::NO_SPREAD[lang], &g_noSpread, isHost); ShowWarningMarker(Locales::HOST_ONLY_2[lang]);
            ImGui::Checkbox(Locales::LASER_CROSSHAIR[lang], &g_useGunLaser);

            ImGui::SeparatorText(Locales::LEVELS[lang]);
            if (DisabledButton(Locales::NEXT_LEVEL[lang], inGame && isHost)) g_shouldChangeLevel = true;
            ShowWarningMarker(Locales::HOST_ONLY[lang]);

            ImGui::Text(Locales::ALLOWED_LEVELS[lang]);
            ShowWarningMarker(Locales::HOST_ONLY[lang]);
            for (LevelBan& levelBan : g_levelBans) {
                if (ImGui::Selectable(levelBan.name.c_str(), levelBan.allowed)) {
                    if (levelBan.allowed && LevelBan::totalBans + 1 < g_levelBans.size()) {
                        levelBan.allowed = false;
                        ++LevelBan::totalBans;
                    }
                    else if (!levelBan.allowed) {
                        levelBan.allowed = true;
                        --LevelBan::totalBans;
                    }
                }
            }

            ImGui::SetNextItemWidth(50.f);
            static char lvlBuff[4] = ""; ImGui::InputText(Locales::LEVELS_COMPLETED[lang], lvlBuff, sizeof(lvlBuff), ImGuiInputTextFlags_CharsDecimal);
            static bool showError = false;
            ImGui::SameLine();
            if (DisabledButton(Locales::APPLY[lang], inGame && isHost)) {
                unsigned int res;
                auto [ptr, ec] = std::from_chars(lvlBuff, lvlBuff + sizeof(lvlBuff), res);
                showError = !(ec == std::errc() && ptr == lvlBuff + strlen(lvlBuff));
                if (!showError) {
                    RunManager::instance()->levelsCompleted() = res;
                }
            }
            ShowWarningMarker(Locales::HOST_ONLY[lang]);
            if (showError) { ImGui::SameLine(); ImGui::Text(Locales::INVALID_INPUT[lang]); }

            ImGui::SeparatorText(Locales::UPGRADES[lang]);
            if (pPlayerAvatar && pPunManager) {
                StatsManager* pStats = pPunManager->statsManager();
                System::String* steamId = SemiFunc__PlayerGetSteamID.ptr(pPlayerAvatar);

                if (ImGui::BeginTable("Stats_Upgrades", 2, ImGuiTableFlags_SizingStretchProp)) {
                    g_pretendMaster = true;
                    char buff[64];

                    if (pStats->playerUpgradeStrength()->ContainsKey(steamId)) {
                        ImGui::TableNextRow(); ImGui::TableNextColumn(); memset(buff, 0, sizeof(buff)); sprintf_s(buff, Locales::STRENGTH[lang], pStats->playerUpgradeStrength()->GetItem(steamId)); ImGui::Text(buff);
                        ImGui::TableNextColumn(); if (DisabledButton("+##strength", inGame)) PunManager__UpgradePlayerGrabStrength.ptr(pPunManager, steamId);
                    }

                    if (pStats->playerUpgradeThrow()->ContainsKey(steamId)) {
                        ImGui::TableNextRow(); ImGui::TableNextColumn(); memset(buff, 0, sizeof(buff)); sprintf_s(buff, Locales::THROW[lang], pStats->playerUpgradeThrow()->GetItem(steamId)); ImGui::Text(buff);
                        ImGui::TableNextColumn(); if (DisabledButton("+##throw", inGame)) PunManager__UpgradePlayerThrowStrength.ptr(pPunManager, steamId);
                    }

                    if (pStats->playerUpgradeStamina()->ContainsKey(steamId)) {
                        ImGui::TableNextRow(); ImGui::TableNextColumn(); memset(buff, 0, sizeof(buff)); sprintf_s(buff, Locales::STAMINA[lang], pStats->playerUpgradeStamina()->GetItem(steamId)); ImGui::Text(buff);
                        ImGui::TableNextColumn(); if (DisabledButton("+##stamina", inGame)) PunManager__UpgradePlayerEnergy.ptr(pPunManager, steamId);
                    }

                    if (pStats->playerUpgradeSpeed()->ContainsKey(steamId)) {
                        ImGui::TableNextRow(); ImGui::TableNextColumn(); memset(buff, 0, sizeof(buff)); sprintf_s(buff, Locales::SPEED[lang], pStats->playerUpgradeSpeed()->GetItem(steamId)); ImGui::Text(buff);
                        ImGui::TableNextColumn(); if (DisabledButton("+##speed", inGame)) PunManager__UpgradePlayerSprintSpeed.ptr(pPunManager, steamId);
                    }

                    if (pStats->playerUpgradeLaunch()->ContainsKey(steamId)) {
                        ImGui::TableNextRow(); ImGui::TableNextColumn(); memset(buff, 0, sizeof(buff)); sprintf_s(buff, Locales::TUMBLE_LAUNCH[lang], pStats->playerUpgradeLaunch()->GetItem(steamId)); ImGui::Text(buff);
                        ImGui::TableNextColumn(); if (DisabledButton("+##tumble", inGame)) PunManager__UpgradePlayerTumbleLaunch.ptr(pPunManager, steamId);
                    }

                    if (pStats->playerUpgradeHealth()->ContainsKey(steamId)) {
                        ImGui::TableNextRow(); ImGui::TableNextColumn(); memset(buff, 0, sizeof(buff)); sprintf_s(buff, Locales::HEALTH_STAT[lang], pStats->playerUpgradeHealth()->GetItem(steamId)); ImGui::Text(buff);
                        ImGui::TableNextColumn(); if (DisabledButton("+##health", inGame)) PunManager__UpgradePlayerHealth.ptr(pPunManager, steamId);
                    }

                    if (pStats->playerUpgradeMapPlayerCount()->ContainsKey(steamId)) {
                        ImGui::TableNextRow(); ImGui::TableNextColumn(); memset(buff, 0, sizeof(buff)); sprintf_s(buff, Locales::PLAYERS_COUNT[lang], pStats->playerUpgradeMapPlayerCount()->GetItem(steamId)); ImGui::Text(buff);
                        ImGui::TableNextColumn(); if (DisabledButton("+##players", inGame)) PunManager__UpgradeMapPlayerCount.ptr(pPunManager, steamId);
                    }

                    if (pStats->playerUpgradeExtraJump()->ContainsKey(steamId)) {
                        ImGui::TableNextRow(); ImGui::TableNextColumn(); memset(buff, 0, sizeof(buff)); sprintf_s(buff, Locales::EXTRA_JUMPS[lang], pStats->playerUpgradeExtraJump()->GetItem(steamId)); ImGui::Text(buff);
                        ImGui::TableNextColumn(); if (DisabledButton("+##jump", inGame)) PunManager__UpgradePlayerExtraJump.ptr(pPunManager, steamId);
                    }
                    g_pretendMaster = false;
                    ImGui::EndTable();
                }
            }

            // Players
            ImGui::SeparatorText(Locales::PLAYERS[lang]);
            static int playerIndex = 0;
            auto players = *GameDirector::instance()->PlayerList();
            if (playerIndex >= players.length || playerIndex < 0) {
                playerIndex = players.length - 1;
            }

            auto pPlayer = playerIndex < 0 ? nullptr : players[playerIndex];
            auto name = pPlayer->playerName();
            if (!pPlayer->playerName())
                pPlayer = nullptr;

            if (ImGui::BeginCombo(Locales::PLAYER[lang], pPlayer ? pPlayer->playerName()->UTF8() : "", ImGuiComboFlags_WidthFitPreview)) {
                for (int n = 0; n < players.length; n++) {
                    const bool is_selected = (playerIndex == n);
                    if (ImGui::Selectable(players[n]->playerName()->UTF8(), is_selected))
                        playerIndex = n;

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if (auto pChat = PlayerAvatar::instance()->voiceChat()) {
                bool isSpectator = pChat->inLobbyMixer();
                ImGui::AlignTextToFramePadding();
                ImGui::Text(Locales::VOICE_CHAT[lang], isSpectator ? Locales::SPECTATOR[lang] : Locales::ALIVE[lang]);
                ImGui::SameLine();
                if (DisabledButton(Locales::SWITCH[lang], inGame))
                    switchParams = SwitchChatParams(true, pChat, !isSpectator);
            }

#ifndef NEXUS
            if (!pPlayer || !pPlayer->isLocal()) {
                if (PremiumButton(Locales::KILL[lang], inGame && pPlayer && !pPlayer->deadSet(), wm)) {
                    g_pPlayerToKill = pPlayer;
                }
                ImGui::SameLine();
            }
#endif
            if (pPlayer && pPlayer->isLocal()) {
                if (DisabledButton(Locales::KILL[lang], inGame && !pPlayer->deadSet())) {
                    g_pPlayerToKill = pPlayer;
                }
                ImGui::SameLine();
            }
            if (DisabledButton(Locales::REVIVE[lang], inGame && pPlayer && pPlayer->deadSet())) {
                g_pPlayerToRevive = pPlayer;
            }

            ImGui::Checkbox(Locales::SHOW_PLAYERS[lang], &g_playersEsp);
            if (DisabledButton(Locales::BECOME_GHOST[lang], inGame && isMultiplayer && pPlayer && !pPlayer->deadSet()))
                g_shouldBecomeGhost = true;
            ShowHelpMarker(Locales::GHOST_HINT[lang]);

            ImGui::SeparatorText(Locales::MISC[lang]);
            bool fog = !RenderSettings__get_fog.ptr();
            if (ImGui::Checkbox(Locales::IMPROVE_VISION[lang], &fog))
                RenderSettings__set_fog.ptr(!fog);

            if (DisabledButton(Locales::INF_MONEY[lang], inGame && pRunManager->levelCurrent() == pRunManager->levelShop())) g_shouldSetInfMoney = true;
            ImGui::Checkbox(Locales::DELETE_SAVE[lang], &g_noSaveDelete); ShowWarningMarker(Locales::HOST_ONLY[lang]);
            ImGui::Checkbox(Locales::SHOW_SPAWN[lang], &g_spawnEsp); ImGui::SameLine(); 
            ImGui::ColorEdit4("Color##2", HaxSdk::GetGlobals().spawnColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        
#ifndef NEXUS
            // Links
            ImGui::SeparatorText(Locales::LINKS[lang]);
            HaxTexture img = lang == 1 ? g_boostyLink : g_patreonLink;
            if (ImGui::ImageButton(img.m_pTexture, ImVec2(img.m_width, img.m_height) * g_scaleFactor, ImVec2(0, 0), ImVec2(1, 1), 0))
                ShellExecute(0, 0, lang == 1 ? "https://boosty.to/sacraciamods" : "https://www.patreon.com/sacracia", 0, 0, SW_SHOW);
            if (ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
#endif
        }

        ImGui::EndTable();
    }
    ImGui::End();
}

void HaxSdk::RenderBackground() {
    static bool g_skipRendering = false;
    static std::chrono::steady_clock::time_point g_lastErrorTime;

    if (g_skipRendering) {
        auto now = std::chrono::steady_clock::now();
        if (now - g_lastErrorTime < std::chrono::seconds(1)) {
            return;
        }
        g_skipRendering = false;
    }

    try {
        if (ImGui::GetIO().WantSaveIniSettings) {
            HaxSdk::GetGlobals().Save();
            ImGui::MarkIniSettingsDirty();
        }

        GameDirector* pDirector = GameDirector::instance();
        MenuManager* pMenu = MenuManager::instance();
        if (!pMenu || (pMenu->currentMenuPage() && pMenu->currentMenuPage()->m_CachedPtr) || !InGame())
            return;

        Unity::Camera* pCamera = SemiFunc__MainCamera.ptr();
        if (!pCamera || !pCamera->m_CachedPtr)
            return;

        float screenWidth = (float)Unity::Screen::GetWidth();
        float screenHeight = (float)Unity::Screen::GetHeight();

        EnemyDirector* pEnemyDirector = *EnemyDirector__instance;

        float* pSpawnColor = HaxSdk::GetGlobals().spawnColor;
        float* pExtrColor = HaxSdk::GetGlobals().extractionColor;

        if (g_spawnEsp) {
            auto pTrack = TruckSafetySpawnPoint::instance();
            if (pTrack && pTrack->GetGameObject() && pTrack->GetGameObject()->m_CachedPtr) {
                Unity::Vector3 worldPos = pTrack->GetTransform()->GetPosition();
                Unity::Vector3 screenPos = WorldToScreen(worldPos, g_viewMatrix, g_projMatrix, screenWidth, screenHeight);
                if (screenPos.z > 0.f) {
                    AddText(Locales::SPAWN[HaxSdk::GetGlobals().locale], ImVec2(screenPos.x, screenPos.y), ImGui::ColorConvertFloat4ToU32(ImVec4(pSpawnColor[0], pSpawnColor[1], pSpawnColor[2], pSpawnColor[3])), 13.f, TextShift_Center);
                }
            }
        }

        if (g_extractionEsp && *RoundDirector__instance) {
            for (auto pGo : *RoundDirector::instance()->extractionPointList()) {
                if (!pGo || !pGo->m_CachedPtr)
                    continue;
                ExtractionPoint* pPoint = (ExtractionPoint*)pGo->GetComponent(ExtractionPoint::GetType());
                if (!pPoint)
                    continue;

                Unity::Vector3 worldPos = pGo->GetTransform()->GetPosition();
                Unity::Vector3 screenPos = WorldToScreen(worldPos, g_viewMatrix, g_projMatrix, screenWidth, screenHeight);
                if (screenPos.z > 0.f) {
                    if (pPoint->currentState() != STATE_COMPLETE) {
                        ImU32 color = pPoint->currentState() == STATE_ACTIVE ? ImGui::ColorConvertFloat4ToU32(ImVec4(pExtrColor[0], pExtrColor[1], pExtrColor[2], pExtrColor[3])) : 0xFF3D3D3D;
                        AddText(Locales::EXTRACTION[HaxSdk::GetGlobals().locale], ImVec2(screenPos.x, screenPos.y), color, 13.f, TextShift_Center);
                    }
                }
            }
        }

        if (g_playersEsp) {
            for (PlayerAvatar* pAvatar : *GameDirector::instance()->PlayerList())
            {
                if (pAvatar && pAvatar->m_CachedPtr && pAvatar != PlayerAvatar::instance())
                {
                    if (!pAvatar->deadSet())
                    {
                        auto* pTrans = pAvatar->GetTransform();
                        if (Unity::IsNotNull(pTrans)) {
                            Unity::Vector3 worldPos = pTrans->GetPosition();
                            Unity::Vector3 screenPos = WorldToScreen(worldPos, g_viewMatrix, g_projMatrix, screenWidth, screenHeight);
                            if (screenPos.z > 0.f) {
                                AddText(pAvatar->playerName()->UTF8(), ImVec2(screenPos.x, screenPos.y), 0xFF009C0A, 13.f, TextShift_Center);
                            }
                        }
                    }
                    else if (PlayerDeathHead* pHead = pAvatar->playerDeathHead())
                    {
                        if (Unity::IsNotNull(pHead)) {
                            Unity::Vector3 worldPos = pHead->GetTransform()->GetPosition();
                            Unity::Vector3 screenPos = WorldToScreen(worldPos, g_viewMatrix, g_projMatrix, screenWidth, screenHeight);
                            if (screenPos.z > 0.f) {
                                if (auto pName = pAvatar->playerName())
                                    AddText(pName->UTF8(), ImVec2(screenPos.x, screenPos.y), 0xFF3D3D3D, 13.f, TextShift_Center);
                            }
                        }
                    }
                }
            }
        }

        ValuableDirector* pValuableDirector = ValuableDirector::instance();
        if (g_itemsEspSettings.on && pValuableDirector) {
            for (ValuableObject* vo : *pValuableDirector->valuableList()) {
                if (Unity::IsNotNull(vo) && vo->physGrabObject()) {

                    Unity::GameObject* go = vo->GetGameObject();
                    if (!go || !go->m_CachedPtr || !go->GetActiveInHierarchy())
                        continue;

                    PlayerAvatar* pPlayer = PlayerAvatar::instance();
                    if (!pPlayer || !pPlayer->GetGameObject() || !pPlayer->GetGameObject()->m_CachedPtr)
                        continue;

                    Unity::Vector3 worldPos = vo->physGrabObject()->centerPoint();
                    if (g_itemsEspSettings.limitDist && Unity::Vector3::Distance(worldPos, pPlayer->GetTransform()->GetPosition()) > 16.f)
                        continue;

                    Unity::Vector3 pos = WorldToScreen(worldPos, g_viewMatrix, g_projMatrix, screenWidth, screenHeight);

                    if (pos.z > 0) {
                        char* name = go->GetName()->UTF8();
                        std::string_view wrap(name);
                        if (wrap.ends_with("(Clone)")) *(name + strlen(name) - 7) = '\0';
                        if (wrap.starts_with("Valuable ")) name += 9;

                        if (g_itemsEspSettings.showPrice) {
                            char buff[64] = {0};
                            if (sprintf_s(buff, "%s (%.0f$)", name, vo->dollarValueCurrent()) > 0)
                                AddText(buff, ImVec2(pos.x, pos.y), 0xFF00FFFF, 13.f, TextShift_Center);
                        }
                        else {
                            AddText(name, ImVec2(pos.x, pos.y), 0xFF00FFFF, 13.f, TextShift_Center);
                        }
                    }
                }
            }
        }

        if (g_espSettings.on) {
            auto pEnemiesSpawned = pEnemyDirector->enemiesSpawned();
            for (auto it = pEnemiesSpawned->begin(); it != pEnemiesSpawned->end(); ++it) {
                EnemyParent* pEnemyParent = *it;
                if (pEnemyParent->m_CachedPtr && pEnemyParent->Spawned()) {
                    Enemy* pEnemy = pEnemyParent->enemy();
                    EnemyRigidbody* pRigidbody = pEnemy->Rigidbody();
                    std::wstring_view enemyName(pEnemyParent->enemyName()->Data());
                    if (enemyName.starts_with(L"Spewer")) {
                        EnemySlowMouth* pSlowMouth = (EnemySlowMouth*)pEnemyParent->GetComponentInChildren(EnemySlowMouth::GetType());
                        if (pSlowMouth->attachedTimer() > 0.f) {
                            continue;
                        }
                    }

                    auto enemyPos = pEnemy->CenterTransform()->GetPosition();
                    float dist = Unity::Vector3::Distance(enemyPos, PlayerAvatar::instance()->GetTransform()->GetPosition());
                    if (dist < 2.f)
                        continue;

                    auto bigBounds = Unity::Bounds(pEnemy->CenterTransform()->GetPosition(), Unity::Vector3::zero());
                    if (enemyName.find(L"Peeper") == std::wstring_view::npos) {
                        auto pGrabObject = pRigidbody->physGrabObject();
                        if (!g_cachedEnemyColliders.contains(pGrabObject)) {
                            auto pColliders = pGrabObject->GetComponentsInChildren(Unity::Collider::GetClass()->GetSystemType());
                            std::vector<Unity::Collider*> colliders;
                            colliders.reserve(pColliders->length);
                            for (auto it = pColliders->begin(); it != pColliders->end(); ++it) {
                                auto pCollider = (Unity::Collider*)(*it);
                                if (pCollider) {
                                    colliders.push_back(pCollider);
                                    bigBounds.Encapsulate(pCollider->GetBounds());
                                }
                            }
                            HaxSdk::Log("Colliders cached\n");
                            g_cachedEnemyColliders[pGrabObject] = colliders;
                        }
                        else {
                            for (Unity::Collider* pCollider : g_cachedEnemyColliders[pGrabObject]) {
                                bigBounds.Encapsulate(pCollider->GetBounds());
                            }
                        }
                    }
                    else {
                        auto pMeshes = pEnemyParent->GetComponentsInChildren(Unity::MeshRenderer::GetClass()->GetSystemType());
                        for (auto it = pMeshes->begin(); it != pMeshes->end(); ++it) {
                            if (auto pMesh = (Unity::MeshRenderer*)(*it))
                                bigBounds.Encapsulate(pMesh->GetBounds());
                        }
                    }

                    if (SemiFunc__OnScreen.ptr(bigBounds.center, 0.5f, 0.5f)) {
                        Unity::Rect rect = CalcBoundsInScreenSpace(bigBounds, pCamera);

                        if (g_espSettings.showBorder) {
                            float yOffset = rect.height / 4.f;
                            float xOffset = rect.width / 4.f;

                            ImVec2 topLeft[] = { ImVec2(rect.x, rect.y + yOffset), ImVec2(rect.x, rect.y), ImVec2(rect.x + xOffset, rect.y) };
                            ImGui::GetBackgroundDrawList()->AddPolyline(topLeft, 3, 0xFF000000, 0, 3.f);
                            ImGui::GetBackgroundDrawList()->AddPolyline(topLeft, 3, 0xFF0000FF, 0, 1.f);
                            ImVec2 topRight[] = { ImVec2(rect.x + rect.width - xOffset, rect.y), ImVec2(rect.x + rect.width, rect.y), ImVec2(rect.x + rect.width, rect.y + yOffset) };
                            ImGui::GetBackgroundDrawList()->AddPolyline(topRight, 3, 0xFF000000, 0, 3.f);
                            ImGui::GetBackgroundDrawList()->AddPolyline(topRight, 3, 0xFF0000FF, 0, 1.f);
                            ImVec2 bottomRight[] = { ImVec2(rect.x + rect.width - xOffset, rect.y + rect.height), ImVec2(rect.x + rect.width, rect.y + rect.height), ImVec2(rect.x + rect.width, rect.y + rect.height - yOffset) };
                            ImGui::GetBackgroundDrawList()->AddPolyline(bottomRight, 3, 0xFF000000, 0, 3.f);
                            ImGui::GetBackgroundDrawList()->AddPolyline(bottomRight, 3, 0xFF0000FF, 0, 1.f);
                            ImVec2 bottomLeft[] = { ImVec2(rect.x, rect.y + rect.height - yOffset), ImVec2(rect.x, rect.y + rect.height), ImVec2(rect.x + xOffset, rect.y + rect.height) };
                            ImGui::GetBackgroundDrawList()->AddPolyline(bottomLeft, 3, 0xFF000000, 0, 3.f);
                            ImGui::GetBackgroundDrawList()->AddPolyline(bottomLeft, 3, 0xFF0000FF, 0, 1.f);
                        }

                        if (g_espSettings.showName) {
                            const char* nameUTF8 = pEnemyParent->enemyName()->UTF8();
                            ImVec2 textSize = g_espFont->CalcTextSizeA(15.f, FLT_MAX, 0.0f, nameUTF8);
                            AddText(nameUTF8, ImVec2(rect.GetCenter().x, rect.y - 2.f - textSize.y), 0xFFFFFFFF, 15.f, TextShift_Center);
                        }

                        if (g_espSettings.showHp) {
                            EnemyHealth* pEnemyHealth = pEnemy->Health();
                            Int32 curHp = pEnemyHealth->healthCurrent();
                            Int32 maxHp = pEnemyHealth->health();
                            float coef = (float)curHp / maxHp;
                            ImU32 lerped = ImGui::ColorConvertFloat4ToU32(ImLerp(ImVec4(1.f, 0.f, 0.f, 1.f), ImVec4(0.f, 1.f, 0.f, 1.f), coef));
                            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(rect.x - 7.f, rect.y), ImVec2(rect.x - 7.f, rect.y + rect.height), 0xFF000000, 4.f);
                            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(rect.x - 7.f, rect.y + rect.height - rect.height * coef), ImVec2(rect.x - 7.f, rect.y + rect.height), lerped, 2.f);

                            char buff[8] = { 0 };
                            sprintf_s(buff, "%d", curHp);
                            AddText(buff, ImVec2(rect.x - 9.f, rect.y), lerped, 13.f, TextShift_Left);
                        }
                    }
                }
            }
        }
    } 
    catch (...) {
        g_lastErrorTime = std::chrono::steady_clock::now();
        g_skipRendering = true;
        HaxSdk::Log("Render failed, pausing for 1 second\n");
    }
}

void CheatMenu::Initialize() {
    #define HAX_STATIC_FIELD(c, t, f, a, ns, cn, fn) c ## __ ## f = (t*)(Class::Find(a, ns, cn)->FindStaticField(fn))
    #define HAX_FIELD(c, t, f, a, ns, cn, fn) c ## __ ## f = Class::Find(a, ns, cn)->FindField(fn)->Offset()
    #define HAX_METHOD(c, r, m, p, a, ns, cn, mn, s) c ## __ ## m = Class::Find(a, ns, cn)->FindMethod(mn, s)
    #include "game_data.h"

    std::cout << "EVENT:" << EventSystem__Update.ptr << '\n';
    {
        auto pType = Class::Find(MAIN_ASSEMBLY, "", "ExtractionPoint/State")->GetSystemType();
        auto pValues = System::Enum::GetValues(pType);
        auto pNames = System::Enum::GetNames(pType);
        for (size_t i = 0; i < pNames->length; ++i) {
            auto pName = pNames->operator[](i);
            if (wcscmp(pName->chars, L"Idle") == 0) { STATE_IDLE = pValues->operator[](i); }
        }
    }
    {
        auto pType = Class::Find(MAIN_ASSEMBLY, "", "RunManager/ChangeLevelType")->GetSystemType();
        auto pValues = System::Enum::GetValues(pType);
        auto pNames = System::Enum::GetNames(pType);
        for (size_t i = 0; i < pNames->length; ++i) {
            auto pName = pNames->operator[](i);
            if (wcscmp(pName->chars, L"Normal") == 0) { NORMAL = pValues->operator[](i); }
        }
    }
    {
        auto pType = Class::Find(MAIN_ASSEMBLY, "", "GameDirector/gameState")->GetSystemType();
        auto pValues = System::Enum::GetValues(pType);
        auto pNames = System::Enum::GetNames(pType);
        for (size_t i = 0; i < pNames->length; ++i) {
            auto pName = pNames->operator[](i);
            if (wcscmp(pName->chars, L"Main") == 0) { MAIN = pValues->operator[](i); }
        }
    }
    {
        auto pType = Class::Find(MAIN_ASSEMBLY, "", "ExtractionPoint/State")->GetSystemType();
        auto pValues = System::Enum::GetValues(pType);
        auto pNames = System::Enum::GetNames(pType);
        for (size_t i = 0; i < pNames->length; ++i) {
            auto pName = pNames->operator[](i);
            if (wcscmp(pName->chars, L"Active") == 0) { STATE_ACTIVE = pValues->operator[](i); }
            else if (wcscmp(pName->chars, L"Complete") == 0) { STATE_COMPLETE = pValues->operator[](i); }
        }
    }
    HAX_ASSERT((STATE_IDLE >= 0), "STATE_IDLE not found");
    HAX_ASSERT((NORMAL >= 0), "NORMAL not found");
    HAX_ASSERT((MAIN >= 0), "MAIN not found");
    HAX_ASSERT((STATE_ACTIVE >= 0), "STATE_ACTIVE not found");
    HAX_ASSERT((STATE_COMPLETE >= 0), "STATE_COMPLETE not found");

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    ::DetourAttach(&PlayerHealth__Hurt.orig, Hooked_PlayerHealth__Hurt);
    ::DetourAttach(&PlayerHealth__Update.orig, Hooked_PlayerHealth__Update);
    ::DetourAttach(&PlayerController__FixedUpdate.orig, Hooked_PlayerController__FixedUpdate);
    ::DetourAttach(&PlayerController__Update.orig, Hooked_PlayerController__Update);
    ::DetourAttach(&PlayerTumble__TumbleRequest.orig, Hooked_PlayerTumble__TumbleRequest);
    ::DetourAttach(&EnemyDirector__Update.orig, Hooked_EnemyDirector__Update);
    ::DetourAttach(&EnemyHealth__Hurt.orig, Hooked_EnemyHealth__Hurt);
    ::DetourAttach(&EnemyRigidbody__FixedUpdate.orig, Hooked_EnemyRigidbody__FixedUpdate);
    ::DetourAttach(&EnemyParent__Update.orig, Hooked_EnemyParent__Update);
    ::DetourAttach(&PlayerAvatar__OnDestroy.orig, Hooked_PlayerAvatar__OnDestroy);
    ::DetourAttach(&GameDirector__Update.orig, Hooked_GameDirector__Update);
    ::DetourAttach(&PhysGrabber__PhysGrabLogic.orig, Hooked_PhysGrabber__PhysGrabLogic);
    ::DetourAttach(&PhysGrabber__RayCheck.orig, Hooked_PhysGrabber__RayCheck);
    ::DetourAttach(&Physics__Raycast.orig, Hooked_Physics__Raycast);
    ::DetourAttach(&ItemBattery__Update.orig, Hooked_ItemBattery__Update);
    ::DetourAttach(&ItemGun__Shoot.orig, Hooked_ItemGun__Shoot);
    ::DetourAttach(&ItemGun__Update.orig, Hooked_ItemGun__Update);
    ::DetourAttach(&RunManager__SetRunLevel.orig, Hooked_RunManager__SetRunLevel);
    ::DetourAttach(&PhotonNetwork__IsMasterClient.orig, Hooked_PhotonNetwork__IsMasterClient);
    ::DetourAttach(&RenderSettings__set_fog.orig, Hooked_RenderSettings__set_fog);
    ::DetourAttach(&PhysGrabObjectImpactDetector__FixedUpdate.orig, Hooked_PhysGrabObjectImpactDetector__FixedUpdate);
    ::DetourAttach(&DataDirector__SaveDeleteCheck.orig, Hooked_DataDirector__SaveDeleteCheck);
    ::DetourAttach(&PlayerAvatar__SpawnRPC.orig, Hooked_PlayerAvatar__SpawnRPC);
    ::DetourAttach(&Camera__FireOnPreRender.orig, Hooked_Camera__FireOnPreRender);
    ::DetourAttach(&PlayerAvatar__PlayerDeathRPC.orig, Hooked_PlayerAvatar__PlayerDeathRPC);
    DetourTransactionCommit();
}

static void AddText(const char* text, const ImVec2& pos, ImU32 col, float fontSize, TextShift shift) {
    ImVec2 textSize = g_espFont->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);
    ImDrawList* pDrawList = ImGui::GetBackgroundDrawList();
    float xShift = shift == TextShift_Right ? 0.f : textSize.x / (float)shift;
    pDrawList->AddText(g_espFont, fontSize, ImVec2(pos.x - xShift + 1.F, pos.y - 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(g_espFont, fontSize, ImVec2(pos.x - xShift + 1.F, pos.y + 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(g_espFont, fontSize, ImVec2(pos.x - xShift - 1.F, pos.y + 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(g_espFont, fontSize, ImVec2(pos.x - xShift - 1.F, pos.y - 1.F), IM_COL32_BLACK, text);
    pDrawList->AddText(g_espFont, fontSize, ImVec2(pos.x - xShift, pos.y), col, text);
}

static Unity::Vector3 WorldToScreen(Unity::Vector3& worldPos, Unity::Matrix4x4& viewMatrix, Unity::Matrix4x4& projectionMatrix, float screenWidth, float screenHeight)
{
    // 1. Конвертация Unity::Vector3 в Vector4
    Unity::Vector4 worldPos4 = Unity::Vector4(worldPos.x, worldPos.y, worldPos.z, 1.0f);

    // 2. View * World
    Unity::Vector4 viewSpace = viewMatrix * worldPos4;

    // 3. Projection * View
    Unity::Vector4 clipSpace = projectionMatrix * viewSpace;

    if (clipSpace.w < 0.1f)
        return Unity::Vector3(-9999, -9999, -9999); // объект за камерой

    // 4. Преобразование в нормализованные координаты устройства (NDC)
    Unity::Vector3 ndc;
    ndc.x = clipSpace.x / clipSpace.w;
    ndc.y = clipSpace.y / clipSpace.w;
    ndc.z = clipSpace.z / clipSpace.w;

    // 5. Преобразование в экранные координаты
    Unity::Vector3 screenPos;
    screenPos.x = (ndc.x * 0.5f + 0.5f) * screenWidth;
    screenPos.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * screenHeight;
    screenPos.z = ndc.z;

    return screenPos;
}

static Unity::Rect CalcBoundsInScreenSpace(Unity::Bounds bigBounds, Unity::Camera* pCamera) {
    float screenWidth = (float)Unity::Screen::GetWidth();
    float screenHeight = (float)Unity::Screen::GetHeight();
    float ScaleX = screenWidth / (float)pCamera->GetPixelWidth();
    float ScaleY = screenHeight / (float)pCamera->GetPixelHeight();

    Unity::Vector3 screenSpaceCorners[8] =
    {
        Unity::Vector3(bigBounds.center.x + bigBounds.extents.x, bigBounds.center.y + bigBounds.extents.y, bigBounds.center.z + bigBounds.extents.z),
        Unity::Vector3(bigBounds.center.x + bigBounds.extents.x, bigBounds.center.y + bigBounds.extents.y, bigBounds.center.z - bigBounds.extents.z),
        Unity::Vector3(bigBounds.center.x + bigBounds.extents.x, bigBounds.center.y - bigBounds.extents.y, bigBounds.center.z + bigBounds.extents.z),
        Unity::Vector3(bigBounds.center.x + bigBounds.extents.x, bigBounds.center.y - bigBounds.extents.y, bigBounds.center.z - bigBounds.extents.z),
        Unity::Vector3(bigBounds.center.x - bigBounds.extents.x, bigBounds.center.y + bigBounds.extents.y, bigBounds.center.z + bigBounds.extents.z),
        Unity::Vector3(bigBounds.center.x - bigBounds.extents.x, bigBounds.center.y + bigBounds.extents.y, bigBounds.center.z - bigBounds.extents.z),
        Unity::Vector3(bigBounds.center.x - bigBounds.extents.x, bigBounds.center.y - bigBounds.extents.y, bigBounds.center.z + bigBounds.extents.z),
        Unity::Vector3(bigBounds.center.x - bigBounds.extents.x, bigBounds.center.y - bigBounds.extents.y, bigBounds.center.z - bigBounds.extents.z)
    };

    for (int i = 0; i < 8; ++i) {
        screenSpaceCorners[i] = WorldToScreen(screenSpaceCorners[i], g_viewMatrix, g_projMatrix, screenWidth, screenHeight);
    }

    float x = FLT_MAX;
    float y = FLT_MAX;
    float x2 = FLT_MIN;
    float y2 = FLT_MIN;

    for (const Unity::Vector3& v : screenSpaceCorners) {
        if (v.z > 0.f) {
            x = (std::min)(v.x, x);
            y = (std::min)(v.y, y);
            x2 = (std::max)(v.x, x2);
            y2 = (std::max)(v.y, y2);
        }
    }

    if (y2 - y < 15.f) {
        y2 = y + 15.f;
    }

    return Unity::Rect::MinMaxRect(x, y, x2, y2);
}

static bool DisabledButton(const char* label, bool enabled) {
    ImGui::BeginDisabled(!enabled);
    bool pressed = ImGui::Button(label);
    ImGui::EndDisabled();
    return pressed;
}

static bool DisabledCheckbox(const char* label, bool* pFlag, bool condition) {
    bool tmp = false;
    bool enabled = condition && pFlag;
    ImGui::BeginDisabled(!enabled);
    bool pressed = ImGui::Checkbox(label, pFlag ? pFlag : &tmp);
    ImGui::EndDisabled();
    return pressed;
}

static void ParseEnemies(System::List<EnemySetup*>* enemies) {
    for (auto pEnemySetup : *enemies) {
        for (auto pEnemyGo : *pEnemySetup->spawnObjects()) {
            auto go = (Unity::GameObject*)Unity::Object::Instantiate(pEnemyGo, Unity::Vector3(0.f, -1000.f, 0.f), Unity::Quaternion::identity());
            auto pEnemyParent = (EnemyParent*)go->GetComponent(EnemyParent::GetType());
            if (pEnemyParent) {
                std::string enemyName(pEnemyParent->enemyName()->UTF8());
                if (enemyName != "Banger" && enemyName != "Gnome" && !g_enemiesPool.contains(enemyName)) {
                    g_enemiesPool[enemyName] = pEnemySetup;
                    HaxSdk::Log(std::format("Added enemy {}\n", enemyName));
                }
            }
            Unity::Object::Destroy(go);
        }
    }
}

static void ParseItems() {
    auto pDict = StatsManager::instance()->itemDictionary();
    auto pEntries = pDict->GetEntries();
    Int32 count = pDict->Count();
    for (Int32 i = 0; i < count; ++i) {
        if (const char* utf8 = pEntries->operator[](i).key->UTF8()) {
            std::string name(utf8 + (strncmp(utf8, "Item ", 5) == 0 ? 5 : 0));
            if (!g_itemsPool.contains(name)) {
                g_itemsPool[name] = pEntries->operator[](i).value;
                HaxSdk::Log(std::format("Item parsed {}\n", name));
            }
        }
    }
}

static void ParseLevels() {
    for (Level* pLevel : *RunManager::instance()->levels()) {
        if (const char* utf8 = pLevel->NarrativeName()->UTF8()) {
            g_levelBans.emplace_back(pLevel, std::string(utf8));
            HaxSdk::Log(std::format("Parsed level {}\n", utf8));
        }
        else {
            HaxSdk::Log("Unable to parse level. Name is nullptr\n");
        }
    }
}

static bool RemoveEnemyFromMap(EnemyParent* pEnemyParent) {
    if (g_enemiesOnMap.contains(pEnemyParent)) {
        auto pMapCustom = g_enemiesOnMap[pEnemyParent];
        auto pMapCustomEntity = pMapCustom->mapCustomEntity();
        if (!pMapCustomEntity || !pMapCustomEntity->m_CachedPtr) {
            HaxSdk::Log("Enemy removed from map\n");
            return true;
        }
    }
    return false;
}

static void ShowTooltip(const char* tip) {
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::BeginTooltip();
        ImGui::Text(tip);
        ImGui::EndTooltip();
    }
}

static void ShowHelpMarker(const char* tip) {
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    ShowTooltip(tip);
}

static void ShowWarningMarker(const char* tip) {
    ImGui::SameLine();
    ImGui::TextDisabled("(!)");
    ShowTooltip(tip);
}

static bool PremiumButton(const char* label, bool enabled, HaxTexture& watermark) {
    static float wmSize = ImGui::CalcTextSize("Text").y + ImGui::GetStyle().FramePadding.y * 2.f;
#ifdef FREE
    ImGui::BeginDisabled(true);
    ImGui::Button(label);
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::Image(watermark.m_pTexture, ImVec2(wmSize, wmSize));
    int lang = HaxSdk::GetGlobals().locale;
    ShowTooltip(Locales::WATERMARK[lang]);
    return false;
#else
    return DisabledButton(label, enabled);
#endif
}

static bool PremiumCheckbox(const char* label, bool* pFlag, bool condition, HaxTexture& watermark) {
    static float wmSize = ImGui::CalcTextSize("Text").y + ImGui::GetStyle().FramePadding.y * 2.f;
#ifdef FREE
    bool tmp = false;
    ImGui::BeginDisabled();
    ImGui::Checkbox(label, &tmp);
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::Image(watermark.m_pTexture, ImVec2(wmSize, wmSize));
    int lang = HaxSdk::GetGlobals().locale;
    ShowTooltip(Locales::WATERMARK[lang]);
    return false;
#else
    return DisabledCheckbox(label, pFlag, condition);
#endif
}

static void Hooked_PlayerHealth__Update(PlayerHealth* __this) {
    if (g_shouldHealToMax && __this->playerAvatar()->isLocal()) {
        g_shouldHealToMax = false;
        Int32 arg1 = 99999; bool arg2 = true;
        void* args[] = { &arg1, &arg2 };
        PlayerHealth__HealOther.Invoke(__this, args);
    }
    PlayerHealth__Update.orig(__this);
}

static void Hooked_PlayerController__Update(PlayerController* __this) {
    Int32& jumps = __this->JumpExtra();

    Int32 cachedJumps = jumps;
    if (g_infJumps) { jumps = 9999; }

    PlayerController__Update.orig(__this);

    jumps = cachedJumps;
}

static void Hooked_PlayerHealth__Hurt(PlayerHealth* __this, Int32 dmg, bool savingGrace, Int32 enemyIndex) {
    if (g_godmode && __this == PlayerAvatar::instance()->playerHealth()) {
        if (g_healToMax) {
            Int32 arg1 = 99999;
            bool arg2 = true;
            void* args[] = { &arg1, &arg2 };
            PlayerHealth__HealOther.Invoke(__this, args);
        }
        return;
    }
    PlayerHealth__Hurt.orig(__this, dmg, savingGrace, enemyIndex);
}

static void Hooked_PlayerAvatar__PlayerDeathRPC(PlayerAvatar* __this, Int32 enemyIndex) {
    if (g_shouldBecomeGhost && __this->isLocal()) {
        return;
    }
    PlayerAvatar__PlayerDeathRPC.orig(__this, enemyIndex);
}

static void Hooked_PlayerController__FixedUpdate(PlayerController* __this) {
    float& moveSpeed = __this->MoveSpeed();
    float& sprintSpeed = __this->SprintSpeed();
    float& crouchSpeed = __this->CrouchSpeed();

    float cachedMovespeed = moveSpeed;
    float cachedSprintSpeed = sprintSpeed;
    float cachedCrouchSpeed = crouchSpeed;

    if (g_infStamina) { __this->EnergyCurrent() = __this->EnergyStart(); }
    if (g_speedMult > 1.f) {
        moveSpeed *= g_speedMult;
        sprintSpeed *= g_speedMult;
        crouchSpeed *= g_speedMult;
    }

    if (g_crouchSpeedType == SpeedType_Walk) { crouchSpeed = moveSpeed; }
    if (g_crouchSpeedType == SpeedType_Sprint) { crouchSpeed = sprintSpeed; }
    if (g_walkSpeedType == SpeedType_Sprint) { moveSpeed = sprintSpeed; }

    PlayerController__FixedUpdate.orig(__this);

    moveSpeed = cachedMovespeed;
    sprintSpeed = cachedSprintSpeed;
    crouchSpeed = cachedCrouchSpeed;
}

static void Hooked_PlayerTumble__TumbleRequest(PlayerTumble* __this, bool _isTumbling, bool _playerInput) {
    if (!g_neverTumble || !_isTumbling) {
        PlayerTumble__TumbleRequest.orig(__this, _isTumbling, _playerInput);
    }
}

static void Hooked_EnemyDirector__Update(EnemyDirector* __this) {
    if (g_enemiesPool.size() == 0) {
        ParseEnemies(__this->enemiesDifficulty1());
        ParseEnemies(__this->enemiesDifficulty2());
        ParseEnemies(__this->enemiesDifficulty3());
    }

    if (g_itemsPool.size() == 0) {
        ParseItems();
    }

    if (g_levelBans.size() == 0) {
        ParseLevels();
    }

    if (g_pPlayerToKill) {
        Int32 arg = 0;
        void* args[] = { &arg };
        PlayerAvatar__PlayerDeath.Invoke(g_pPlayerToKill, args);
        g_pPlayerToKill = nullptr;
    }

    if (g_pPlayerToRevive) {
        bool arg = false;
        void* args[] = { &arg };
        PlayerAvatar__Revive.Invoke(g_pPlayerToRevive, args);
        g_pPlayerToRevive = nullptr;
    }

    if (g_shouldTeleportItems) {
        g_shouldTeleportItems = false;
        for (ValuableObject* vo : *ValuableDirector::instance()->valuableList()) {
            if (Unity::IsNotNull(vo) && vo->physGrabObject()) {
                Unity::Vector3 arg1 = PlayerAvatar::instance()->GetTransform()->GetPosition();
                Unity::Quaternion arg2 = Unity::Quaternion::identity();
                void* args[] = { &arg1, &arg2 };
                PhysGrabObject__Teleport.Invoke(vo->physGrabObject(), args);
            }
        }
    }

    if (g_enemyToSpawn) {
        EnemySetup* pEnemy = g_enemyToSpawn;
        g_enemyToSpawn = nullptr;
        try {
            auto pLevelGenerator = LevelGenerator::Instance();
            if (pLevelGenerator->Generated()) {
                Unity::Vector3 spawnPos = LevelGenerator::Instance()->LevelPathPoints()->operator[](0)->GetTransform()->GetPosition();
                void* args[] = { pEnemy, &spawnPos };
                LevelGenerator__EnemySpawn.Invoke(pLevelGenerator, args);
            }
        }
        catch (...) {
            HaxSdk::Log("Unable to spawn enemy\n");
        }
    }

    if (g_itemToSpawn) {
        Item* pItem = g_itemToSpawn;
        g_itemToSpawn = nullptr;
        try {
            if (Unity::IsNotNull(pItem)) {
                if (SemiFunc__IsMasterClient.ptr()) {
                    System::String* pNewName = System::String::New("Items/")->operator+(pItem->prefab()->GetName());
                    auto pTransform = SemiFunc__MainCamera.ptr()->GetTransform();
                    Unity::Vector3 pos = pTransform->GetPosition() + pTransform->GetForward() * 2.f;
                    Photon::PhotonNetwork::InstantiateRoomObject(pNewName, pos, Unity::Quaternion::identity(), 0, nullptr);
                }
                if (!SemiFunc__IsMultiplayer.ptr()) {
                    auto pTransform = SemiFunc__MainCamera.ptr()->GetTransform();
                    Unity::Vector3 pos = pTransform->GetPosition() + pTransform->GetForward() * 2.f;
                    Unity::Object::Instantiate(pItem->prefab(), pos, Unity::Quaternion::identity());
                }
            }
        }
        catch (...) {
            HaxSdk::Log("Unable to spawn item\n");
        }
    }

    /*if (g_shouldDespawnEnemies) {
        g_shouldDespawnEnemies = false;
        for (auto pEnemy : *__this->enemiesSpawned()) {
            EnemyParent__Despawn.ptr(pEnemy);
        }
    }*/

    if (g_shouldKillEnemies) {
        g_shouldKillEnemies = false;
        Int32 dmg = 99999;
        Unity::Vector3 dir = Unity::Vector3::one();
        void* args[] = { &dmg, &dir };
        for (auto pEnemy : *__this->enemiesSpawned()) {
            EnemyHealth__Hurt.Invoke(pEnemy->enemy()->Health(), args);
        }
    }

    if (g_shouldBecomeGhost) {
        Int32 arg = 0;
        void* args[] = { &args };
        PlayerAvatar__PlayerDeath.Invoke(PlayerAvatar::instance(), args);
        g_shouldBecomeGhost = false;
    }

    if (!g_pChamsMaterial) {
        auto pShader = Unity::Shader::Find(System::String::New("Hidden/Internal-Colored"));
        g_pChamsMaterial = Unity::Material::New(pShader);
        g_pChamsMaterial->SetHideFlags(Unity::HideFlags::HideAndDontSave);
        g_pChamsMaterial->SetInt(System::String::New("_ZWrite"), 0);
        g_pChamsMaterial->SetInt(System::String::New("_ZTest"), 8);
        HaxSdk::Log("Material created\n");
    }

    if (g_shouldRevealRooms) {
        g_shouldRevealRooms = false;
        for (LevelPoint* pPoint : *LevelGenerator::Instance()->LevelPathPoints()) {
            RoomVolume__SetExplored.ptr(pPoint->Room());
        }
    }

    if (g_shouldFillQuota) {
        g_shouldFillQuota = false;
        g_pretendMaster = true;
        if (auto pDirector = RoundDirector::instance()) {
            if (auto pPoint = pDirector->extractionPointCurrent()) {
                ExtractionPoint__HaulGoalSet.ptr(pPoint, 0);
            }
        }
        g_pretendMaster = false;
    }

    if (g_shouldUnlockAllPoints) {
        g_shouldUnlockAllPoints = false;
        RoundDirector__ExtractionPointsUnlock.ptr(RoundDirector::instance());
    }

    if (g_shouldSetInfMoney) {
        g_shouldSetInfMoney = false;
        g_pretendMaster = true;
        SemiFunc__StatSetRunCurrency.ptr(999999);
        g_pretendMaster = false;
    }

    if (switchParams.shouldSwitch) {
        switchParams.shouldSwitch = false;
        PlayerVoiceChat__ToggleLobby.ptr(switchParams.pChat, switchParams.newValue);
    }

    EnemyDirector__Update.orig(__this);
}

static void Hooked_EnemyHealth__Hurt(EnemyHealth* __this, Int32 _damage, void* _hurtDirection) {
    _damage *= g_damageMult;
    if (g_ohk) {
        _damage = __this->health();
    }
    EnemyHealth__Hurt.orig(__this, _damage, _hurtDirection);
}

static void Hooked_EnemyRigidbody__FixedUpdate(EnemyRigidbody* __this) {
    if (g_infiniteGrab) {
        __this->grabShakeReleaseTimer() = 0.f;
        EnemyDirector::instance()->debugEasyGrab() = true;
        EnemyDirector::instance()->debugNoGrabMaxTime() = true;
    }
    EnemyRigidbody__FixedUpdate.orig(__this);
    if (g_infiniteGrab) {
        EnemyDirector::instance()->debugEasyGrab() = false;
        EnemyDirector::instance()->debugNoGrabMaxTime() = false;
    }
}

static void Hooked_EnemyParent__Update(EnemyParent* __this) {
    Enemy* pEnemy = __this->enemy();
    Int32 enemyState = pEnemy->CurrentState();
    if (enemyState != 0 && enemyState != 0xB && enemyState != 1) {
        MapCustom* pCustom = g_enemiesOnMap[__this];
        if (!pCustom) {
            pCustom = (MapCustom*)__this->GetComponent(MapCustom::GetType());
            if (!pCustom) {
                pCustom = (MapCustom*)pEnemy->GetGameObject()->AddComponent(MapCustom::GetType());
                pCustom->SetName(__this->enemyName()->operator+(System::String::New("_Point")));
                pCustom->color() = Unity::Color::red();
                pCustom->sprite() = PlayerAvatar::instance()->playerDeathHead()->mapCustom()->sprite();
                HaxSdk::Log("MAPCUSTOM CREATED\n");
            }
            else {
                HaxSdk::Log("MAPCUSTOM FOUND\n");
            }
            g_enemiesOnMap[__this] = pCustom;
        }

        if (!g_showEnemiesOnMap || __this->enemy()->Health()->dead()) {
            MapCustom__Hide.ptr(pCustom);
        }
    }
    else if (g_enemiesOnMap.contains(__this)) {
        MapCustom__Hide.ptr(g_enemiesOnMap[__this]);
    }
}

static void Hooked_GameDirector__Update(GameDirector* __this) {
    Unity::Camera* pCamera = __this->MainCamera();
    float farClipPlane = pCamera->GetFarClipPlane();
    float drawDistance = (float)g_drawDistance;

    if (farClipPlane != (float)RenderDistance_High && farClipPlane != (float)RenderDistance_Medium) {
        g_validFarClipPlane = farClipPlane;
    }
    if (farClipPlane != drawDistance && drawDistance != (float)RenderDistance_Default) {
        pCamera->SetFarClipPlane(drawDistance);
    }
    if (drawDistance == (float)RenderDistance_Default && farClipPlane != g_validFarClipPlane) {
        pCamera->SetFarClipPlane(g_validFarClipPlane);
    }

    if (g_shouldRevealValuables) {
        g_shouldRevealValuables = false;
        if (auto pValuableDirector = ValuableDirector::instance()) {
            Int32 arg = 0;
            void* args[] = { &arg };
            for (ValuableObject* v : *pValuableDirector->valuableList()) {

                ValuableObject__Discover.Invoke(v, args);
            }
        }
    }

    if (g_shouldActivateNextExtr) {
        g_shouldActivateNextExtr = false;
        for (Unity::GameObject* pGo : *RoundDirector::instance()->extractionPointList()) {
            if (auto pPoint = (ExtractionPoint*)pGo->GetComponent(ExtractionPoint::GetType())) {
                if (pPoint->currentState() == STATE_IDLE && !pPoint->isLocked()) {
                    ExtractionPoint__OnClick.Invoke(pPoint, nullptr);
                }
            }
        }
    }

    if (g_shouldChangeLevel) {
        g_shouldChangeLevel = false;
        bool arg1 = true; bool arg2 = false;
        void* args[] = { &arg1, &arg2, &NORMAL };
        RunManager__ChangeLevel.Invoke(RunManager::instance(), args);
    }

    GameDirector__Update.orig(__this);
}

static void Hooked_PlayerAvatar__OnDestroy(PlayerAvatar* __this) {
    PlayerAvatar__OnDestroy.orig(__this);

    HaxSdk::Log(std::format("Clearing cache of enemy-mapcustom: size was {}\n", g_enemiesOnMap.size()));
    g_enemiesOnMap.clear();

    HaxSdk::Log(std::format("Clearing cache of gun-laser: size was {}\n", g_cachedGunLasers.size()));
    g_cachedGunLasers.clear();

    HaxSdk::Log(std::format("Clearing cache of enemy-colliders: size was {}\n", g_cachedEnemyColliders.size()));
    g_cachedEnemyColliders.clear();
}

static void Hooked_PhysGrabber__PhysGrabLogic(PhysGrabber* __this) {
    float& range = __this->grabRange();
    float cached = range;
    if (g_highGrabRange && __this == PlayerAvatar::instance()->physGrabber()) {
        range = 999999.f;
    }
    PhysGrabber__PhysGrabLogic.orig(__this);
    range = cached;
}

static void Hooked_PhysGrabber__RayCheck(PhysGrabber* __this, bool _grab) {
    float& range = __this->grabRange();
    float cached = range;
    if (g_highGrabRange && __this == PlayerAvatar::instance()->physGrabber()) {
        range = 999999.f;
        g_raycastMaxDist = true;
    }
    PhysGrabber__RayCheck.orig(__this, _grab);

    range = cached;
    g_raycastMaxDist = false;
}

static bool Hooked_Physics__Raycast(void* v1, void* v2, void* v3, float maxDistance, Int32 v5, Int32 v6) {
    if (g_raycastMaxDist) {
        maxDistance = 999999.f;
    }
    return Physics__Raycast.orig(v1, v2, v3, maxDistance, v5, v6);
}

static void Hooked_ItemBattery__Update(ItemBattery* __this) {
    if (g_infBattery) {
        __this->batteryLife() = 100.f;
    }
    ItemBattery__Update.orig(__this);
}

static void Hooked_ItemGun__Shoot(ItemGun* __this) {
    float& spread = __this->gunRandomSpread();
    float cached = spread;
    if (g_noSpread) {
        spread = 0.f;
    }
    ItemGun__Shoot.orig(__this);
    spread = cached;
}

static void Hooked_ItemGun__Update(ItemGun* __this) {
    Unity::LineRenderer* pLaser = g_cachedGunLasers[__this];
    if (!pLaser) {
        auto pType = Unity::LineRenderer::GetClass()->GetSystemType();
        pLaser = (Unity::LineRenderer*)__this->GetComponent(pType);
        if (!pLaser) {
            pLaser = (Unity::LineRenderer*)__this->GetGameObject()->AddComponent(pType);
            pLaser->SetStartWidth(0.02f);
            pLaser->SetEndWidth(0.02f);
            pLaser->SetSortingOrder(1);
            pLaser->SetPositionCount(2);
            Unity::Material* pMat = Unity::Material::New(Unity::Shader::Find(System::String::New("Sprites/Default")));
            pMat->SetColor(Unity::Color::red());
            pLaser->SetMaterial(pMat);
            HaxSdk::Log("Laser created\n");
        }
        g_cachedGunLasers[__this] = pLaser;
    }

    PhysGrabObject* pPhys = __this->physGrabObject();
    if (g_useGunLaser && pPhys && pPhys->grabbedLocal()) {
        auto pTransform = __this->gunMuzzle();
        pLaser->SetEnabled(true);
        pLaser->SetPosition(0, pTransform->GetPosition());
        pLaser->SetPosition(1, pTransform->GetPosition() + pTransform->GetForward() * __this->gunRange());
    }
    else {
        pLaser->SetEnabled(false);
    }

    ItemGun__Update.orig(__this);
}

static void Hooked_RunManager__SetRunLevel(RunManager* __this) {
    LevelBan level = g_levelBans[0];
    Level* pPrevLevel = __this->previousRunLevel();
    size_t totalAllowed = g_levelBans.size() - LevelBan::totalBans;
    while (!level.allowed || (level.pLevel == pPrevLevel && totalAllowed > 1)) {
        level = g_levelBans[rand() % g_levelBans.size()];
    }
    __this->levelCurrent() = level.pLevel;
}

static bool Hooked_PhotonNetwork__IsMasterClient() {
    return g_pretendMaster ? true : PhotonNetwork__IsMasterClient.orig();
}

static void Hooked_RenderSettings__set_fog(bool value) {
    g_validFog = value;
    RenderSettings__set_fog.orig(g_removeFog ? false : value);
}

static void Hooked_PhysGrabObjectImpactDetector__FixedUpdate(PhysGrabObjectImpactDetector* __this) {
    float& timer = __this->impactDisabledTimer();
    if (g_noImpact && __this->isValuable()) {
        timer = (std::max)(1.f, timer);
    }
    PhysGrabObjectImpactDetector__FixedUpdate.orig(__this);
}

static void Hooked_DataDirector__SaveDeleteCheck(DataDirector* __this, bool _leaveGame) {
    if (!g_noSaveDelete) {
        DataDirector__SaveDeleteCheck.orig(__this, _leaveGame);
    }
}

static void Hooked_PlayerAvatar__SpawnRPC(PlayerAvatar* __this, void* position, void* rotation) {
    if (__this->isLocal() && __this->spawned()) {
        HaxSdk::Log("Antivoid defence triggered\n");
        return;
    }
    PlayerAvatar__SpawnRPC.orig(__this, position, rotation);
}

static void Hooked_Camera__FireOnPreRender(Unity::Camera* pCam) {
    if (pCam == SemiFunc__MainCamera.ptr()) {
        g_viewMatrix = pCam->GetWorldToCameraMatrix();
        g_projMatrix = pCam->GetProjectionMatrix();
    }
    Camera__FireOnPreRender.orig(pCam);
}