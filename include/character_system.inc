#if defined _RP_CHARACTER_SYSTEM_INCLUDED
    #endinput
#endif
#define _RP_CHARACTER_SYSTEM_INCLUDED

#define CHARACTER_DATABASE_PATH ("characters.db")
#define CHARACTER_DEFAULT_MONEY (0)

enum E_CHARACTER_LOAD_RESULT
{
    CHARACTER_LOAD_ERROR = -1,
    CHARACTER_LOAD_NOT_FOUND = 0,
    CHARACTER_LOAD_FOUND = 1
};

enum E_CHARACTER_GENDER
{
    CHARACTER_GENDER_UNKNOWN,
    CHARACTER_GENDER_MALE,
    CHARACTER_GENDER_FEMALE
};

enum E_CHARACTER_BIRTHPLACE
{
    CHARACTER_BIRTHPLACE_UNKNOWN,
    CHARACTER_BIRTHPLACE_LOS_SANTOS,
    CHARACTER_BIRTHPLACE_SAN_FIERRO,
    CHARACTER_BIRTHPLACE_LAS_VENTURAS,
    CHARACTER_BIRTHPLACE_RED_COUNTY,
    CHARACTER_BIRTHPLACE_BONE_COUNTY,
    CHARACTER_BIRTHPLACE_WHETSTONE,
    CHARACTER_BIRTHPLACE_OUTSIDE_SAN_ANDREAS
};

new DB:CharacterDatabase;
new PlayerCharacterId[MAX_PLAYERS];
new PlayerCharacterGender[MAX_PLAYERS];
new PlayerCharacterBirthplace[MAX_PLAYERS];
new PlayerCharacterBirthDay[MAX_PLAYERS];
new PlayerCharacterBirthMonth[MAX_PLAYERS];
new PlayerCharacterBirthYear[MAX_PLAYERS];
new PlayerCharacterHeight[MAX_PLAYERS];
new PlayerCharacterStarterClaimed[MAX_PLAYERS];
new PlayerCharacterCreationVersion[MAX_PLAYERS];

stock bool:Character_IsDatabaseReady() return (_:CharacterDatabase != 0);

stock Character_Reset(playerid)
{
    if (!Player_IsValidId(playerid)) return 0;
    PlayerCharacterId[playerid] = 0;
    PlayerCharacterGender[playerid] = CHARACTER_GENDER_UNKNOWN;
    PlayerCharacterBirthplace[playerid] = CHARACTER_BIRTHPLACE_UNKNOWN;
    PlayerCharacterBirthDay[playerid] = 0;
    PlayerCharacterBirthMonth[playerid] = 0;
    PlayerCharacterBirthYear[playerid] = 0;
    PlayerCharacterHeight[playerid] = 170;
    PlayerCharacterStarterClaimed[playerid] = 1;
    PlayerCharacterCreationVersion[playerid] = 0;
    return 1;
}

stock bool:Character_IsNameValid(const name[])
{
    new length = strlen(name), underscore = -1;
    if (length < 5 || length > MAX_PLAYER_NAME) return false;
    for (new i = 0; i < length; i++)
    {
        if (name[i] == '_')
        {
            if (underscore != -1) return false;
            underscore = i;
            continue;
        }
        if (!((name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= 'a' && name[i] <= 'z'))) return false;
    }
    if (underscore < 2 || length - underscore - 1 < 2) return false;
    if (!strcmp(name, "Player_Admin", true) || !strcmp(name, "Admin_Server", true)) return false;
    return true;
}

stock Character_GetGenderName(gender, output[], size)
{
    if (gender == CHARACTER_GENDER_MALE) format(output, size, "Laki-laki");
    else if (gender == CHARACTER_GENDER_FEMALE) format(output, size, "Perempuan");
    else format(output, size, "Belum diketahui");
    return 1;
}

stock Character_GetBirthplaceName(place, output[], size)
{
    switch (place)
    {
        case CHARACTER_BIRTHPLACE_LOS_SANTOS: format(output, size, "Los Santos");
        case CHARACTER_BIRTHPLACE_SAN_FIERRO: format(output, size, "San Fierro");
        case CHARACTER_BIRTHPLACE_LAS_VENTURAS: format(output, size, "Las Venturas");
        case CHARACTER_BIRTHPLACE_RED_COUNTY: format(output, size, "Red County");
        case CHARACTER_BIRTHPLACE_BONE_COUNTY: format(output, size, "Bone County");
        case CHARACTER_BIRTHPLACE_WHETSTONE: format(output, size, "Whetstone");
        case CHARACTER_BIRTHPLACE_OUTSIDE_SAN_ANDREAS: format(output, size, "Luar San Andreas");
        default: format(output, size, "Belum diketahui");
    }
    return 1;
}

stock Character_GetAge(day, month, year)
{
    new nowYear, nowMonth, nowDay;
    getdate(nowYear, nowMonth, nowDay);
    if (year <= 0) return 0;
    new age = nowYear - year;
    if (nowMonth < month || (nowMonth == month && nowDay < day)) age--;
    return age;
}

