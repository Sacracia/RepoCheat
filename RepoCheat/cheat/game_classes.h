#pragma once

#include "haxsdk_unity.h"

#define STATIC_FIELD(t, n)                                      \
static t& n()                                                   \
{                                                               \
    return *(t*)s_ ## n ## .m_Address;                          \
}

#define FIELD(t, n)                                             \
t& n()                                                          \
{                                                               \
    CHECK_NULL();                                               \
    return *(t*)((char*)GetPointer() + s_ ## n ## .m_Offset);   \
}

#define ENUM_VALUE(n)                                           \
static int n()                                                  \
{                                                               \
    return s_ ## n ## .m_Value;                                 \
}

using namespace System;
using namespace Unity;

struct PhysGrabObject : Unity::Behaviour
{
    inline operator bool() const { return !Null(); }
    inline bool operator==(const PhysGrabObject& o) const { return o.m_Pointer == m_Pointer; }

    FIELD(bool, grabbedLocal);
    FIELD(Vector3, centerPoint);

private:
    static inline HaxSdk::FieldOffset s_grabbedLocal{"Assembly-CSharp", "", "PhysGrabObject", "grabbedLocal"};
    static inline HaxSdk::FieldOffset s_centerPoint{"Assembly-CSharp", "", "PhysGrabObject", "centerPoint"};
};

template <>
struct std::hash<PhysGrabObject>
{
    size_t operator()(const PhysGrabObject& e) const { return (size_t)e.GetPointer(); }
};

struct EnemyRigidbody : Unity::Behaviour
{
    inline operator bool() const { return !Null(); }

    FIELD(PhysGrabObject, physGrabObject);
    FIELD(float, grabShakeReleaseTimer);
    FIELD(float, grabStrengthTimer);
    FIELD(float, grabStrengthTime);

private:
    static inline HaxSdk::FieldOffset s_physGrabObject{"Assembly-CSharp", "", "EnemyRigidbody", "physGrabObject"};
    static inline HaxSdk::FieldOffset s_grabShakeReleaseTimer{"Assembly-CSharp", "", "EnemyRigidbody", "grabShakeReleaseTimer"};
    static inline HaxSdk::FieldOffset s_grabStrengthTimer{"Assembly-CSharp", "", "EnemyRigidbody", "grabStrengthTimer"};
    static inline HaxSdk::FieldOffset s_grabStrengthTime{"Assembly-CSharp", "", "EnemyRigidbody", "grabStrengthTime"};
public:
    static inline MethodInfo s_FixedUpdate{"Assembly-CSharp", "", "EnemyRigidbody", "FixedUpdate"};
};

struct EnemyHealth : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    FIELD(int, healthCurrent);
    FIELD(int, health);
    FIELD(bool, dead);

    void Hurt(int damage, const Vector3& hurtDirection)
    {
        CHECK_NULL();
        s_Hurt.Thunk<void, EnemyHealth, int, const Vector3_Boxed&>(*this, damage, hurtDirection.Box());
    }

private:
    static inline HaxSdk::FieldOffset s_healthCurrent{"Assembly-CSharp", "", "EnemyHealth", "healthCurrent"};
    static inline HaxSdk::FieldOffset s_health{"Assembly-CSharp", "", "EnemyHealth", "health"};
    static inline HaxSdk::FieldOffset s_dead{"Assembly-CSharp", "", "EnemyHealth", "dead"};
public:
    static inline MethodInfo s_Hurt{"Assembly-CSharp", "", "EnemyHealth", "Hurt"};
};

struct EnemyState
{
    ENUM_VALUE(None);
    ENUM_VALUE(Spawn);
    ENUM_VALUE(Despawn);

private:
    static inline HaxSdk::EnumValue s_None{"Assembly-CSharp", "", "EnemyState", "None"};
    static inline HaxSdk::EnumValue s_Spawn{"Assembly-CSharp", "", "EnemyState", "Spawn"};
    static inline HaxSdk::EnumValue s_Despawn{"Assembly-CSharp", "", "EnemyState", "Despawn"};

};

struct Enemy : Unity::Behaviour
{
    inline operator bool() const { return !Null(); }

    FIELD(EnemyRigidbody, Rigidbody);
    FIELD(Transform, CenterTransform);
    FIELD(EnemyHealth, Health);
    FIELD(int, CurrentState);

private:
    static inline HaxSdk::FieldOffset s_Rigidbody{"Assembly-CSharp", "", "Enemy", "Rigidbody"};
    static inline HaxSdk::FieldOffset s_CenterTransform{"Assembly-CSharp", "", "Enemy", "CenterTransform"};
    static inline HaxSdk::FieldOffset s_Health{"Assembly-CSharp", "", "Enemy", "Health"};
    static inline HaxSdk::FieldOffset s_CurrentState{"Assembly-CSharp", "", "Enemy", "CurrentState"};
};

struct EnemyParent : Behaviour
{
    EnemyParent() : Behaviour(nullptr) {}
    explicit EnemyParent(unsafe::Object* ptr) : Behaviour(ptr) {}

    inline operator bool() const { return !Null(); }
    inline bool operator==(const EnemyParent& e) const { return m_Pointer == e.m_Pointer; }

