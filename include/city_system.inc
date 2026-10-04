#if defined _RP_CITY_SYSTEM_INCLUDED
    #endinput
#endif
#define _RP_CITY_SYSTEM_INCLUDED

// City dialogs use their own range and do not overlap the account dialogs (1000-1003).
#define DIALOG_CITY_GPS                 (1100)
#define CITY_INVALID_LOCATION           (-1)
#define CITY_PICKUP_MODEL               (1239)
#define CITY_PICKUP_TYPE                (1)
#define CITY_CHECKPOINT_SIZE            (4.0)
#define CITY_COLOR_MESSAGE              (0x6EC1E4FF)
#define CITY_COLOR_SUCCESS              (0x72C96BFF)
#define CITY_LOCATION_NAME_LENGTH       (48)
#define CITY_AREA_NAME_LENGTH           (32)

enum E_CITY_LOCATION_TYPE
{
    CITY_LOCATION_GOVERNMENT,
    CITY_LOCATION_POLICE,
    CITY_LOCATION_MEDICAL,
    CITY_LOCATION_FINANCIAL,
    CITY_LOCATION_SERVICE,
    CITY_LOCATION_EMPLOYMENT,
    CITY_LOCATION_VEHICLE,
    CITY_LOCATION_REPAIR,
    CITY_LOCATION_FUEL,
    CITY_LOCATION_STORE
};

enum E_CITY_LOCATION
{
    CityLocationId,
    CityLocationName[CITY_LOCATION_NAME_LENGTH],
    E_CITY_LOCATION_TYPE:CityLocationType,
    Float:CityLocationX,
    Float:CityLocationY,
    Float:CityLocationZ,
    CityLocationInterior,
    CityLocationWorld
};

new const CityLocations[][E_CITY_LOCATION] =
{
    {0, "Los Santos City Hall", CITY_LOCATION_GOVERNMENT, 1481.1011, -1740.0787, 13.5469, 0, 0},
    {1, "Los Santos Police Department", CITY_LOCATION_POLICE, 1554.6942, -1675.6036, 16.1953, 0, 0},
    {2, "All Saints General Hospital", CITY_LOCATION_MEDICAL, 1178.6853, -1323.6768, 14.1131, 0, 0},
    {3, "Los Santos Bank", CITY_LOCATION_FINANCIAL, 1457.1158, -1010.6375, 26.8438, 0, 0},
    {4, "Department of Motor Vehicles", CITY_LOCATION_SERVICE, 1219.2427, -1812.4861, 16.5938, 0, 0},
    {5, "Los Santos Job Center", CITY_LOCATION_EMPLOYMENT, 1726.8397, -1638.0193, 20.2169, 0, 0},
    {6, "Los Santos Vehicle Dealership", CITY_LOCATION_VEHICLE, 2131.7983, -1150.3834, 24.1473, 0, 0},
    {7, "Los Santos Workshop", CITY_LOCATION_REPAIR, 1025.0602, -1024.3148, 32.1016, 0, 0},
    {8, "Idlewood Gas Station", CITY_LOCATION_FUEL, 1940.2258, -1772.6729, 13.3906, 0, 0},
    {9, "Unity General Store", CITY_LOCATION_STORE, 1833.7050, -1842.5939, 13.5781, 0, 0}
};

enum E_CITY_AREA
{
    CityAreaName[CITY_AREA_NAME_LENGTH],
    Float:CityAreaMinX,
    Float:CityAreaMinY,
    Float:CityAreaMaxX,
    Float:CityAreaMaxY
};