stock bool:Character_InitializeDatabase()
{
    new DBResult:r;
    CharacterDatabase = db_open(CHARACTER_DATABASE_PATH);
    if (!Character_IsDatabaseReady()) return false;
    r = db_query(CharacterDatabase, "CREATE TABLE IF NOT EXISTS characters (character_id INTEGER PRIMARY KEY AUTOINCREMENT, account_identifier TEXT, account_id INTEGER NOT NULL UNIQUE, character_name TEXT NOT NULL UNIQUE, money INTEGER NOT NULL DEFAULT 0, skin INTEGER NOT NULL, health REAL NOT NULL, armour REAL NOT NULL, spawn_x REAL NOT NULL, spawn_y REAL NOT NULL, spawn_z REAL NOT NULL, spawn_angle REAL NOT NULL, created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP, updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP, gender INTEGER DEFAULT 0, birthplace INTEGER DEFAULT 0, birth_day INTEGER DEFAULT 0, birth_month INTEGER DEFAULT 0, birth_year INTEGER DEFAULT 0, height_cm INTEGER DEFAULT 170, starter_pack_claimed INTEGER NOT NULL DEFAULT 1, creation_version INTEGER NOT NULL DEFAULT 0)");
    if (_:r == 0) { db_close(CharacterDatabase); CharacterDatabase = DB:0; return false; }
    db_free_result(r);
    // SQLite reports duplicate-column errors on an already migrated database; those are harmless.
    r = db_query(CharacterDatabase, "ALTER TABLE characters ADD COLUMN gender INTEGER DEFAULT 0"); if (_:r != 0) db_free_result(r);
    r = db_query(CharacterDatabase, "ALTER TABLE characters ADD COLUMN birthplace INTEGER DEFAULT 0"); if (_:r != 0) db_free_result(r);
    r = db_query(CharacterDatabase, "ALTER TABLE characters ADD COLUMN birth_day INTEGER DEFAULT 0"); if (_:r != 0) db_free_result(r);
    r = db_query(CharacterDatabase, "ALTER TABLE characters ADD COLUMN birth_month INTEGER DEFAULT 0"); if (_:r != 0) db_free_result(r);
    r = db_query(CharacterDatabase, "ALTER TABLE characters ADD COLUMN birth_year INTEGER DEFAULT 0"); if (_:r != 0) db_free_result(r);
    r = db_query(CharacterDatabase, "ALTER TABLE characters ADD COLUMN height_cm INTEGER DEFAULT 170"); if (_:r != 0) db_free_result(r);
    r = db_query(CharacterDatabase, "ALTER TABLE characters ADD COLUMN starter_pack_claimed INTEGER NOT NULL DEFAULT 1"); if (_:r != 0) db_free_result(r);
    r = db_query(CharacterDatabase, "ALTER TABLE characters ADD COLUMN creation_version INTEGER NOT NULL DEFAULT 0"); if (_:r != 0) db_free_result(r);
    r = db_query(CharacterDatabase, "CREATE UNIQUE INDEX IF NOT EXISTS idx_characters_name_nocase ON characters(character_name COLLATE NOCASE)");
    if (_:r == 0) return false;
    db_free_result(r);
    printf("[Character] SQLite database ready: scriptfiles/%s", CHARACTER_DATABASE_PATH);
    return true;
}

stock Character_CloseDatabase() { if (!Character_IsDatabaseReady()) return 0; db_close(CharacterDatabase); CharacterDatabase = DB:0; return 1; }

stock Character_CaptureRuntime(playerid)
{
    if (!Player_IsOnline(playerid) || !PlayerData[playerid][PlayerHasCharacter]) return 0;
    PlayerData[playerid][PlayerSkin] = GetPlayerSkin(playerid);
    GetPlayerHealth(playerid, PlayerData[playerid][PlayerHealth]);
    GetPlayerArmour(playerid, PlayerData[playerid][PlayerArmour]);
    GetPlayerPos(playerid, PlayerData[playerid][PlayerSpawnX], PlayerData[playerid][PlayerSpawnY], PlayerData[playerid][PlayerSpawnZ]);
    GetPlayerFacingAngle(playerid, PlayerData[playerid][PlayerSpawnAngle]);
    return 1;
}