    FIELD(Enemy, enemy);
    FIELD(String, enemyName);
    FIELD(bool, Spawned);

private:
    static inline HaxSdk::FieldOffset s_enemy{"Assembly-CSharp", "", "EnemyParent", "Enemy"};
    static inline HaxSdk::FieldOffset s_Spawned{"Assembly-CSharp", "", "EnemyParent", "Spawned"};
    static inline HaxSdk::FieldOffset s_enemyName{"Assembly-CSharp", "", "EnemyParent", "enemyName"};
};

struct EnemySetup : Unity::Object
{
    EnemySetup() : Unity::Object(nullptr) {}

    inline operator bool() const { return !Null(); }
    inline EnemySetup& operator=(const Unity::Object& o) { m_Pointer = o.GetPointer(); return *this; }

    FIELD(List<GameObject>, spawnObjects);

private:
    static inline HaxSdk::FieldOffset s_spawnObjects{"Assembly-CSharp", "", "EnemySetup", "spawnObjects"};
};

struct EnemyDirector : Unity::Behaviour
{
    inline operator bool() const { return !Null(); }

    STATIC_FIELD(EnemyDirector, instance);

    FIELD(List<EnemyParent>, enemiesSpawned); 
    FIELD(List<EnemySetup>, enemiesDifficulty1);
    FIELD(List<EnemySetup>, enemiesDifficulty2);
    FIELD(List<EnemySetup>, enemiesDifficulty3);
    FIELD(bool, debugNoVision);
    FIELD(bool, debugNoGrabMaxTime);
    FIELD(bool, debugEasyGrab);

private:
    static inline HaxSdk::StaticField s_instance{"Assembly-CSharp", "", "EnemyDirector", "instance"};
    static inline HaxSdk::FieldOffset s_enemiesSpawned{"Assembly-CSharp", "", "EnemyDirector", "enemiesSpawned"};
    static inline HaxSdk::FieldOffset s_enemiesDifficulty1{"Assembly-CSharp", "", "EnemyDirector", "enemiesDifficulty1"};
    static inline HaxSdk::FieldOffset s_enemiesDifficulty2{"Assembly-CSharp", "", "EnemyDirector", "enemiesDifficulty2"};
    static inline HaxSdk::FieldOffset s_enemiesDifficulty3{"Assembly-CSharp", "", "EnemyDirector", "enemiesDifficulty3"};
    static inline HaxSdk::FieldOffset s_debugNoVision{"Assembly-CSharp", "", "EnemyDirector", "debugNoVision"};
    static inline HaxSdk::FieldOffset s_debugNoGrabMaxTime{"Assembly-CSharp", "", "EnemyDirector", "debugNoGrabMaxTime"};
    static inline HaxSdk::FieldOffset s_debugEasyGrab{"Assembly-CSharp", "", "EnemyDirector", "debugEasyGrab"};
};

struct Level : Unity::Object
{
    inline operator bool() const { return !Null(); }
    inline bool operator==(const Level& o) const { return m_Pointer == o.m_Pointer; }

    FIELD(String, NarrativeName);
private:
    static inline HaxSdk::FieldOffset s_NarrativeName{"Assembly-CSharp", "", "Level", "NarrativeName"};
};

struct RunManager : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    STATIC_FIELD(RunManager, instance);

    FIELD(List<Level>, levels);
    FIELD(Level, levelCurrent);
    FIELD(Level, levelMainMenu);
    FIELD(Level, levelLobbyMenu);
    FIELD(Level, levelShop);
    FIELD(Level, previousRunLevel);
    FIELD(int, levelsCompleted);

    void ChangeLevel(bool completedLevel, bool levelFailed, int changeLevelType = 0)
    {
        CHECK_NULL();
        s_ChangeLevel.Thunk<void, RunManager, bool, bool, int>(*this, completedLevel, levelFailed, changeLevelType);
    }

private:
    static inline HaxSdk::StaticField s_instance{"Assembly-CSharp", "", "RunManager", "instance"};
    static inline HaxSdk::FieldOffset s_levels{"Assembly-CSharp", "", "RunManager", "levels"};
    static inline HaxSdk::FieldOffset s_levelCurrent{"Assembly-CSharp", "", "RunManager", "levelCurrent"};
    static inline HaxSdk::FieldOffset s_levelMainMenu{"Assembly-CSharp", "", "RunManager", "levelMainMenu"};
    static inline HaxSdk::FieldOffset s_levelLobbyMenu{"Assembly-CSharp", "", "RunManager", "levelLobbyMenu"};
    static inline HaxSdk::FieldOffset s_levelShop{"Assembly-CSharp", "", "RunManager", "levelShop"};
    static inline HaxSdk::FieldOffset s_previousRunLevel{"Assembly-CSharp", "", "RunManager", "previousRunLevel"};
    static inline HaxSdk::FieldOffset s_levelsCompleted{"Assembly-CSharp", "", "RunManager", "levelsCompleted"};
public:
    static inline MethodInfo s_ChangeLevel{"Assembly-CSharp", "", "RunManager", "ChangeLevel"};
    static inline MethodInfo s_SetRunLevel{"Assembly-CSharp", "", "RunManager", "SetRunLevel"};
};

struct PlayerHealth : MonoBehaviour
{
    inline bool operator==(const PlayerHealth& o) const { return m_Pointer == o.m_Pointer; }
    inline operator bool() const { return !Null(); }

    FIELD(int, maxHealth);
    FIELD(int, health);

