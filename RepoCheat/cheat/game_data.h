
#ifdef HAX_STATIC_FIELD
HAX_STATIC_FIELD(PlayerAvatar, PlayerAvatar*, instance, MAIN_ASSEMBLY, "", "PlayerAvatar", "instance");

HAX_STATIC_FIELD(EnemyDirector, EnemyDirector*, instance, MAIN_ASSEMBLY, "", "EnemyDirector", "instance");

HAX_STATIC_FIELD(LevelGenerator, LevelGenerator*, Instance, MAIN_ASSEMBLY, "", "LevelGenerator", "Instance");

HAX_STATIC_FIELD(ValuableDirector, ValuableDirector*, instance, MAIN_ASSEMBLY, "", "ValuableDirector", "instance");

HAX_STATIC_FIELD(RoundDirector, RoundDirector*, instance, MAIN_ASSEMBLY, "", "RoundDirector", "instance");

HAX_STATIC_FIELD(StatsManager, StatsManager*, instance, MAIN_ASSEMBLY, "", "StatsManager", "instance");

HAX_STATIC_FIELD(RunManager, RunManager*, instance, MAIN_ASSEMBLY, "", "RunManager", "instance");

HAX_STATIC_FIELD(PunManager, PunManager*, instance, MAIN_ASSEMBLY, "", "PunManager", "instance");

HAX_STATIC_FIELD(GameDirector, GameDirector*, instance, MAIN_ASSEMBLY, "", "GameDirector", "instance");

HAX_STATIC_FIELD(Map, Map*, Instance, MAIN_ASSEMBLY, "", "Map", "Instance");

HAX_STATIC_FIELD(TruckSafetySpawnPoint, TruckSafetySpawnPoint*, instance, MAIN_ASSEMBLY, "", "TruckSafetySpawnPoint", "instance");

HAX_STATIC_FIELD(MenuManager, MenuManager*, instance, MAIN_ASSEMBLY, "", "MenuManager", "instance");

#undef HAX_STATIC_FIELD
#endif

#ifdef HAX_FIELD
HAX_FIELD(PlayerAvatar, PlayerHealth*, playerHealth, MAIN_ASSEMBLY, "", "PlayerAvatar", "playerHealth");
HAX_FIELD(PlayerAvatar, bool, isLocal, MAIN_ASSEMBLY, "", "PlayerAvatar", "isLocal");
HAX_FIELD(PlayerAvatar, PlayerDeathHead*, playerDeathHead, MAIN_ASSEMBLY, "", "PlayerAvatar", "playerDeathHead");
HAX_FIELD(PlayerAvatar, PhysGrabber*, physGrabber, MAIN_ASSEMBLY, "", "PlayerAvatar", "physGrabber");
HAX_FIELD(PlayerAvatar, System::String*, playerName, MAIN_ASSEMBLY, "", "PlayerAvatar", "playerName");
HAX_FIELD(PlayerAvatar, bool, deadSet, MAIN_ASSEMBLY, "", "PlayerAvatar", "deadSet");
HAX_FIELD(PlayerAvatar, PlayerVoiceChat*, voiceChat, MAIN_ASSEMBLY, "", "PlayerAvatar", "voiceChat");
HAX_FIELD(PlayerAvatar, bool, spawned, MAIN_ASSEMBLY, "", "PlayerAvatar", "spawned");

HAX_FIELD(PlayerHealth, PlayerAvatar*, playerAvatar, MAIN_ASSEMBLY, "", "PlayerHealth", "playerAvatar");