new const CityAreas[][E_CITY_AREA] =
{
    {"Pershing Square / Downtown", 1320.0, -1850.0, 1620.0, -1520.0},
    {"Market", 700.0, -1500.0, 1320.0, -1100.0},
    {"Idlewood", 1810.0, -1950.0, 2120.0, -1600.0},
    {"Ganton", 2220.0, -1850.0, 2600.0, -1490.0},
    {"Jefferson", 1990.0, -1600.0, 2280.0, -1250.0},
    {"East Los Santos", 2280.0, -1600.0, 2800.0, -1050.0},
    {"Vinewood", 620.0, -1250.0, 1550.0, -700.0},
    {"Rodeo", 0.0, -1700.0, 700.0, -900.0},
    {"Marina", 620.0, -1900.0, 1050.0, -1500.0},
    {"Santa Maria Beach", 0.0, -2300.0, 950.0, -1700.0}
};

new CityLocationPickup[sizeof(CityLocations)];
new Text3D:CityLocationLabel[sizeof(CityLocations)];
new CityPlayerGPS[MAX_PLAYERS];

stock bool:City_IsLocationValid(locationid)
{
    return (locationid >= 0 && locationid < sizeof(CityLocations));
}

stock bool:City_CanPlayerUse(playerid)
{
    return Player_IsOnline(playerid) && PlayerData[playerid][AccountLoggedIn] && PlayerData[playerid][PlayerHasCharacter];
}

stock City_GetLocationName(locationid, output[], outputSize)
{
    if (!City_IsLocationValid(locationid))
    {
        format(output, outputSize, "Unknown Location");
        return 0;
    }
    format(output, outputSize, "%s", CityLocations[locationid][CityLocationName]);
    return 1;
}

stock City_GetNearestLocation(playerid)
{
    new nearest = CITY_INVALID_LOCATION;
    new Float:nearestDistance = 999999.0;
    new Float:distance;

    if (!City_CanPlayerUse(playerid)) return CITY_INVALID_LOCATION;
    for (new locationid = 0; locationid < sizeof(CityLocations); locationid++)
    {
        if (GetPlayerInterior(playerid) != CityLocations[locationid][CityLocationInterior] ||
            GetPlayerVirtualWorld(playerid) != CityLocations[locationid][CityLocationWorld]) continue;

        distance = GetPlayerDistanceFromPoint(playerid, CityLocations[locationid][CityLocationX], CityLocations[locationid][CityLocationY], CityLocations[locationid][CityLocationZ]);
        if (distance < nearestDistance)
        {
            nearestDistance = distance;
            nearest = locationid;
        }
    }
    return nearest;
}

stock City_GetAreaName(playerid, output[], outputSize)
{
    new Float:x, Float:y, Float:z;
    if (!City_CanPlayerUse(playerid) || GetPlayerInterior(playerid) != 0 || GetPlayerVirtualWorld(playerid) != 0)
    {
        format(output, outputSize, "Unknown Area");
        return 0;
    }

    GetPlayerPos(playerid, x, y, z);
    for (new areaid = 0; areaid < sizeof(CityAreas); areaid++)
    {
        if (x >= CityAreas[areaid][CityAreaMinX] && x <= CityAreas[areaid][CityAreaMaxX] &&
            y >= CityAreas[areaid][CityAreaMinY] && y <= CityAreas[areaid][CityAreaMaxY])
        {
            format(output, outputSize, "%s", CityAreas[areaid][CityAreaName]);
            return 1;
        }
    }
    format(output, outputSize, "Los Santos");
    return 1;
}

stock City_Initialize()
{
    new labelText[96];
    for (new locationid = 0; locationid < sizeof(CityLocations); locationid++)
    {
        CityLocationPickup[locationid] = CreatePickup(CITY_PICKUP_MODEL, CITY_PICKUP_TYPE,
            CityLocations[locationid][CityLocationX], CityLocations[locationid][CityLocationY], CityLocations[locationid][CityLocationZ], CityLocations[locationid][CityLocationWorld]);
        format(labelText, sizeof(labelText), "%s\n{B7B7B7}City point of interest", CityLocations[locationid][CityLocationName]);
        CityLocationLabel[locationid] = Create3DTextLabel(labelText, CITY_COLOR_MESSAGE,
            CityLocations[locationid][CityLocationX], CityLocations[locationid][CityLocationY], CityLocations[locationid][CityLocationZ] + 0.5,
            25.0, CityLocations[locationid][CityLocationWorld], 1);
    }
    for (new playerid = 0; playerid < MAX_PLAYERS; playerid++) CityPlayerGPS[playerid] = CITY_INVALID_LOCATION;
    printf("[City] Los Santos initialized with %d points of interest.", sizeof(CityLocations));
    return 1;
}