    void HealOther(int healAmount, bool effect)
    {
        CHECK_NULL();
        s_HealOther.Thunk<void, PlayerHealth, int, bool>(*this, healAmount, effect);
    }

private:
    static inline HaxSdk::FieldOffset s_maxHealth{ "Assembly-CSharp", "", "PlayerHealth", "maxHealth" };
    static inline HaxSdk::FieldOffset s_health{ "Assembly-CSharp", "", "PlayerHealth", "health" };
public:
    static inline MethodInfo s_HealOther{"Assembly-CSharp", "", "PlayerHealth", "HealOther"};
    static inline MethodInfo s_Hurt{"Assembly-CSharp", "", "PlayerHealth", "Hurt"};
};

struct MapCustomEntity : MonoBehaviour
{
    inline operator bool() const { return !Null(); }
};

struct MapCustom : MonoBehaviour
{
    explicit MapCustom(unsafe::Object* ptr) : MonoBehaviour(ptr) {}

    inline operator bool() const { return !Null(); }

    FIELD(Sprite, sprite);
    FIELD(Color, color);
    FIELD(MapCustomEntity, mapCustomEntity);

    void Hide()
    {
        CHECK_NULL();
        s_Hide.Thunk<void, MapCustom>(*this);
    }

private:
    static inline HaxSdk::FieldOffset s_sprite{"Assembly-CSharp", "", "MapCustom", "sprite"};
    static inline HaxSdk::FieldOffset s_color{"Assembly-CSharp", "", "MapCustom", "color"};
    static inline HaxSdk::FieldOffset s_mapCustomEntity{"Assembly-CSharp", "", "MapCustom", "mapCustomEntity"};
public:
    static inline MethodInfo s_Hide{"Assembly-CSharp", "", "MapCustom", "Hide"};
};

struct PlayerDeathHead : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    FIELD(MapCustom, mapCustom);

private:
    static inline HaxSdk::FieldOffset s_mapCustom{"Assembly-CSharp", "", "PlayerDeathHead", "mapCustom"};
};

struct PhysGrabber : MonoBehaviour
{
    PhysGrabber(unsafe::Object* ptr) : MonoBehaviour(ptr) {}

    inline bool operator==(const PhysGrabber& o) const { return m_Pointer == o.m_Pointer; }
    inline operator bool() const { return !Null(); }

    FIELD(float, grabRange);
    FIELD(float, grabStrength);
    FIELD(float, throwStrength);

    void PhysGrabOverChargeImpact()
    {
        CHECK_NULL();
        s_PhysGrabOverChargeImpact.Thunk<void, PhysGrabber>(*this);
    }

private:
    static inline HaxSdk::FieldOffset s_grabRange{"Assembly-CSharp", "", "PhysGrabber", "grabRange"};
    static inline HaxSdk::FieldOffset s_grabStrength{ "Assembly-CSharp", "", "PhysGrabber", "grabStrength" };
    static inline HaxSdk::FieldOffset s_throwStrength{ "Assembly-CSharp", "", "PhysGrabber", "throwStrength" };
public:
    static inline MethodInfo s_PhysGrabLogic{"Assembly-CSharp", "", "PhysGrabber", "PhysGrabLogic"};
    static inline MethodInfo s_RayCheck{"Assembly-CSharp", "", "PhysGrabber", "RayCheck"};
    static inline MethodInfo s_PhysGrabOverChargeImpact{"Assembly-CSharp", "", "PhysGrabber", "PhysGrabOverChargeImpact"};
    static inline MethodInfo s_PhysGrabOverCharge{ "Assembly-CSharp", "", "PhysGrabber", "PhysGrabOverCharge" };
};

struct PlayerVoiceChat : MonoBehaviour
{
    PlayerVoiceChat() : MonoBehaviour(nullptr) {}
    inline operator bool() const { return !Null(); }

    FIELD(bool, inLobbyMixer);

    void ToggleLobby(bool toggle)
    {
        CHECK_NULL();
        s_ToggleLobby.Thunk<void, PlayerVoiceChat, bool>(*this, toggle);
    }

private:
    static inline HaxSdk::FieldOffset s_inLobbyMixer{"Assembly-CSharp", "", "PlayerVoiceChat", "inLobbyMixer"};
public:
    static inline MethodInfo s_ToggleLobby{"Assembly-CSharp", "", "PlayerVoiceChat", "ToggleLobby"};
};

struct PlayerTumble : MonoBehaviour
{
    inline operator bool() { return !Null(); }

    FIELD(int, tumbleLaunch);

private:
    static inline HaxSdk::FieldOffset s_tumbleLaunch{ "Assembly-CSharp", "", "PlayerTumble", "tumbleLaunch" };
public:
    static inline MethodInfo s_TumbleRequest{"Assembly-CSharp", "", "PlayerTumble", "TumbleRequest"};
};

struct PlayerAvatar : MonoBehaviour
{
    PlayerAvatar() : MonoBehaviour(nullptr) {}
    PlayerAvatar(unsafe::Object* ptr) : MonoBehaviour(ptr) {}

    inline operator bool() { return !Null(); }
    PlayerAvatar& operator=(const System::Object& o) { m_Pointer = o.GetPointer(); return *this; }

    STATIC_FIELD(PlayerAvatar, instance);

    FIELD(PlayerHealth, playerHealth);
    FIELD(bool, isLocal);
    FIELD(bool, deadSet);
    FIELD(PlayerDeathHead, playerDeathHead);
    FIELD(PhysGrabber, physGrabber);
    FIELD(String, playerName);
    FIELD(PlayerVoiceChat, voiceChat);
    FIELD(bool, spawned);
    FIELD(Photon::PhotonView, photonView);
    FIELD(PlayerTumble, tumble);
    FIELD(int, upgradeMapPlayerCount);