HAX_FIELD(PlayerController, float, EnergyCurrent, MAIN_ASSEMBLY, "", "PlayerController", "EnergyCurrent");
HAX_FIELD(PlayerController, float, EnergyStart, MAIN_ASSEMBLY, "", "PlayerController", "EnergyStart");
HAX_FIELD(PlayerController, float, MoveSpeed, MAIN_ASSEMBLY, "", "PlayerController", "MoveSpeed");
HAX_FIELD(PlayerController, float, SprintSpeed, MAIN_ASSEMBLY, "", "PlayerController", "SprintSpeed");
HAX_FIELD(PlayerController, float, CrouchSpeed, MAIN_ASSEMBLY, "", "PlayerController", "CrouchSpeed");
HAX_FIELD(PlayerController, Int32, JumpExtra, MAIN_ASSEMBLY, "", "PlayerController", "JumpExtra");

HAX_FIELD(EnemyDirector, System::List<EnemyParent*>*, enemiesSpawned, MAIN_ASSEMBLY, "", "EnemyDirector", "enemiesSpawned");
HAX_FIELD(EnemyDirector, System::List<EnemySetup*>*, enemiesDifficulty1, MAIN_ASSEMBLY, "", "EnemyDirector", "enemiesDifficulty1");
HAX_FIELD(EnemyDirector, System::List<EnemySetup*>*, enemiesDifficulty2, MAIN_ASSEMBLY, "", "EnemyDirector", "enemiesDifficulty2");
HAX_FIELD(EnemyDirector, System::List<EnemySetup*>*, enemiesDifficulty3, MAIN_ASSEMBLY, "", "EnemyDirector", "enemiesDifficulty3");
HAX_FIELD(EnemyDirector, bool, debugNoVision, MAIN_ASSEMBLY, "", "EnemyDirector", "debugNoVision");
HAX_FIELD(EnemyDirector, bool, debugNoGrabMaxTime, MAIN_ASSEMBLY, "", "EnemyDirector", "debugNoGrabMaxTime");
HAX_FIELD(EnemyDirector, bool, debugEasyGrab, MAIN_ASSEMBLY, "", "EnemyDirector", "debugEasyGrab");

HAX_FIELD(EnemyParent, bool, Spawned, MAIN_ASSEMBLY, "", "EnemyParent", "Spawned");
HAX_FIELD(EnemyParent, Enemy*, enemy, MAIN_ASSEMBLY, "", "EnemyParent", "Enemy");
HAX_FIELD(EnemyParent, System::String*, enemyName, MAIN_ASSEMBLY, "", "EnemyParent", "enemyName");

HAX_FIELD(Enemy, EnemyRigidbody*, Rigidbody, MAIN_ASSEMBLY, "", "Enemy", "Rigidbody");
HAX_FIELD(Enemy, Unity::Transform*, CenterTransform, MAIN_ASSEMBLY, "", "Enemy", "CenterTransform");
HAX_FIELD(Enemy, EnemyHealth*, Health, MAIN_ASSEMBLY, "", "Enemy", "Health");
HAX_FIELD(Enemy, Int32, CurrentState, MAIN_ASSEMBLY, "", "Enemy", "CurrentState");

HAX_FIELD(EnemySlowMouth, float, attachedTimer, MAIN_ASSEMBLY, "", "EnemySlowMouth", "attachedTimer");

HAX_FIELD(EnemyRigidbody, PhysGrabObject*, physGrabObject, MAIN_ASSEMBLY, "", "EnemyRigidbody", "physGrabObject");
HAX_FIELD(EnemyRigidbody, float, grabShakeReleaseTimer, MAIN_ASSEMBLY, "", "EnemyRigidbody", "grabShakeReleaseTimer");
HAX_FIELD(EnemyRigidbody, float, grabStrengthTimer, MAIN_ASSEMBLY, "", "EnemyRigidbody", "grabStrengthTimer");
HAX_FIELD(EnemyRigidbody, float, grabStrengthTime, MAIN_ASSEMBLY, "", "EnemyRigidbody", "grabStrengthTime");

HAX_FIELD(EnemyHealth, Int32, healthCurrent, MAIN_ASSEMBLY, "", "EnemyHealth", "healthCurrent");
HAX_FIELD(EnemyHealth, Int32, health, MAIN_ASSEMBLY, "", "EnemyHealth", "health");
HAX_FIELD(EnemyHealth, bool, dead, MAIN_ASSEMBLY, "", "EnemyHealth", "dead");