stock City_Shutdown()
{
    for (new locationid = 0; locationid < sizeof(CityLocations); locationid++)
    {
        DestroyPickup(CityLocationPickup[locationid]);
        Delete3DTextLabel(CityLocationLabel[locationid]);
    }
    return 1;
}

stock City_ResetPlayer(playerid)
{
    if (!Player_IsValidId(playerid)) return 0;
    DisablePlayerCheckpoint(playerid);
    CityPlayerGPS[playerid] = CITY_INVALID_LOCATION;
    return 1;
}

stock City_OnPlayerSpawn(playerid)
{
    if (!City_CanPlayerUse(playerid)) return 0;
    if (City_IsLocationValid(CityPlayerGPS[playerid]))
    {
        SetPlayerCheckpoint(playerid, CityLocations[CityPlayerGPS[playerid]][CityLocationX], CityLocations[CityPlayerGPS[playerid]][CityLocationY], CityLocations[CityPlayerGPS[playerid]][CityLocationZ], CITY_CHECKPOINT_SIZE);
    }
    return 1;
}

stock City_ShowGPS(playerid)
{
    new list[768];
    if (!City_CanPlayerUse(playerid))
    {
        SendClientMessage(playerid, CITY_COLOR_MESSAGE, "Anda harus login dan memiliki character aktif untuk menggunakan GPS.");
        return 0;
    }
    for (new locationid = 0; locationid < sizeof(CityLocations); locationid++)
    {
        strcat(list, CityLocations[locationid][CityLocationName], sizeof(list));
        if (locationid < sizeof(CityLocations) - 1) strcat(list, "\n", sizeof(list));
    }
    ShowPlayerDialog(playerid, DIALOG_CITY_GPS, DIALOG_STYLE_LIST, "GPS Los Santos", list, "Pilih", "Tutup");
    return 1;
}

stock City_SetGPS(playerid, locationid)
{
    new message[96];
    if (!City_CanPlayerUse(playerid) || !City_IsLocationValid(locationid)) return 0;
    CityPlayerGPS[playerid] = locationid;
    SetPlayerCheckpoint(playerid, CityLocations[locationid][CityLocationX], CityLocations[locationid][CityLocationY], CityLocations[locationid][CityLocationZ], CITY_CHECKPOINT_SIZE);
    format(message, sizeof(message), "GPS: tujuan dipasang ke %s.", CityLocations[locationid][CityLocationName]);
    SendClientMessage(playerid, CITY_COLOR_SUCCESS, message);
    return 1;
}

stock City_ClearGPS(playerid)
{
    if (!City_CanPlayerUse(playerid)) return 0;
    if (!City_IsLocationValid(CityPlayerGPS[playerid]))
    {
        SendClientMessage(playerid, CITY_COLOR_MESSAGE, "GPS: tidak ada tujuan aktif.");
        return 1;
    }
    DisablePlayerCheckpoint(playerid);
    CityPlayerGPS[playerid] = CITY_INVALID_LOCATION;
    SendClientMessage(playerid, CITY_COLOR_MESSAGE, "GPS: tujuan dibatalkan.");
    return 1;
}

stock City_HandleGPSDialog(playerid, response, listitem)
{
    if (!City_CanPlayerUse(playerid)) return 1;
    if (!response) return 1;
    if (!City_IsLocationValid(listitem))
    {
        SendClientMessage(playerid, CITY_COLOR_MESSAGE, "GPS: tujuan tidak valid.");
        return 1;
    }
    City_SetGPS(playerid, listitem);
    return 1;
}