    void PlayerDeath(int enemyIndex)
    {
        CHECK_NULL();
        s_PlayerDeath.Invoke<void, PlayerAvatar, int>(*this, enemyIndex);
    }

    void Revive(bool revivedByTruck = false)
    {
        CHECK_NULL();
        s_Revive.Invoke<void, PlayerAvatar, bool>(*this, revivedByTruck);
    }

private:
    static inline HaxSdk::StaticField s_instance{"Assembly-CSharp", "", "PlayerAvatar", "instance"};
    static inline HaxSdk::FieldOffset s_playerHealth{"Assembly-CSharp", "", "PlayerAvatar", "playerHealth"};
    static inline HaxSdk::FieldOffset s_isLocal{"Assembly-CSharp", "", "PlayerAvatar", "isLocal"};
    static inline HaxSdk::FieldOffset s_deadSet{"Assembly-CSharp", "", "PlayerAvatar", "deadSet"};
    static inline HaxSdk::FieldOffset s_playerDeathHead{"Assembly-CSharp", "", "PlayerAvatar", "playerDeathHead"};
    static inline HaxSdk::FieldOffset s_physGrabber{"Assembly-CSharp", "", "PlayerAvatar", "physGrabber"};
    static inline HaxSdk::FieldOffset s_playerName{"Assembly-CSharp", "", "PlayerAvatar", "playerName"};
    static inline HaxSdk::FieldOffset s_voiceChat{"Assembly-CSharp", "", "PlayerAvatar", "voiceChat"};
    static inline HaxSdk::FieldOffset s_spawned{"Assembly-CSharp", "", "PlayerAvatar", "spawned"};
    static inline HaxSdk::FieldOffset s_photonView{"Assembly-CSharp", "", "PlayerAvatar", "photonView"};
    static inline HaxSdk::FieldOffset s_tumble{ "Assembly-CSharp", "", "PlayerAvatar", "tumble" };
    static inline HaxSdk::FieldOffset s_upgradeMapPlayerCount{ "Assembly-CSharp", "", "PlayerAvatar", "upgradeMapPlayerCount" };
public:
    static inline MethodInfo s_OnDestroy{"Assembly-CSharp", "", "PlayerAvatar", "OnDestroy"};
    static inline MethodInfo s_PlayerDeath{"Assembly-CSharp", "", "PlayerAvatar", "PlayerDeath"};
    static inline MethodInfo s_Revive{"Assembly-CSharp", "", "PlayerAvatar", "Revive"};

};

struct SemiFunc
{
    static inline Camera MainCamera() 
    { 
        return s_MainCamera.Thunk<Camera>(); 
    }

    static inline bool OnScreen(const Vector3& pos, float paddWidth, float paddHeight) 
    { 
        return s_OnScreen.Thunk<bool, const Vector3_Boxed&, float, float>(pos.Box(), paddWidth, paddHeight); 
    }

    static inline bool IsMasterClientOrSingleplayer()
    {
        return s_IsMasterClientOrSingleplayer.Thunk<bool>();
    }

    static inline bool IsMultiplayer()
    {
        return s_IsMultiplayer.Thunk<bool>();
    }

    static inline bool IsMasterClient()
    {
        return s_IsMasterClient.Thunk<bool>();
    }

    static inline String PlayerGetSteamID(PlayerAvatar player)
    {
        return s_PlayerGetSteamID.Thunk<String, PlayerAvatar>(player);
    }

    static inline int StatSetRunCurrency(int value)
    {
        return s_StatSetRunCurrency.Thunk<int, int>(value);
    }

private:
    static inline MethodInfo s_OnScreen{"Assembly-CSharp", "", "SemiFunc", "OnScreen"};
    static inline MethodInfo s_MainCamera{"Assembly-CSharp", "", "SemiFunc", "MainCamera"};
    static inline MethodInfo s_IsMasterClientOrSingleplayer{"Assembly-CSharp", "", "SemiFunc", "IsMasterClientOrSingleplayer"};
    static inline MethodInfo s_IsMultiplayer{"Assembly-CSharp", "", "SemiFunc", "IsMultiplayer"};
    static inline MethodInfo s_IsMasterClient{"Assembly-CSharp", "", "SemiFunc", "IsMasterClient"};
    static inline MethodInfo s_PlayerGetSteamID{"Assembly-CSharp", "", "SemiFunc", "PlayerGetSteamID"};
    static inline MethodInfo s_StatSetRunCurrency{"Assembly-CSharp", "", "SemiFunc", "StatSetRunCurrency"};

};

struct GameDirector : MonoBehaviour
{
    inline operator bool() { return !Null(); }

    STATIC_FIELD(GameDirector, instance);
    FIELD(Camera, MainCamera);
    FIELD(List<PlayerAvatar>, PlayerList);
    FIELD(int, currentState);

private:
    static inline HaxSdk::StaticField s_instance{"Assembly-CSharp", "", "GameDirector", "instance"};
    static inline HaxSdk::FieldOffset s_MainCamera{"Assembly-CSharp", "", "GameDirector", "MainCamera"};
    static inline HaxSdk::FieldOffset s_currentState{"Assembly-CSharp", "", "GameDirector", "currentState"};
    static inline HaxSdk::FieldOffset s_PlayerList{"Assembly-CSharp", "", "GameDirector", "PlayerList"};
};