HAX_FIELD(EnemySetup, System::List<Unity::GameObject*>*, spawnObjects, MAIN_ASSEMBLY, "", "EnemySetup", "spawnObjects");

HAX_FIELD(LevelGenerator, System::List<LevelPoint*>*, LevelPathPoints, MAIN_ASSEMBLY, "", "LevelGenerator", "LevelPathPoints");
HAX_FIELD(LevelGenerator, bool, Generated, MAIN_ASSEMBLY, "", "LevelGenerator", "Generated");

HAX_FIELD(LevelPoint, RoomVolume*, Room, MAIN_ASSEMBLY, "", "LevelPoint", "Room");

HAX_FIELD(MapCustom, Unity::Sprite*, sprite, MAIN_ASSEMBLY, "", "MapCustom", "sprite");
HAX_FIELD(MapCustom, Unity::Color, color, MAIN_ASSEMBLY, "", "MapCustom", "color");
HAX_FIELD(MapCustom, MapCustomEntity*, mapCustomEntity, MAIN_ASSEMBLY, "", "MapCustom", "mapCustomEntity");

HAX_FIELD(PlayerDeathHead, MapCustom*, mapCustom, MAIN_ASSEMBLY, "", "PlayerDeathHead", "mapCustom");

HAX_FIELD(ValuableObject, PhysGrabObject*, physGrabObject, MAIN_ASSEMBLY, "", "ValuableObject", "physGrabObject");
HAX_FIELD(ValuableObject, float, dollarValueCurrent, MAIN_ASSEMBLY, "", "ValuableObject", "dollarValueCurrent");

HAX_FIELD(GameDirector, Unity::Camera*, MainCamera, MAIN_ASSEMBLY, "", "GameDirector", "MainCamera");
HAX_FIELD(GameDirector, Int32, currentState, MAIN_ASSEMBLY, "", "GameDirector", "currentState");
HAX_FIELD(GameDirector, System::List<PlayerAvatar*>*, PlayerList, MAIN_ASSEMBLY, "", "GameDirector", "PlayerList");

HAX_FIELD(ValuableDirector, System::List<ValuableObject*>*, valuableList, MAIN_ASSEMBLY, "", "ValuableDirector", "valuableList");

HAX_FIELD(RoundDirector, bool, extractionPointActive, MAIN_ASSEMBLY, "", "RoundDirector", "extractionPointActive");
HAX_FIELD(RoundDirector, System::List<Unity::GameObject*>*, extractionPointList, MAIN_ASSEMBLY, "", "RoundDirector", "extractionPointList");
HAX_FIELD(RoundDirector, ExtractionPoint*, extractionPointCurrent, MAIN_ASSEMBLY, "", "RoundDirector", "extractionPointCurrent");

HAX_FIELD(ExtractionPoint, bool, isLocked, MAIN_ASSEMBLY, "", "ExtractionPoint", "isLocked");
HAX_FIELD(ExtractionPoint, Int32, currentState, MAIN_ASSEMBLY, "", "ExtractionPoint", "currentState");

HAX_FIELD(StatsManager, System::Dictionary<System::String* COMMA Item*>*, itemDictionary, MAIN_ASSEMBLY, "", "StatsManager", "itemDictionary");
HAX_FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeStrength, MAIN_ASSEMBLY, "", "StatsManager", "playerUpgradeStrength");
HAX_FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeThrow, MAIN_ASSEMBLY, "", "StatsManager", "playerUpgradeThrow");
HAX_FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeStamina, MAIN_ASSEMBLY, "", "StatsManager", "playerUpgradeStamina");
HAX_FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeSpeed, MAIN_ASSEMBLY, "", "StatsManager", "playerUpgradeSpeed");
HAX_FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeLaunch, MAIN_ASSEMBLY, "", "StatsManager", "playerUpgradeLaunch");
HAX_FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeHealth, MAIN_ASSEMBLY, "", "StatsManager", "playerUpgradeHealth");
HAX_FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeMapPlayerCount, MAIN_ASSEMBLY, "", "StatsManager", "playerUpgradeMapPlayerCount");
HAX_FIELD(StatsManager, System::Dictionary<System::String* COMMA Int32>*, playerUpgradeExtraJump, MAIN_ASSEMBLY, "", "StatsManager", "playerUpgradeExtraJump");