stock City_HandleDialog(playerid, dialogid, response, listitem)
{
    if (dialogid != DIALOG_CITY_GPS) return 0;
    City_HandleGPSDialog(playerid, response, listitem);
    return 1;
}

stock City_ShowWhereAmI(playerid)
{
    new area[CITY_AREA_NAME_LENGTH], locationName[CITY_LOCATION_NAME_LENGTH], message[128];
    new nearest;
    if (!City_CanPlayerUse(playerid)) return 0;
    City_GetAreaName(playerid, area, sizeof(area));
    nearest = City_GetNearestLocation(playerid);
    if (City_IsLocationValid(nearest))
    {
        City_GetLocationName(nearest, locationName, sizeof(locationName));
        format(message, sizeof(message), "Lokasi: %s | Titik kota terdekat: %s", area, locationName);
    }
    else format(message, sizeof(message), "Lokasi: %s", area);
    SendClientMessage(playerid, CITY_COLOR_MESSAGE, message);
    return 1;
}

stock City_ShowInformation(playerid)
{
    new area[CITY_AREA_NAME_LENGTH], message[144];
    if (!City_CanPlayerUse(playerid)) return 0;
    City_GetAreaName(playerid, area, sizeof(area));
    SendClientMessage(playerid, CITY_COLOR_MESSAGE, "Los Santos | Status: Kota utama");
    format(message, sizeof(message), "Character: %s | Lokasi: %s | GPS: %s", PlayerData[playerid][PlayerCharacterName], area, City_IsLocationValid(CityPlayerGPS[playerid]) ? ("aktif") : ("tidak aktif"));
    SendClientMessage(playerid, CITY_COLOR_MESSAGE, message);
    return 1;
}

stock City_HandleCommand(playerid, const cmdtext[])
{
    if (strcmp(cmdtext, "/city", true) != 0 && strcmp(cmdtext, "/gps", true) != 0 &&
        strcmp(cmdtext, "/gpsclear", true) != 0 && strcmp(cmdtext, "/whereami", true) != 0) return 0;
    if (!City_CanPlayerUse(playerid))
    {
        SendClientMessage(playerid, CITY_COLOR_MESSAGE, "Command kota hanya tersedia setelah login dan character aktif.");
        return 1;
    }
    if (strcmp(cmdtext, "/city", true) == 0) return City_ShowInformation(playerid);
    if (strcmp(cmdtext, "/gps", true) == 0) return City_ShowGPS(playerid);
    if (strcmp(cmdtext, "/gpsclear", true) == 0) return City_ClearGPS(playerid);
    return City_ShowWhereAmI(playerid);
}

stock City_OnPlayerEnterCheckpoint(playerid)
{
    new locationid, message[96];
    if (!City_CanPlayerUse(playerid)) return 0;
    locationid = CityPlayerGPS[playerid];
    if (!City_IsLocationValid(locationid)) return 0;
    if (!IsPlayerInRangeOfPoint(playerid, CITY_CHECKPOINT_SIZE + 2.0, CityLocations[locationid][CityLocationX], CityLocations[locationid][CityLocationY], CityLocations[locationid][CityLocationZ])) return 0;
    DisablePlayerCheckpoint(playerid);
    CityPlayerGPS[playerid] = CITY_INVALID_LOCATION;
    format(message, sizeof(message), "GPS: Anda telah tiba di %s.", CityLocations[locationid][CityLocationName]);
    SendClientMessage(playerid, CITY_COLOR_SUCCESS, message);
    return 1;
}

stock City_OnPlayerPickUpPickup(playerid, pickupid)
{
    new message[112];
    if (!City_CanPlayerUse(playerid)) return 0;
    for (new locationid = 0; locationid < sizeof(CityLocations); locationid++)
    {
        if (CityLocationPickup[locationid] != pickupid) continue;
        format(message, sizeof(message), "%s: fitur ini akan tersedia pada tahap berikutnya.", CityLocations[locationid][CityLocationName]);
        SendClientMessage(playerid, CITY_COLOR_MESSAGE, message);
        return 1;
    }
    return 0;
}