struct GameDirector_gameState
{
    ENUM_VALUE(Main);

private:
    static inline HaxSdk::EnumValue s_Main{"Assembly-CSharp", "", "GameDirector/gameState", "Main"};
};

struct PlayerController : MonoBehaviour
{
    inline operator bool() { return !Null(); }

    STATIC_FIELD(PlayerController, instance);

    FIELD(float, EnergyCurrent);
    FIELD(float, EnergyStart);
    FIELD(float, MoveSpeed);
    FIELD(float, SprintSpeed);
    FIELD(float, SprintSpeedUpgrades);
    FIELD(float, CrouchSpeed);
    FIELD(int, JumpExtra);

private:
    static inline HaxSdk::StaticField s_instance{ "Assembly-CSharp", "", "PlayerController", "instance" };
    static inline HaxSdk::FieldOffset s_EnergyCurrent{"Assembly-CSharp", "", "PlayerController", "EnergyCurrent"};
    static inline HaxSdk::FieldOffset s_EnergyStart{"Assembly-CSharp", "", "PlayerController", "EnergyStart"};
    static inline HaxSdk::FieldOffset s_MoveSpeed{"Assembly-CSharp", "", "PlayerController", "MoveSpeed"};
    static inline HaxSdk::FieldOffset s_SprintSpeed{"Assembly-CSharp", "", "PlayerController", "SprintSpeed"};
    static inline HaxSdk::FieldOffset s_CrouchSpeed{"Assembly-CSharp", "", "PlayerController", "CrouchSpeed"};
    static inline HaxSdk::FieldOffset s_JumpExtra{"Assembly-CSharp", "", "PlayerController", "JumpExtra"};
    static inline HaxSdk::FieldOffset s_SprintSpeedUpgrades{ "Assembly-CSharp", "", "PlayerController", "SprintSpeedUpgrades" };
public:
    static inline MethodInfo s_FixedUpdate{"Assembly-CSharp", "", "PlayerController", "FixedUpdate"};
    static inline MethodInfo s_Update{"Assembly-CSharp", "", "PlayerController", "Update"};
};

struct EnemySlowMouth : MonoBehaviour
{
    EnemySlowMouth() : MonoBehaviour(nullptr) {}
    explicit EnemySlowMouth(unsafe::Object* ptr) : MonoBehaviour(ptr) {}

    inline operator bool() { return !Null(); }

    FIELD(float, attachedTimer);

    static inline HaxSdk::FieldOffset s_attachedTimer{"Assembly-CSharp", "", "EnemySlowMouth", "attachedTimer"};
};

template <>
struct std::hash<EnemyParent>
{
    size_t operator()(const EnemyParent& e) const { return (size_t)e.GetPointer(); }
};

struct MenuManager : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    STATIC_FIELD(MenuManager, instance);

    FIELD(Component, currentMenuPage);

private:
    static inline HaxSdk::StaticField s_instance{"Assembly-CSharp", "", "MenuManager", "instance"};
    static inline HaxSdk::FieldOffset s_currentMenuPage{"Assembly-CSharp", "", "MenuManager", "currentMenuPage"};
};

struct RoomVolume : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    void SetExplored()
    {
        CHECK_NULL();
        s_SetExplored.Thunk<void, RoomVolume>(*this);
    }

    static inline MethodInfo s_SetExplored{"Assembly-CSharp", "", "RoomVolume", "SetExplored"};
};

struct LevelPoint : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    FIELD(RoomVolume, Room);

private:
    static inline HaxSdk::FieldOffset s_Room{"Assembly-CSharp", "", "LevelPoint", "Room"};
};

struct LevelGenerator : MonoBehaviour
{
    explicit LevelGenerator(unsafe::Object* ptr) : MonoBehaviour(ptr) {}

    inline operator bool() const { return !Null(); }
    inline LevelGenerator& operator=(const LevelGenerator& o) { m_Pointer = o.m_Pointer; return *this; }

    STATIC_FIELD(LevelGenerator, Instance);

    FIELD(List<LevelPoint>, LevelPathPoints);
    FIELD(bool, Generated);

    void EnemySpawn(EnemySetup enemySetup, Vector3 position)
    {
        CHECK_NULL();
        s_EnemySpawn.Thunk<void, LevelGenerator, EnemySetup, const Vector3_Boxed&>(*this, enemySetup, position.Box());
    }

private:
    static inline HaxSdk::StaticField s_Instance{"Assembly-CSharp", "", "LevelGenerator", "Instance"};
    static inline HaxSdk::FieldOffset s_LevelPathPoints{"Assembly-CSharp", "", "LevelGenerator", "LevelPathPoints"};
    static inline HaxSdk::FieldOffset s_Generated{"Assembly-CSharp", "", "LevelGenerator", "Generated"};
public:
    static inline MethodInfo s_EnemySpawn{"Assembly-CSharp", "", "LevelGenerator", "EnemySpawn"};
};

struct ExtractionPoint : MonoBehaviour
{
    explicit ExtractionPoint(unsafe::Object* ptr) : MonoBehaviour(ptr) {}

    inline operator bool() const { return !Null(); }

    FIELD(bool, isLocked);
    FIELD(int, currentState);

