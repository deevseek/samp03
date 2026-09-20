#if defined _RP_PLAYER_SYSTEM_INCLUDED
    #endinput
#endif
#define _RP_PLAYER_SYSTEM_INCLUDED

#define PLAYER_DEFAULT_SKIN        (60)
#define PLAYER_SPAWN_X             (1481.1011)
#define PLAYER_SPAWN_Y             (-1740.0787)
#define PLAYER_SPAWN_Z             (13.5469)
#define PLAYER_SPAWN_ANGLE         (0.0)
#define PLAYER_DEFAULT_HEALTH      (100.0)
#define PLAYER_DEFAULT_ARMOUR      (0.0)

enum E_PLAYER_DATA
{
    bool:PlayerConnected,
    PlayerId,
    PlayerName[MAX_PLAYER_NAME + 1],
    bool:PlayerHasCharacter,
    PlayerAdminLevel,
    PlayerMoney,
    PlayerSkin,
    Float:PlayerSpawnX,
    Float:PlayerSpawnY,
    Float:PlayerSpawnZ,
    Float:PlayerSpawnAngle,
    Float:PlayerHealth,
    Float:PlayerArmour
};

new PlayerData[MAX_PLAYERS][E_PLAYER_DATA];

stock bool:Player_IsValidId(playerid)
{
    return (playerid >= 0 && playerid < MAX_PLAYERS);
}

stock bool:Player_IsOnline(playerid)
{
    return Player_IsValidId(playerid) && PlayerData[playerid][PlayerConnected];
}

stock Player_ResetState(playerid)
{
    if (!Player_IsValidId(playerid)) return 0;

    PlayerData[playerid][PlayerConnected] = false;
    PlayerData[playerid][PlayerId] = INVALID_PLAYER_ID;
    PlayerData[playerid][PlayerName][0] = EOS;
    PlayerData[playerid][PlayerHasCharacter] = false;
    PlayerData[playerid][PlayerAdminLevel] = 0;
    PlayerData[playerid][PlayerMoney] = 0;
    PlayerData[playerid][PlayerSkin] = PLAYER_DEFAULT_SKIN;
    PlayerData[playerid][PlayerSpawnX] = PLAYER_SPAWN_X;
    PlayerData[playerid][PlayerSpawnY] = PLAYER_SPAWN_Y;
    PlayerData[playerid][PlayerSpawnZ] = PLAYER_SPAWN_Z;
    PlayerData[playerid][PlayerSpawnAngle] = PLAYER_SPAWN_ANGLE;
    PlayerData[playerid][PlayerHealth] = PLAYER_DEFAULT_HEALTH;
    PlayerData[playerid][PlayerArmour] = PLAYER_DEFAULT_ARMOUR;
    return 1;
}

stock bool:Player_Initialize(playerid)
{
    new playerName[MAX_PLAYER_NAME + 1];

    if (!Player_IsValidId(playerid)) return false;

    Player_ResetState(playerid);
    GetPlayerName(playerid, playerName, sizeof(playerName));
    if (!strlen(playerName)) return false;

    PlayerData[playerid][PlayerConnected] = true;
    PlayerData[playerid][PlayerId] = playerid;
    format(PlayerData[playerid][PlayerName], MAX_PLAYER_NAME + 1, "%s", playerName);
    return true;
}

stock Player_ApplySpawn(playerid)
{
    if (!Player_IsOnline(playerid)) return 0;

    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerSkin(playerid, PlayerData[playerid][PlayerSkin]);
    SetPlayerPos(playerid, PlayerData[playerid][PlayerSpawnX], PlayerData[playerid][PlayerSpawnY], PlayerData[playerid][PlayerSpawnZ]);
    SetPlayerFacingAngle(playerid, PlayerData[playerid][PlayerSpawnAngle]);
    SetPlayerHealth(playerid, PlayerData[playerid][PlayerHealth]);
    SetPlayerArmour(playerid, PlayerData[playerid][PlayerArmour]);
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, PlayerData[playerid][PlayerMoney]);
    return 1;
}

stock Player_ResetRuntimeAfterDeath(playerid)
{
    if (!Player_IsOnline(playerid)) return 0;

    PlayerData[playerid][PlayerHealth] = PLAYER_DEFAULT_HEALTH;
    PlayerData[playerid][PlayerArmour] = PLAYER_DEFAULT_ARMOUR;
    return 1;
}

stock Player_GetOnlineCount()
{
    new count;

    for (new playerid = 0; playerid < MAX_PLAYERS; playerid++)
    {
        if (Player_IsOnline(playerid)) count++;
    }
    return count;
}