HAX_FIELD(Item, Unity::GameObject*, prefab, MAIN_ASSEMBLY, "", "Item", "prefab");

HAX_FIELD(PhysGrabber, float, grabRange, MAIN_ASSEMBLY, "", "PhysGrabber", "grabRange");

HAX_FIELD(ItemBattery, float, batteryLife, MAIN_ASSEMBLY, "", "ItemBattery", "batteryLife");

HAX_FIELD(ItemGun, float, gunRandomSpread, MAIN_ASSEMBLY, "", "ItemGun", "gunRandomSpread");
HAX_FIELD(ItemGun, PhysGrabObject*, physGrabObject, MAIN_ASSEMBLY, "", "ItemGun", "physGrabObject");
HAX_FIELD(ItemGun, Unity::Transform*, gunMuzzle, MAIN_ASSEMBLY, "", "ItemGun", "gunMuzzle");
HAX_FIELD(ItemGun, float, gunRange, MAIN_ASSEMBLY, "", "ItemGun", "gunRange");

HAX_FIELD(Level, System::String*, NarrativeName, MAIN_ASSEMBLY, "", "Level", "NarrativeName");

HAX_FIELD(PhysGrabObject, bool, grabbedLocal, MAIN_ASSEMBLY, "", "PhysGrabObject", "grabbedLocal");
HAX_FIELD(PhysGrabObject, Unity::Vector3, centerPoint, MAIN_ASSEMBLY, "", "PhysGrabObject", "centerPoint");

HAX_FIELD(RunManager, System::List<Level*>*, levels, MAIN_ASSEMBLY, "", "RunManager", "levels");
HAX_FIELD(RunManager, Level*, levelCurrent, MAIN_ASSEMBLY, "", "RunManager", "levelCurrent");
HAX_FIELD(RunManager, Level*, levelMainMenu, MAIN_ASSEMBLY, "", "RunManager", "levelMainMenu");
HAX_FIELD(RunManager, Level*, levelLobbyMenu, MAIN_ASSEMBLY, "", "RunManager", "levelLobbyMenu");
HAX_FIELD(RunManager, Level*, levelShop, MAIN_ASSEMBLY, "", "RunManager", "levelShop");
HAX_FIELD(RunManager, Level*, previousRunLevel, MAIN_ASSEMBLY, "", "RunManager", "previousRunLevel");
HAX_FIELD(RunManager, Int32, levelsCompleted, MAIN_ASSEMBLY, "", "RunManager", "levelsCompleted");

HAX_FIELD(PunManager, StatsManager*, statsManager, MAIN_ASSEMBLY, "", "PunManager", "statsManager");

HAX_FIELD(MenuManager, Unity::Component*, currentMenuPage, MAIN_ASSEMBLY, "", "MenuManager", "currentMenuPage");

HAX_FIELD(Map, System::List<MapModule*>*, MapModules, MAIN_ASSEMBLY, "", "Map", "MapModules");

HAX_FIELD(PlayerVoiceChat, bool, inLobbyMixer, MAIN_ASSEMBLY, "", "PlayerVoiceChat", "inLobbyMixer");

HAX_FIELD(PhysGrabObjectImpactDetector, bool, isValuable, MAIN_ASSEMBLY, "", "PhysGrabObjectImpactDetector", "isValuable");
HAX_FIELD(PhysGrabObjectImpactDetector, float, impactDisabledTimer, MAIN_ASSEMBLY, "", "PhysGrabObjectImpactDetector", "impactDisabledTimer");