    void HaulGoalSet(int value)
    {
        CHECK_NULL();
        s_HaulGoalSet.Thunk<void, ExtractionPoint, int>(*this, value);
    }

    void OnClick()
    {
        CHECK_NULL();
        s_OnClick.Thunk<void, ExtractionPoint>(*this);
    }

private:
    static inline HaxSdk::FieldOffset s_isLocked{"Assembly-CSharp", "", "ExtractionPoint", "isLocked"};
    static inline HaxSdk::FieldOffset s_currentState{"Assembly-CSharp", "", "ExtractionPoint", "currentState"};
public:
    static inline MethodInfo s_HaulGoalSet{"Assembly-CSharp", "", "ExtractionPoint", "HaulGoalSet"};
    static inline MethodInfo s_OnClick{"Assembly-CSharp", "", "ExtractionPoint", "OnClick"};
};

struct RoundDirector : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    STATIC_FIELD(RoundDirector, instance);

    FIELD(bool, extractionPointActive);
    FIELD(ExtractionPoint, extractionPointCurrent);
    FIELD(List<GameObject>, extractionPointList);

    void ExtractionPointsUnlock()
    {
        CHECK_NULL();
        s_ExtractionPointsUnlock.Thunk<void, RoundDirector>(*this);
    }

private:
    static inline HaxSdk::StaticField s_instance{"Assembly-CSharp", "", "RoundDirector", "instance"};
    static inline HaxSdk::FieldOffset s_extractionPointActive{"Assembly-CSharp", "", "RoundDirector", "extractionPointActive"};
    static inline HaxSdk::FieldOffset s_extractionPointCurrent{"Assembly-CSharp", "", "RoundDirector", "extractionPointCurrent"};
    static inline HaxSdk::FieldOffset s_extractionPointList{"Assembly-CSharp", "", "RoundDirector", "extractionPointList"};
public:
    static inline MethodInfo s_ExtractionPointsUnlock{"Assembly-CSharp", "", "RoundDirector", "ExtractionPointsUnlock"};
};

struct ExtractionPoint_State
{
    ENUM_VALUE(Complete);
    ENUM_VALUE(Active);
    ENUM_VALUE(Idle);

private:
    static inline HaxSdk::EnumValue s_Complete{"Assembly-CSharp", "", "ExtractionPoint/State", "Complete"};
    static inline HaxSdk::EnumValue s_Active{"Assembly-CSharp", "", "ExtractionPoint/State", "Active"};
    static inline HaxSdk::EnumValue s_Idle{"Assembly-CSharp", "", "ExtractionPoint/State", "Idle"};
};

struct PhysGrabObjectImpactDetector : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    FIELD(bool, isValuable);
    FIELD(float, impactDisabledTimer);

private:
    static inline HaxSdk::FieldOffset s_isValuable{"Assembly-CSharp", "", "PhysGrabObjectImpactDetector", "isValuable"};
    static inline HaxSdk::FieldOffset s_impactDisabledTimer{"Assembly-CSharp", "", "PhysGrabObjectImpactDetector", "impactDisabledTimer"};
public:
    static inline MethodInfo s_FixedUpdate{"Assembly-CSharp", "", "PhysGrabObjectImpactDetector", "FixedUpdate"};
};

struct ValuableObject : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    FIELD(PhysGrabObject, physGrabObject);
    FIELD(float, dollarValueCurrent);

    void Discover(int state)
    {
        CHECK_NULL();
        s_Discover.Thunk<void, ValuableObject, int>(*this, state);
    }

private:
    static inline HaxSdk::FieldOffset s_physGrabObject{"Assembly-CSharp", "", "ValuableObject", "physGrabObject"};
    static inline HaxSdk::FieldOffset s_dollarValueCurrent{"Assembly-CSharp", "", "ValuableObject", "dollarValueCurrent"};
public:
    static inline MethodInfo s_Discover{"Assembly-CSharp", "", "ValuableObject", "Discover"};
};

struct ValuableDirector : MonoBehaviour
{
    STATIC_FIELD(ValuableDirector, instance);

    FIELD(List<ValuableObject>, valuableList);

private:
    static inline HaxSdk::StaticField s_instance{"Assembly-CSharp", "", "ValuableDirector", "instance"};
    static inline HaxSdk::FieldOffset s_valuableList{"Assembly-CSharp", "", "ValuableDirector", "valuableList"};
};

struct ValuableDiscoverGraphic_State
{
    ENUM_VALUE(Discover);

private:
    static inline HaxSdk::EnumValue s_Discover{"Assembly-CSharp", "", "ValuableDiscoverGraphic/State", "Discover"};
};

struct Item : Unity::Object
{
    Item() : Unity::Object(nullptr) {}
    Item(unsafe::Object* ptr) : Unity::Object(ptr) {}

    inline operator bool() const { return !Null(); }
    inline Item& operator=(const Unity::Object& o) { m_Pointer = o.GetPointer(); return *this; }

    FIELD(GameObject, prefab);

private:
    static inline HaxSdk::FieldOffset s_prefab{"Assembly-CSharp", "", "Item", "prefab"};
};

