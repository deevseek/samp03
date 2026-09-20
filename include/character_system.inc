#if defined _RP_CHARACTER_SYSTEM_INCLUDED
    #endinput
#endif
#define _RP_CHARACTER_SYSTEM_INCLUDED

// SQLite is provided by the SA-MP server and does not require an external plugin.
#define CHARACTER_DATABASE_PATH        "characters.db"
#define CHARACTER_DEFAULT_MONEY         (0)

new DB:CharacterDatabase;
new PlayerCharacterId[MAX_PLAYERS];

stock bool:Character_IsDatabaseReady()
{
    return (_:CharacterDatabase != 0);
}

stock Character_Reset(playerid)
{
    if (!Player_IsValidId(playerid)) return 0;

    PlayerCharacterId[playerid] = 0;
    return 1;
}

stock bool:Character_IsNameValid(const characterName[])
{
    new length = strlen(characterName);

    if (length < 3 || length > MAX_PLAYER_NAME) return false;

    for (new index = 0; index < length; index++)
    {
        if ((characterName[index] >= 'A' && characterName[index] <= 'Z') ||
            (characterName[index] >= 'a' && characterName[index] <= 'z') ||
            (characterName[index] >= '0' && characterName[index] <= '9') ||
            characterName[index] == '_') continue;

        return false;
    }
    return true;
}

stock bool:Character_InitializeDatabase()
{
    new DBResult:result;

    CharacterDatabase = db_open(CHARACTER_DATABASE_PATH);
    if (!Character_IsDatabaseReady())
    {
        printf("[Character] Database connection failure: %s", CHARACTER_DATABASE_PATH);
        return false;
    }

    result = db_query(CharacterDatabase,
        "CREATE TABLE IF NOT EXISTS characters (character_id INTEGER PRIMARY KEY AUTOINCREMENT, account_identifier TEXT, account_id INTEGER NOT NULL UNIQUE, character_name TEXT NOT NULL, money INTEGER NOT NULL DEFAULT 0, skin INTEGER NOT NULL, health REAL NOT NULL, armour REAL NOT NULL, spawn_x REAL NOT NULL, spawn_y REAL NOT NULL, spawn_z REAL NOT NULL, spawn_angle REAL NOT NULL, created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP, updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP, FOREIGN KEY(account_id) REFERENCES accounts(id))");
    if (_:result == 0)
    {
        printf("[Character] Database initialization failure: could not create characters table.");
        db_close(CharacterDatabase);
        CharacterDatabase = DB:0;
        return false;
    }

    db_free_result(result);
    printf("[Character] SQLite database ready: scriptfiles/%s", CHARACTER_DATABASE_PATH);
    return true;
}

stock Character_CloseDatabase()
{
    if (!Character_IsDatabaseReady()) return 0;

    db_close(CharacterDatabase);
    CharacterDatabase = DB:0;
    return 1;
}

stock Character_CaptureRuntime(playerid)
{
    if (!Player_IsOnline(playerid) || !PlayerData[playerid][PlayerHasCharacter]) return 0;

    PlayerData[playerid][PlayerMoney] = GetPlayerMoney(playerid);
    PlayerData[playerid][PlayerSkin] = GetPlayerSkin(playerid);
    GetPlayerHealth(playerid, PlayerData[playerid][PlayerHealth]);
    GetPlayerArmour(playerid, PlayerData[playerid][PlayerArmour]);
    GetPlayerPos(playerid, PlayerData[playerid][PlayerSpawnX], PlayerData[playerid][PlayerSpawnY], PlayerData[playerid][PlayerSpawnZ]);
    GetPlayerFacingAngle(playerid, PlayerData[playerid][PlayerSpawnAngle]);
    return 1;
}

stock bool:Character_Create(playerid, const characterName[])
{
    new query[512], DBResult:result;

    if (!Player_IsOnline(playerid) || !PlayerData[playerid][AccountLoggedIn] || PlayerData[playerid][AccountID] <= 0 || !Character_IsDatabaseReady()) return false;
    if (!Character_IsNameValid(characterName))
    {
        printf("[Character] Character create failure for player %d: invalid name.", playerid);
        return false;
    }

    format(query, sizeof(query), "INSERT INTO characters (account_identifier, account_id, character_name, money, skin, health, armour, spawn_x, spawn_y, spawn_z, spawn_angle) VALUES ('%s', %d, '%s', %d, %d, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f)", PlayerData[playerid][AccountUsername], PlayerData[playerid][AccountID], characterName, CHARACTER_DEFAULT_MONEY, PLAYER_DEFAULT_SKIN, PLAYER_DEFAULT_HEALTH, PLAYER_DEFAULT_ARMOUR, PLAYER_SPAWN_X, PLAYER_SPAWN_Y, PLAYER_SPAWN_Z, PLAYER_SPAWN_ANGLE);
    result = db_query(CharacterDatabase, query);
    if (_:result == 0)
    {
        printf("[Character] Character create failure for %s.", PlayerData[playerid][PlayerName]);
        return false;
    }
    db_free_result(result);

    PlayerData[playerid][PlayerHasCharacter] = true;
    format(PlayerData[playerid][PlayerCharacterName], MAX_PLAYER_NAME + 1, "%s", characterName);
    PlayerData[playerid][PlayerMoney] = CHARACTER_DEFAULT_MONEY;
    PlayerData[playerid][PlayerSkin] = PLAYER_DEFAULT_SKIN;
    PlayerData[playerid][PlayerHealth] = PLAYER_DEFAULT_HEALTH;
    PlayerData[playerid][PlayerArmour] = PLAYER_DEFAULT_ARMOUR;
    PlayerData[playerid][PlayerSpawnX] = PLAYER_SPAWN_X;
    PlayerData[playerid][PlayerSpawnY] = PLAYER_SPAWN_Y;
    PlayerData[playerid][PlayerSpawnZ] = PLAYER_SPAWN_Z;
    PlayerData[playerid][PlayerSpawnAngle] = PLAYER_SPAWN_ANGLE;

    result = db_query(CharacterDatabase, "SELECT last_insert_rowid() AS character_id");
    if (_:result == 0 || db_num_rows(result) == 0)
    {
        if (_:result != 0) db_free_result(result);
        printf("[Character] Character create failure for %s: could not read character ID.", PlayerData[playerid][PlayerName]);
        PlayerData[playerid][PlayerHasCharacter] = false;
        return false;
    }
    db_get_field_assoc(result, "character_id", query, sizeof(query));
    PlayerCharacterId[playerid] = strval(query);
    db_free_result(result);
    return true;
}