#undef HAX_FIELD
#endif

#ifdef HAX_METHOD
HAX_METHOD(PlayerHealth, void, Hurt, (PlayerHealth*, Int32, bool, Int32), MAIN_ASSEMBLY, "", "PlayerHealth", "Hurt", nullptr);
HAX_METHOD(PlayerHealth, void, HealOther, (PlayerHealth*, Int32, bool), MAIN_ASSEMBLY, "", "PlayerHealth", "HealOther", nullptr);
HAX_METHOD(PlayerHealth, void, Update, (PlayerHealth*), MAIN_ASSEMBLY, "", "PlayerHealth", "Update", nullptr);

HAX_METHOD(PlayerController, void, FixedUpdate, (PlayerController*), MAIN_ASSEMBLY, "", "PlayerController", "FixedUpdate", nullptr);
HAX_METHOD(PlayerController, void, Update, (PlayerController*), MAIN_ASSEMBLY, "", "PlayerController", "Update", nullptr);

HAX_METHOD(PlayerTumble, void, TumbleRequest, (PlayerTumble*, bool, bool), MAIN_ASSEMBLY, "", "PlayerTumble", "TumbleRequest", nullptr);

HAX_METHOD(SemiFunc, Unity::Camera*, MainCamera, (), MAIN_ASSEMBLY, "", "SemiFunc", "MainCamera", nullptr);
HAX_METHOD(SemiFunc, bool, OnScreen, (Unity::Vector3, float, float), MAIN_ASSEMBLY, "", "SemiFunc", "OnScreen", nullptr);
HAX_METHOD(SemiFunc, bool, IsMasterClient, (), MAIN_ASSEMBLY, "", "SemiFunc", "IsMasterClient", nullptr);
HAX_METHOD(SemiFunc, bool, IsMultiplayer, (), MAIN_ASSEMBLY, "", "SemiFunc", "IsMultiplayer", nullptr);
HAX_METHOD(SemiFunc, System::String*, PlayerGetSteamID, (PlayerAvatar*), MAIN_ASSEMBLY, "", "SemiFunc", "PlayerGetSteamID", nullptr);
HAX_METHOD(SemiFunc, Int32, StatSetRunCurrency, (Int32), MAIN_ASSEMBLY, "", "SemiFunc", "StatSetRunCurrency", nullptr);
HAX_METHOD(SemiFunc, bool, IsMasterClientOrSingleplayer, (), MAIN_ASSEMBLY, "", "SemiFunc", "IsMasterClientOrSingleplayer", nullptr);

HAX_METHOD(EnemyDirector, void, Update, (EnemyDirector*), MAIN_ASSEMBLY, "", "EnemyDirector", "Update", nullptr);

HAX_METHOD(LevelGenerator, void, EnemySpawn, (LevelGenerator*, EnemySetup*, Unity::Vector3), MAIN_ASSEMBLY, "", "LevelGenerator", "EnemySpawn", nullptr);

HAX_METHOD(EnemyParent, void, Despawn, (EnemyParent*), MAIN_ASSEMBLY, "", "EnemyParent", "Despawn", nullptr);
HAX_METHOD(EnemyParent, void, SpawnRPC, (EnemyParent*), MAIN_ASSEMBLY, "", "EnemyParent", "SpawnRPC", nullptr);
HAX_METHOD(EnemyParent, void, Update, (EnemyParent*), MAIN_ASSEMBLY, "", "EnemyParent", "Update", nullptr);

HAX_METHOD(EnemyHealth, void, Hurt, (EnemyHealth*, Int32, void*), MAIN_ASSEMBLY, "", "EnemyHealth", "Hurt", nullptr);

HAX_METHOD(EnemyRigidbody, void, FixedUpdate, (EnemyRigidbody*), MAIN_ASSEMBLY, "", "EnemyRigidbody", "FixedUpdate", nullptr);