stock E_CHARACTER_LOAD_RESULT:Character_Load(playerid)
{
    new query[640], value[64], DBResult:r;
    if (!Player_IsOnline(playerid) || !PlayerData[playerid][AccountLoggedIn] || PlayerData[playerid][AccountID] <= 0 || !Character_IsDatabaseReady()) return CHARACTER_LOAD_ERROR;
    format(query, sizeof(query), "SELECT character_id,character_name,money,skin,health,armour,spawn_x,spawn_y,spawn_z,spawn_angle,COALESCE(gender,0) gender,COALESCE(birthplace,0) birthplace,COALESCE(birth_day,0) birth_day,COALESCE(birth_month,0) birth_month,COALESCE(birth_year,0) birth_year,COALESCE(height_cm,170) height_cm,COALESCE(starter_pack_claimed,1) starter_pack_claimed,COALESCE(creation_version,0) creation_version FROM characters WHERE account_id=%d LIMIT 1", PlayerData[playerid][AccountID]);
    r = db_query(CharacterDatabase, query);
    if (_:r == 0) { printf("[Character] Load query failed for account %d.", PlayerData[playerid][AccountID]); return CHARACTER_LOAD_ERROR; }
    if (!db_num_rows(r)) { db_free_result(r); return CHARACTER_LOAD_NOT_FOUND; }
    db_get_field_assoc(r,"character_id",value,sizeof(value)); PlayerCharacterId[playerid]=strval(value);
    db_get_field_assoc(r,"character_name",PlayerData[playerid][PlayerCharacterName],MAX_PLAYER_NAME+1);
    db_get_field_assoc(r,"money",value,sizeof(value)); PlayerData[playerid][PlayerMoney]=strval(value);
    db_get_field_assoc(r,"skin",value,sizeof(value)); PlayerData[playerid][PlayerSkin]=strval(value);
    db_get_field_assoc(r,"health",value,sizeof(value)); PlayerData[playerid][PlayerHealth]=floatstr(value);
    db_get_field_assoc(r,"armour",value,sizeof(value)); PlayerData[playerid][PlayerArmour]=floatstr(value);
    db_get_field_assoc(r,"spawn_x",value,sizeof(value)); PlayerData[playerid][PlayerSpawnX]=floatstr(value);
    db_get_field_assoc(r,"spawn_y",value,sizeof(value)); PlayerData[playerid][PlayerSpawnY]=floatstr(value);
    db_get_field_assoc(r,"spawn_z",value,sizeof(value)); PlayerData[playerid][PlayerSpawnZ]=floatstr(value);
    db_get_field_assoc(r,"spawn_angle",value,sizeof(value)); PlayerData[playerid][PlayerSpawnAngle]=floatstr(value);
    db_get_field_assoc(r,"gender",value,sizeof(value)); PlayerCharacterGender[playerid]=strval(value);
    db_get_field_assoc(r,"birthplace",value,sizeof(value)); PlayerCharacterBirthplace[playerid]=strval(value);
    db_get_field_assoc(r,"birth_day",value,sizeof(value)); PlayerCharacterBirthDay[playerid]=strval(value);
    db_get_field_assoc(r,"birth_month",value,sizeof(value)); PlayerCharacterBirthMonth[playerid]=strval(value);
    db_get_field_assoc(r,"birth_year",value,sizeof(value)); PlayerCharacterBirthYear[playerid]=strval(value);
    db_get_field_assoc(r,"height_cm",value,sizeof(value)); PlayerCharacterHeight[playerid]=strval(value);
    db_get_field_assoc(r,"starter_pack_claimed",value,sizeof(value)); PlayerCharacterStarterClaimed[playerid]=strval(value);
    db_get_field_assoc(r,"creation_version",value,sizeof(value)); PlayerCharacterCreationVersion[playerid]=strval(value);
    PlayerData[playerid][PlayerHasCharacter] = true;
    db_free_result(r);
    return CHARACTER_LOAD_FOUND;
}

stock bool:Character_Save(playerid)
{
    new query[768], DBResult:r;
    if (!Player_IsOnline(playerid) || !PlayerData[playerid][PlayerHasCharacter] || PlayerCharacterId[playerid] <= 0) return false;
    Character_CaptureRuntime(playerid);
    format(query,sizeof(query),"UPDATE characters SET character_name='%s',money=%d,skin=%d,health=%.4f,armour=%.4f,spawn_x=%.4f,spawn_y=%.4f,spawn_z=%.4f,spawn_angle=%.4f,gender=%d,birthplace=%d,birth_day=%d,birth_month=%d,birth_year=%d,height_cm=%d,starter_pack_claimed=%d,updated_at=CURRENT_TIMESTAMP WHERE character_id=%d AND account_id=%d",PlayerData[playerid][PlayerCharacterName],PlayerData[playerid][PlayerMoney],PlayerData[playerid][PlayerSkin],PlayerData[playerid][PlayerHealth],PlayerData[playerid][PlayerArmour],PlayerData[playerid][PlayerSpawnX],PlayerData[playerid][PlayerSpawnY],PlayerData[playerid][PlayerSpawnZ],PlayerData[playerid][PlayerSpawnAngle],PlayerCharacterGender[playerid],PlayerCharacterBirthplace[playerid],PlayerCharacterBirthDay[playerid],PlayerCharacterBirthMonth[playerid],PlayerCharacterBirthYear[playerid],PlayerCharacterHeight[playerid],PlayerCharacterStarterClaimed[playerid],PlayerCharacterId[playerid],PlayerData[playerid][AccountID]);
    r=db_query(CharacterDatabase,query); if (_:r==0) return false; db_free_result(r); return true;
}