stock bool:Character_Load(playerid)
{
    new query[512], value[64], DBResult:result;

    if (!Player_IsOnline(playerid) || !PlayerData[playerid][AccountLoggedIn] || PlayerData[playerid][AccountID] <= 0 || !Character_IsDatabaseReady()) return false;

    format(query, sizeof(query), "SELECT character_id, character_name, money, skin, health, armour, spawn_x, spawn_y, spawn_z, spawn_angle FROM characters WHERE account_id = %d LIMIT 1", PlayerData[playerid][AccountID]);
    result = db_query(CharacterDatabase, query);
    if (_:result == 0)
    {
        printf("[Character] Character load failure for %s.", PlayerData[playerid][PlayerName]);
        return false;
    }

    if (db_num_rows(result) == 0)
    {
        db_free_result(result);
        return Character_Create(playerid, PlayerData[playerid][AccountUsername]);
    }

    db_get_field_assoc(result, "character_id", value, sizeof(value));
    PlayerCharacterId[playerid] = strval(value);
    db_get_field_assoc(result, "character_name", PlayerData[playerid][PlayerCharacterName], MAX_PLAYER_NAME + 1);
    db_get_field_assoc(result, "money", value, sizeof(value));
    PlayerData[playerid][PlayerMoney] = strval(value);
    db_get_field_assoc(result, "skin", value, sizeof(value));
    PlayerData[playerid][PlayerSkin] = strval(value);
    db_get_field_assoc(result, "health", value, sizeof(value));
    PlayerData[playerid][PlayerHealth] = floatstr(value);
    db_get_field_assoc(result, "armour", value, sizeof(value));
    PlayerData[playerid][PlayerArmour] = floatstr(value);
    db_get_field_assoc(result, "spawn_x", value, sizeof(value));
    PlayerData[playerid][PlayerSpawnX] = floatstr(value);
    db_get_field_assoc(result, "spawn_y", value, sizeof(value));
    PlayerData[playerid][PlayerSpawnY] = floatstr(value);
    db_get_field_assoc(result, "spawn_z", value, sizeof(value));
    PlayerData[playerid][PlayerSpawnZ] = floatstr(value);
    db_get_field_assoc(result, "spawn_angle", value, sizeof(value));
    PlayerData[playerid][PlayerSpawnAngle] = floatstr(value);
    PlayerData[playerid][PlayerHasCharacter] = true;
    db_free_result(result);
    return true;
}

stock bool:Character_Save(playerid)
{
    new query[512], DBResult:result;

    if (!Player_IsOnline(playerid) || !PlayerData[playerid][AccountLoggedIn] || !PlayerData[playerid][PlayerHasCharacter] || PlayerCharacterId[playerid] <= 0 || !Character_IsDatabaseReady()) return false;

    Character_CaptureRuntime(playerid);
    format(query, sizeof(query), "UPDATE characters SET character_name = '%s', money = %d, skin = %d, health = %.4f, armour = %.4f, spawn_x = %.4f, spawn_y = %.4f, spawn_z = %.4f, spawn_angle = %.4f, updated_at = CURRENT_TIMESTAMP WHERE character_id = %d AND account_id = %d",
        PlayerData[playerid][PlayerCharacterName], PlayerData[playerid][PlayerMoney], PlayerData[playerid][PlayerSkin],
        PlayerData[playerid][PlayerHealth], PlayerData[playerid][PlayerArmour], PlayerData[playerid][PlayerSpawnX], PlayerData[playerid][PlayerSpawnY],
        PlayerData[playerid][PlayerSpawnZ], PlayerData[playerid][PlayerSpawnAngle], PlayerCharacterId[playerid], PlayerData[playerid][AccountID]);
    result = db_query(CharacterDatabase, query);
    if (_:result == 0)
    {
        printf("[Character] Character save failure for %s (ID %d).", PlayerData[playerid][PlayerName], PlayerCharacterId[playerid]);
        return false;
    }
    db_free_result(result);
    return true;
}