HAX_METHOD(MapCustom, void, Hide, (MapCustom*), MAIN_ASSEMBLY, "", "MapCustom", "Hide", nullptr);

HAX_METHOD(PlayerAvatar, void, OnDestroy, (PlayerAvatar*), MAIN_ASSEMBLY, "", "PlayerAvatar", "OnDestroy", nullptr);
HAX_METHOD(PlayerAvatar, void, PlayerDeath, (PlayerAvatar*, Int32), MAIN_ASSEMBLY, "", "PlayerAvatar", "PlayerDeath", nullptr);
HAX_METHOD(PlayerAvatar, void, Revive, (PlayerAvatar*, bool), MAIN_ASSEMBLY, "", "PlayerAvatar", "Revive", nullptr);
HAX_METHOD(PlayerAvatar, void, SpawnRPC, (PlayerAvatar*, void*, void*), MAIN_ASSEMBLY, "", "PlayerAvatar", "SpawnRPC", nullptr);
HAX_METHOD(PlayerAvatar, void, PlayerDeathRPC, (PlayerAvatar*, Int32), MAIN_ASSEMBLY, "", "PlayerAvatar", "PlayerDeathRPC", nullptr);

HAX_METHOD(ValuableObject, void, Update, (ValuableObject*), MAIN_ASSEMBLY, "", "ValuableObject", "Update", nullptr);
HAX_METHOD(ValuableObject, void, Discover, (ValuableObject*, Int32), MAIN_ASSEMBLY, "", "ValuableObject", "Discover", nullptr);

HAX_METHOD(GameDirector, void, Update, (GameDirector*), MAIN_ASSEMBLY, "", "GameDirector", "Update", nullptr);

HAX_METHOD(ExtractionPoint, void, OnClick, (ExtractionPoint*), MAIN_ASSEMBLY, "", "ExtractionPoint", "OnClick", nullptr);

HAX_METHOD(PhysGrabber, void, PhysGrabLogic, (PhysGrabber*), MAIN_ASSEMBLY, "", "PhysGrabber", "PhysGrabLogic", nullptr);
HAX_METHOD(PhysGrabber, void, RayCheck, (PhysGrabber*, bool), MAIN_ASSEMBLY, "", "PhysGrabber", "RayCheck", nullptr);

HAX_METHOD(ItemBattery, void, Update, (ItemBattery*), MAIN_ASSEMBLY, "", "ItemBattery", "Update", nullptr);

HAX_METHOD(ItemGun, void, Shoot, (ItemGun*), MAIN_ASSEMBLY, "", "ItemGun", "Shoot", nullptr);

HAX_METHOD(ItemGun, void, Update, (ItemGun*), MAIN_ASSEMBLY, "", "ItemGun", "Update", nullptr);

HAX_METHOD(RunManager, void, SetRunLevel, (RunManager*), MAIN_ASSEMBLY, "", "RunManager", "SetRunLevel", nullptr);
HAX_METHOD(RunManager, void, ChangeLevel, (RunManager*, bool, bool, Int32), MAIN_ASSEMBLY, "", "RunManager", "ChangeLevel", nullptr);

