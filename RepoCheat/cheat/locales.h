#pragma once

#define RUS(t) reinterpret_cast<const char*>(u8 ## t)

namespace Locales {
    const char* WATERMARK[2] = { "Exclusively on Patreon",   RUS("Эксклюзивно на Бусти") };
    const char* HOTKEY[2] = { "Hotkey##LANG",             RUS("Клавиша##Lang") };
    const char* SHOWHIDE[2] = { "Show / Hide Cheat Menu",   RUS("Показать / Спрятать Меню") };
    const char* SETTINGS[2] = { "Settings",         RUS("Настройки") };
    const char* LANGUAGE[2] = { "Language",         RUS("Язык") };

    const char* HEALTH[2] = { "Health",         RUS("Здоровье") };
    const char* HEAL_TO_MAX[2] = {"Heal to max?", RUS("Исцелять на максимум?")};
    const char* GODMODE[2] = { "Godmode",         RUS("Бессмертие") };
    const char* HEAL_MAX[2] = { "Heal to maximum",         RUS("Исцелиться до максимума") };

    const char* MOVEMENT[2] = { "Movement",         RUS("Передвижение") };
    const char* INF_STAMINA[2] = { "Infinite stamina",         RUS("Бесконечная выносливость") };
    const char* INF_JUMPS[2] = { "Infinite jumps",         RUS("Бесконечные прыжки") };
    const char* NEVER_TUMBLE[2] = { "Never tumble",         RUS("Никогда не падать") };
    const char* SPEED_MULT[2] = { "Speed multiplier",         RUS("Ускорение") };
    const char* CROUCH_SPEED[2] = { "Crouch speed",         RUS("Скорость в приседе") };
    const char* DEFAULT_CROUCH[2] = { "Default##Crouch",         RUS("По умолчанию##Crouch") };
    const char* WALK_CROUCH[2] = { "Walk##Crouch",         RUS("Хождение##Crouch") };
    const char* SPRINT_CROUCH[2] = { "Sprint##Crouch",         RUS("Бег##Crouch") };
    const char* WALK_SPEED[2] = { "Walk speed",         RUS("Скорость хождения") };
    const char* DEFAULT_WALK[2] = { "Default##Walk",         RUS("По умолчанию##Walk") };
    const char* SPRINT_WALK[2] = { "Sprint##Walk",         RUS("Бег##Walk") };

    const char* ENEMIES[2] = { "Enemies",         RUS("Враги") };
    const char* ESP_ENEMIES[2] = { "See through walls",         RUS("Видеть сквозь стены") };
    const char* SHOW_BORDERS[2] = { "Show borders",         RUS("Показывать границы") };
    const char* SHOW_NAME[2] = { "Show names",         RUS("Показывать имена") };
    const char* SHOW_HEALTH[2] = { "Show health",         RUS("Показывать здоровье") };
    const char* BLIND[2] = { "Blind enemies",         RUS("Не видят игроков") };
    const char* OHK[2] = { "Die with one hit",         RUS("Умирают с одного удара") };
    const char* DAMAGE_MULT[2] = { "Damage multiplier",         RUS("Множитель урона") };
    const char* SPAWN_ENEMY[2] = { "Spawn##Enemy",         RUS("Заспавнить##Enemy") };
    const char* ENEMY_SPAWN[2] = { "Enemy##Spawn",         RUS("Враг##Spawn") };
    const char* KILL_ALL[2] = { "Kill all enemies",         RUS("Убить всех врагов") };

    const char* MAP[2] = { "Map",         RUS("Карта") };
    const char* SHOW_ENEMIES[2] = { "Show enemies on map",         RUS("Показывать врагов на карте") };
    const char* REVEAL_ROOMS[2] = { "Reveal all rooms",         RUS("Раскрыть все комнаты на карте") };
    const char* REVEAL_ITEMS[2] = { "Reveal all valuables",         RUS("Показать все ценности на карте") };

    const char* GRAB[2] = { "Grab",         RUS("Захват") };
    const char* GRAB_PERMANENT[2] = { "Enemies can be grabbed permanently", RUS("Врагов можно держать вечно") };
    const char* GRAB_RANGE[2] = { "Unlimited grab range", RUS("Безграничная дальность захвата") };

    const char* VALUABLES[2] = { "Valuables",         RUS("Ценности") };
    const char* NO_IMPACT[2] = { "Dont get damage",         RUS("Не получают урон") };
    const char* ITEMS_ESP[2] = { "Show through walls##Items",         RUS("Показывать через стены##Items") };
    const char* SHOW_PRICE[2] = { "Show price",         RUS("Показывать цену") };
    const char* LIMIT_DIST[2] = { "Show only close ones",         RUS("Показывать только близкие") };