#define COMMA ,
struct StatsManager : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    STATIC_FIELD(StatsManager, instance);

    FIELD(Dictionary<String COMMA Item>, itemDictionary);
    FIELD(Dictionary<String COMMA int>, playerUpgradeStrength);
    FIELD(Dictionary<String COMMA int>, playerUpgradeThrow);
    FIELD(Dictionary<String COMMA int>, playerUpgradeStamina);
    FIELD(Dictionary<String COMMA int>, playerUpgradeSpeed);
    FIELD(Dictionary<String COMMA int>, playerUpgradeLaunch);
    FIELD(Dictionary<String COMMA int>, playerUpgradeHealth);
    FIELD(Dictionary<String COMMA int>, playerUpgradeMapPlayerCount);
    FIELD(Dictionary<String COMMA int>, playerUpgradeExtraJump);
private:
    static inline HaxSdk::StaticField s_instance{"Assembly-CSharp", "", "StatsManager", "instance"};
    static inline HaxSdk::FieldOffset s_itemDictionary{"Assembly-CSharp", "", "StatsManager", "itemDictionary"};
    static inline HaxSdk::FieldOffset s_playerUpgradeStrength{"Assembly-CSharp", "", "StatsManager", "playerUpgradeStrength"};
    static inline HaxSdk::FieldOffset s_playerUpgradeThrow{"Assembly-CSharp", "", "StatsManager", "playerUpgradeThrow"};
    static inline HaxSdk::FieldOffset s_playerUpgradeStamina{"Assembly-CSharp", "", "StatsManager", "playerUpgradeStamina"};
    static inline HaxSdk::FieldOffset s_playerUpgradeSpeed{"Assembly-CSharp", "", "StatsManager", "playerUpgradeSpeed"};
    static inline HaxSdk::FieldOffset s_playerUpgradeLaunch{"Assembly-CSharp", "", "StatsManager", "playerUpgradeLaunch"};
    static inline HaxSdk::FieldOffset s_playerUpgradeHealth{"Assembly-CSharp", "", "StatsManager", "playerUpgradeHealth"};
    static inline HaxSdk::FieldOffset s_playerUpgradeMapPlayerCount{"Assembly-CSharp", "", "StatsManager", "playerUpgradeMapPlayerCount"};
    static inline HaxSdk::FieldOffset s_playerUpgradeExtraJump{"Assembly-CSharp", "", "StatsManager", "playerUpgradeExtraJump"};
};

struct PunManager : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    STATIC_FIELD(PunManager, instance);
    FIELD(StatsManager, statsManager);

    int UpgradePlayerGrabStrength(String steamID)
    {
        CHECK_NULL();
        return s_UpgradePlayerGrabStrength.Thunk<int, PunManager, String>(*this, steamID);
    }

    int UpgradePlayerThrowStrength(String steamID)
    {
        CHECK_NULL();
        return s_UpgradePlayerThrowStrength.Thunk<int, PunManager, String>(*this, steamID);
    }

    int UpgradePlayerEnergy(String steamID)
    {
        CHECK_NULL();
        return s_UpgradePlayerEnergy.Thunk<int, PunManager, String>(*this, steamID);
    }

    int UpgradePlayerSprintSpeed(String steamID)
    {
        CHECK_NULL();
        return s_UpgradePlayerSprintSpeed.Thunk<int, PunManager, String>(*this, steamID);
    }

    int UpgradePlayerTumbleLaunch(String steamID)
    {
        CHECK_NULL();
        return s_UpgradePlayerTumbleLaunch.Thunk<int, PunManager, String>(*this, steamID);
    }

    int UpgradePlayerHealth(String steamID)
    {
        CHECK_NULL();
        return s_UpgradePlayerHealth.Thunk<int, PunManager, String>(*this, steamID);
    }

    int UpgradeMapPlayerCount(String steamID)
    {
        CHECK_NULL();
        return s_UpgradeMapPlayerCount.Thunk<int, PunManager, String>(*this, steamID);
    }

    int UpgradePlayerExtraJump(String steamID)
    {
        CHECK_NULL();
        return s_UpgradePlayerExtraJump.Thunk<int, PunManager, String>(*this, steamID);
    }

private:
    static inline HaxSdk::StaticField s_instance{"Assembly-CSharp", "", "PunManager", "instance"};
    static inline HaxSdk::FieldOffset s_statsManager{"Assembly-CSharp", "", "PunManager", "statsManager"};
private:
    static inline MethodInfo s_UpgradePlayerGrabStrength{"Assembly-CSharp", "", "PunManager", "UpgradePlayerGrabStrength"};
    static inline MethodInfo s_UpgradePlayerThrowStrength{"Assembly-CSharp", "", "PunManager", "UpgradePlayerThrowStrength"};
    static inline MethodInfo s_UpgradePlayerEnergy{"Assembly-CSharp", "", "PunManager", "UpgradePlayerEnergy"};
    static inline MethodInfo s_UpgradePlayerSprintSpeed{"Assembly-CSharp", "", "PunManager", "UpgradePlayerSprintSpeed"};
    static inline MethodInfo s_UpgradePlayerTumbleLaunch{"Assembly-CSharp", "", "PunManager", "UpgradePlayerTumbleLaunch"};
    static inline MethodInfo s_UpgradePlayerHealth{"Assembly-CSharp", "", "PunManager", "UpgradePlayerHealth"};
    static inline MethodInfo s_UpgradeMapPlayerCount{"Assembly-CSharp", "", "PunManager", "UpgradeMapPlayerCount"};
    static inline MethodInfo s_UpgradePlayerExtraJump{"Assembly-CSharp", "", "PunManager", "UpgradePlayerExtraJump"};
};