HAX_METHOD(PunManager, Int32, UpgradePlayerGrabStrength, (PunManager*, System::String*), MAIN_ASSEMBLY, "", "PunManager", "UpgradePlayerGrabStrength", nullptr);
HAX_METHOD(PunManager, Int32, UpgradePlayerThrowStrength, (PunManager*, System::String*), MAIN_ASSEMBLY, "", "PunManager", "UpgradePlayerThrowStrength", nullptr);
HAX_METHOD(PunManager, Int32, UpgradePlayerEnergy, (PunManager*, System::String*), MAIN_ASSEMBLY, "", "PunManager", "UpgradePlayerEnergy", nullptr);
HAX_METHOD(PunManager, Int32, UpgradePlayerSprintSpeed, (PunManager*, System::String*), MAIN_ASSEMBLY, "", "PunManager", "UpgradePlayerSprintSpeed", nullptr);
HAX_METHOD(PunManager, Int32, UpgradePlayerTumbleLaunch, (PunManager*, System::String*), MAIN_ASSEMBLY, "", "PunManager", "UpgradePlayerTumbleLaunch", nullptr);
HAX_METHOD(PunManager, Int32, UpgradePlayerHealth, (PunManager*, System::String*), MAIN_ASSEMBLY, "", "PunManager", "UpgradePlayerHealth", nullptr);
HAX_METHOD(PunManager, Int32, UpgradeMapPlayerCount, (PunManager*, System::String*), MAIN_ASSEMBLY, "", "PunManager", "UpgradeMapPlayerCount", nullptr);
HAX_METHOD(PunManager, Int32, UpgradePlayerExtraJump, (PunManager*, System::String*), MAIN_ASSEMBLY, "", "PunManager", "UpgradePlayerExtraJump", nullptr);

HAX_METHOD(DataDirector, void, SaveDeleteCheck, (DataDirector*, bool), MAIN_ASSEMBLY, "", "DataDirector", "SaveDeleteCheck", nullptr);

HAX_METHOD(ExtractionPoint, void, HaulGoalSet, (ExtractionPoint*, Int32), MAIN_ASSEMBLY, "", "ExtractionPoint", "HaulGoalSet", nullptr);

HAX_METHOD(RoundDirector, void, ExtractionPointsUnlock, (RoundDirector*), MAIN_ASSEMBLY, "", "RoundDirector", "ExtractionPointsUnlock", nullptr);

HAX_METHOD(PlayerVoiceChat, void, ToggleLobby, (PlayerVoiceChat*, bool), MAIN_ASSEMBLY, "", "PlayerVoiceChat", "ToggleLobby", nullptr);

HAX_METHOD(PhysGrabObjectImpactDetector, void, FixedUpdate, (PhysGrabObjectImpactDetector*), MAIN_ASSEMBLY, "", "PhysGrabObjectImpactDetector", "FixedUpdate", nullptr);

HAX_METHOD(Physics, bool, Raycast, (void*, void*, void*, float, Int32, Int32), "UnityEngine.PhysicsModule", "UnityEngine", "Physics", "Raycast", "System.Boolean(UnityEngine.Vector3,UnityEngine.Vector3,UnityEngine.RaycastHit&,System.Single,System.Int32,UnityEngine.QueryTriggerInteraction)");

HAX_METHOD(PhotonNetwork, bool, IsMasterClient, (), "PhotonUnityNetworking", "Photon.Pun", "PhotonNetwork", "get_IsMasterClient", nullptr);

HAX_METHOD(RenderSettings, bool, get_fog, (), "UnityEngine.CoreModule", "UnityEngine", "RenderSettings", "get_fog", nullptr);
HAX_METHOD(RenderSettings, void, set_fog, (bool), "UnityEngine.CoreModule", "UnityEngine", "RenderSettings", "set_fog", nullptr);

HAX_METHOD(Camera, void, FireOnPreRender, (Unity::Camera*), "UnityEngine.CoreModule", "UnityEngine", "Camera", "FireOnPreRender", nullptr);
HAX_METHOD(EventSystem, void, Update, (void*), "UnityEngine.UI", "UnityEngine.EventSystems", "EventSystem", "Update", nullptr);

HAX_METHOD(MapModule, void, Hide, (MapModule*), MAIN_ASSEMBLY, "", "MapModule", "Hide", nullptr);

HAX_METHOD(RoomVolume, void, SetExplored, (RoomVolume*), MAIN_ASSEMBLY, "", "RoomVolume", "SetExplored", nullptr);

HAX_METHOD(PhysGrabObject, void, Teleport, (PhysGrabObject*, Unity::Vector3*, Unity::Quaternion*), MAIN_ASSEMBLY, "", "PhysGrabObject", "Teleport", nullptr);

#undef HAX_METHOD
#endif