    const char* EXTRACTION_POINTS[2] = { "Extraction points",         RUS("Точки сборки ценностей") };
    const char* SHOW_POINTS[2] = { "Show through walls##Points",         RUS("Показывать через стены##Points") };
    const char* ACTIVATE_NEXT[2] = { "Activate next point",         RUS("Активировать следующую") };
    const char* FILL_QUOTA[2] = { "Fill quota",         RUS("Выполнить квоту") };
    const char* UNLOCK_POINTS[2] = { "Unlock all extraction points",         RUS("Разблокировать все точки") };

    const char* ITEMS[2] = { "Items",         RUS("Предметы") };
    const char* ITEM_SPAWN[2] = { "Item##Spawn",         RUS("Предмет##Spawn") };
    const char* SPAWN_ITEM[2] = { "Spawn##Item",         RUS("Заспавнить##Item") };
    const char* MAX_BATTERY[2] = { "Max battery",         RUS("Полный заряд") };
    const char* NO_SPREAD[2] = { "No spread",         RUS("Нет разброса") };
    const char* LASER_CROSSHAIR[2] = { "Laser crosshair",         RUS("Лазерный прицел") };

    const char* LEVELS[2] = { "Levels",         RUS("Уровни") };
    const char* NEXT_LEVEL[2] = { "Go to next level",         RUS("Следующий уровень") };
    const char* ALLOWED_LEVELS[2] = { "Allowed levels:",         RUS("Разрешенные уровни:") };
    const char* LEVELS_COMPLETED[2] = { "Levels completed",         RUS("Пройдено уровней:") };
    const char* APPLY[2] = { "apply##levels",         RUS("применить##уровень") };
    const char* INVALID_INPUT[2] = { "Invalid input!",         RUS("Некорректный ввод!") };

    const char* UPGRADES[2] = { "Upgrades",         RUS("Улучшения") };
    const char* STRENGTH[2] = { "Strength: %d",         RUS("Сила: %d") };
    const char* THROW[2] = { "Throw: %d",         RUS("Бросок: %d") };
    const char* STAMINA[2] = { "Stamina: %d",         RUS("Выносливость: %d") };
    const char* SPEED[2] = { "Speed: %d",         RUS("Скорость: %d") };
    const char* TUMBLE_LAUNCH[2] = { "Tumble launch: %d",         RUS("Кувырок: %d") };
    const char* HEALTH_STAT[2] = { "Health: %d",         RUS("Здоровье: %d") };
    const char* PLAYERS_COUNT[2] = { "Map players count: %d",         RUS("Счетчик игроков: %d") };
    const char* EXTRA_JUMPS[2] = { "Extra jumps: %d",         RUS("Количество прыжков: %d") };

    const char* PLAYERS[2] = { "Players",         RUS("Игроки") };
    const char* PLAYER[2] = { "Player",         RUS("Игрок") };
    const char* SHOW_PLAYERS[2] = { "Show players through walls",         RUS("Показывать игроков через стены") };
    const char* VOICE_CHAT[2] = { "Voice chat: %s",         RUS("Голосовой чат: %s") };
    const char* SPECTATOR[2] = { "SPECTATORS",         RUS("НАБЛЮДАТЕЛЕЙ") };
    const char* ALIVE[2] = { "ALIVES",         RUS("ЖИВЫХ") };
    const char* SWITCH[2] = { "switch",         RUS("переключить") };
    const char* KILL[2] = { "Kill",         RUS("Убить") };
    const char* REVIVE[2] = { "Revive",         RUS("Возродить") };
    const char* BECOME_GHOST[2] = { "Become ghost",         RUS("Стать призраком") };

    const char* MISC[2] = { "Miscellaneous",         RUS("Разное") };
    const char* IMPROVE_VISION[2] = { "Improve vision",         RUS("Улучшить зрение") };
    const char* INF_MONEY[2] = { "Infinite shop money",         RUS("Бесконечные деньги в магазине") };
    const char* DELETE_SAVE[2] = { "Never delete save file",         RUS("Не удалять сохранение") };
    const char* SHOW_SPAWN[2] = { "Show spawn through walls",         RUS("Показывать спавн через стены") };

    const char* SPAWN[2] = { "Spawn",         RUS("Спавн") };
    const char* EXTRACTION[2] = { "Extraction point",         RUS("Точка сбора") };

    const char* HOST_ONLY[2] = { "Doesnt work if you are not host", RUS("Не работает если вы не хост") };
    const char* HOST_ONLY_2[2] = { "Doesnt work if you are not host.\nEffects all players", RUS("Не работает если вы не хост\nРаботает для всех игроков") };

    const char* LINKS[2] = { "Links",         RUS("Ссылки") };
    const char* GHOST_HINT[2] = { "All players consider you dead,\n"
        "but you have full control over your character.\n"
        "You are invisible to players and enemies,\n"
        "but you can interact with the environment.\n"
        "This is an experimental feature and I want to hear your feedback.",

        RUS("Все игроки считают вас мертвым,\n"
        "но вы можете полностью управлять своим персонажем\n"
        "Вы невидимы для игроков и врагов,\n"
        "но можете взаимодействовать с окружением.\n"
        "Это экспериментальная функция.") };
}