struct ItemBattery : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    FIELD(float, batteryLife);
    FIELD(float, batteryLifeInt);
    FIELD(int, batteryBars);
    FIELD(PhysGrabObject, physGrabObject);

    void SetBatteryLife(int batteryLife)
    {
        CHECK_NULL();
        s_SetBatteryLife.Thunk<void(*)(ItemBattery, int)>(*this, batteryLife);
    }

    void BatteryFullPercentChange(int batteryLifeInt, bool charge = false)
    {
        CHECK_NULL();
        s_BatteryFullPercentChange.Thunk<void, ItemBattery, int, bool>(*this, batteryLifeInt, charge);
    }

private:
    static inline HaxSdk::FieldOffset s_batteryLife{"Assembly-CSharp", "", "ItemBattery", "batteryLife"};
    static inline HaxSdk::FieldOffset s_batteryLifeInt{"Assembly-CSharp", "", "ItemBattery", "batteryLifeInt"};
    static inline HaxSdk::FieldOffset s_batteryBars{ "Assembly-CSharp", "", "ItemBattery", "batteryBars" };
    static inline HaxSdk::FieldOffset s_physGrabObject{ "Assembly-CSharp", "", "ItemBattery", "physGrabObject" };

public:
    static inline MethodInfo s_Update{"Assembly-CSharp", "", "ItemBattery", "Update"};
    static inline MethodInfo s_SetBatteryLife{"Assembly-CSharp", "", "ItemBattery", "SetBatteryLife"};
    static inline MethodInfo s_BatteryFullPercentChange{ "Assembly-CSharp", "", "ItemBattery", "BatteryFullPercentChange" };

};

struct ItemGun : MonoBehaviour
{
    ItemGun() : MonoBehaviour(nullptr) {}

    inline operator bool() const { return !Null(); }
    inline bool operator==(const ItemGun& o) const { return m_Pointer == o.m_Pointer; }

    FIELD(float, gunRandomSpread);
    FIELD(PhysGrabObject, physGrabObject);
    FIELD(Transform, gunMuzzle);
    FIELD(float, gunRange);

private:
    static inline HaxSdk::FieldOffset s_gunRandomSpread{"Assembly-CSharp", "", "ItemGun", "gunRandomSpread"};
    static inline HaxSdk::FieldOffset s_physGrabObject{"Assembly-CSharp", "", "ItemGun", "physGrabObject"};
    static inline HaxSdk::FieldOffset s_gunMuzzle{"Assembly-CSharp", "", "ItemGun", "gunMuzzle"};
    static inline HaxSdk::FieldOffset s_gunRange{"Assembly-CSharp", "", "ItemGun", "gunRange"};
public:
    static inline MethodInfo s_Shoot{"Assembly-CSharp", "", "ItemGun", "Shoot"};
    static inline MethodInfo s_Update{"Assembly-CSharp", "", "ItemGun", "Update"};
};

struct DataDirector : MonoBehaviour
{
    inline operator bool() const { return !Null(); }
public:
    static inline MethodInfo s_SaveDeleteCheck{"Assembly-CSharp", "", "DataDirector", "SaveDeleteCheck"};
};

template <>
struct std::hash<ItemGun>
{
    size_t operator()(const ItemGun& e) const { return (size_t)e.GetPointer(); }
};

struct TruckSafetySpawnPoint : MonoBehaviour
{
    inline operator bool() const { return !Null(); }

    STATIC_FIELD(TruckSafetySpawnPoint, instance);
private:
    static inline HaxSdk::StaticField s_instance{"Assembly-CSharp", "", "TruckSafetySpawnPoint", "instance"};
};

// Hooks
static inline MethodInfo Camera_FireOnPreRender{"UnityEngine.CoreModule", "UnityEngine", "Camera", "FireOnPreRender"};
static inline MethodInfo EventSystem_Update{"UnityEngine.UI", "UnityEngine.EventSystems", "EventSystem", "Update"};
static inline MethodInfo PhotonNetwork__IsMasterClient{"PhotonUnityNetworking", "Photon.Pun", "PhotonNetwork", "get_IsMasterClient"};
static inline MethodInfo Physics__Raycast{"UnityEngine.PhysicsModule", "UnityEngine", "Physics", "Raycast", "System.Boolean(UnityEngine.Vector3,UnityEngine.Vector3,UnityEngine.RaycastHit&,System.Single,System.Int32,UnityEngine.QueryTriggerInteraction)"};
static inline MethodInfo PhotonNetwork__ExecuteRpc{"PhotonUnityNetworking", "Photon.Pun", "PhotonNetwork", "ExecuteRpc"};
static inline MethodInfo PhotonMessageInfo__ctor{"PhotonUnityNetworking", "Photon.Pun", "PhotonMessageInfo", ".ctor"};
// Types
template <>
inline System::Type typeof<EnemyParent> = System::Type("Assembly-CSharp", "", "EnemyParent");

template <>
inline System::Type typeof<EnemySlowMouth> = System::Type("Assembly-CSharp", "", "EnemySlowMouth");

template <>
inline System::Type typeof<MapCustom> = System::Type("Assembly-CSharp", "", "MapCustom");

template <>
inline System::Type typeof<ExtractionPoint> = System::Type("Assembly-CSharp", "", "ExtractionPoint");

template <>
inline System::Type typeof<PhysGrabber> = System::Type("Assembly-CSharp", "", "PhysGrabber");
