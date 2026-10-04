#if defined _RP_RADIAL_MENU_INCLUDED
    #endinput
#endif
#define _RP_RADIAL_MENU_INCLUDED

#define RADIAL_OPEN_KEY        KEY_NO
#define RADIAL_COLOR_ACTIVE    (0xE8C878FF)
#define RADIAL_COLOR_LOCKED    (0x727986FF)
#define RADIAL_COLOR_CLOSE     (0xE06C75FF)
#define RADIAL_COLOR_TEXT      (0xF4F1E8FF)
#define RADIAL_COLOR_MUTED     (0xA9ADB6FF)

// Decorative layers live in the same collection as the buttons so the whole
// interface can be shown, hidden, and destroyed in one small loop.
enum E_RADIAL_TEXT
{
    RADIAL_SHADOW,
    RADIAL_PANEL,
    RADIAL_ACCENT,
    RADIAL_HEADER,
    RADIAL_SUBTITLE,
    RADIAL_CHARACTER,
    RADIAL_ACTION,
    RADIAL_PHONE,
    RADIAL_VEHICLE,
    RADIAL_INVENTORY,
    RADIAL_CLOSE,
    RADIAL_FOOTER,
    RADIAL_TEXT_COUNT
};

new PlayerText:RadialText[MAX_PLAYERS][E_RADIAL_TEXT];
new bool:RadialCreated[MAX_PLAYERS], bool:RadialMenuOpen[MAX_PLAYERS];

stock bool:Radial_CanUse(playerid)
{
    return Player_IsOnline(playerid) && PlayerData[playerid][AccountLoggedIn] && PlayerData[playerid][PlayerHasCharacter] && PlayerOnboardingState[playerid] == ONBOARDING_COMPLETE;
}

stock Radial_CreatePanel(playerid, E_RADIAL_TEXT:index, Float:x, Float:y, Float:right, Float:bottom, color)
{
    RadialText[playerid][index] = CreatePlayerTextDraw(playerid, x, y, "_");
    PlayerTextDrawLetterSize(playerid, RadialText[playerid][index], 0.0, 1.0);
    PlayerTextDrawTextSize(playerid, RadialText[playerid][index], right, bottom);
    PlayerTextDrawUseBox(playerid, RadialText[playerid][index], 1);
    PlayerTextDrawBoxColor(playerid, RadialText[playerid][index], color);
    PlayerTextDrawColor(playerid, RadialText[playerid][index], 0x00000000);
    return 1;
}

stock Radial_CreateLabel(playerid, E_RADIAL_TEXT:index, Float:x, Float:y, const label[], color, Float:size_x, Float:size_y, font = 2)
{
    RadialText[playerid][index] = CreatePlayerTextDraw(playerid, x, y, label);
    PlayerTextDrawAlignment(playerid, RadialText[playerid][index], 2);
    PlayerTextDrawLetterSize(playerid, RadialText[playerid][index], size_x, size_y);
    PlayerTextDrawFont(playerid, RadialText[playerid][index], font);
    PlayerTextDrawColor(playerid, RadialText[playerid][index], color);
    PlayerTextDrawBackgroundColor(playerid, RadialText[playerid][index], 0x000000FF);
    PlayerTextDrawSetShadow(playerid, RadialText[playerid][index], 0);
    PlayerTextDrawSetOutline(playerid, RadialText[playerid][index], 1);
    return 1;
}

stock Radial_CreateButton(playerid, E_RADIAL_TEXT:index, Float:x, Float:y, const label[], color, boxcolor)
{
    Radial_CreateLabel(playerid, index, x, y, label, color, 0.25, 1.15);
    PlayerTextDrawTextSize(playerid, RadialText[playerid][index], 128.0, 39.0);
    PlayerTextDrawUseBox(playerid, RadialText[playerid][index], 1);
    PlayerTextDrawBoxColor(playerid, RadialText[playerid][index], boxcolor);
    PlayerTextDrawSetSelectable(playerid, RadialText[playerid][index], 1);
    return 1;
}

stock Radial_Create(playerid)
{
    if (RadialCreated[playerid]) return 1;

    // A restrained charcoal-and-gold identity keeps the menu readable over
    // every part of the game world while giving it a distinct roleplay feel.
    Radial_CreatePanel(playerid, RADIAL_SHADOW, 166.0, 87.0, 476.0, 389.0, 0x00000088);
    Radial_CreatePanel(playerid, RADIAL_PANEL, 170.0, 83.0, 470.0, 383.0, 0x12151BEF);
    Radial_CreatePanel(playerid, RADIAL_ACCENT, 170.0, 83.0, 470.0, 86.0, 0xE8C878FF);

    Radial_CreateLabel(playerid, RADIAL_HEADER, 320.0, 103.0, "~y~SAN ANDREAS ~w~ROLEPLAY", RADIAL_COLOR_TEXT, 0.34, 1.55);
    Radial_CreateLabel(playerid, RADIAL_SUBTITLE, 320.0, 126.0, "QUICK ACCESS  /  PLAYER MENU", RADIAL_COLOR_MUTED, 0.18, 0.85);

    Radial_CreateButton(playerid, RADIAL_CHARACTER, 245.0, 164.0, "~y~IDENTITAS~n~~w~PROFIL KARAKTER", RADIAL_COLOR_TEXT, 0x292E38FF);
    Radial_CreateButton(playerid, RADIAL_ACTION, 395.0, 164.0, "~w~AKSI~n~~r~SEGERA HADIR", RADIAL_COLOR_LOCKED, 0x20242CCC);
    Radial_CreateButton(playerid, RADIAL_PHONE, 245.0, 224.0, "~w~TELEPON~n~~r~SEGERA HADIR", RADIAL_COLOR_LOCKED, 0x20242CCC);
    Radial_CreateButton(playerid, RADIAL_VEHICLE, 395.0, 224.0, "~w~KENDARAAN~n~~r~SEGERA HADIR", RADIAL_COLOR_LOCKED, 0x20242CCC);
    Radial_CreateButton(playerid, RADIAL_INVENTORY, 245.0, 284.0, "~y~INVENTARIS~n~~w~BARANG ANDA", RADIAL_COLOR_TEXT, 0x292E38FF);
    Radial_CreateButton(playerid, RADIAL_CLOSE, 395.0, 284.0, "~r~TUTUP~n~~w~KEMBALI BERMAIN", RADIAL_COLOR_TEXT, 0x3B2529FF);

    Radial_CreateLabel(playerid, RADIAL_FOOTER, 320.0, 350.0, "PILIH MENU  |  TEKAN N UNTUK MENUTUP", RADIAL_COLOR_MUTED, 0.17, 0.8);
    RadialCreated[playerid] = true;
    return 1;
}

stock Radial_Close(playerid)
{
    if (!RadialMenuOpen[playerid]) return 0;
    for (new i = 0; i < RADIAL_TEXT_COUNT; i++) PlayerTextDrawHide(playerid, RadialText[playerid][E_RADIAL_TEXT:i]);
    CancelSelectTextDraw(playerid);
    RadialMenuOpen[playerid] = false;
    return 1;
}

stock Radial_Open(playerid)
{
    if (!Radial_CanUse(playerid)) return 0;
    Radial_Create(playerid);
    for (new i = 0; i < RADIAL_TEXT_COUNT; i++) PlayerTextDrawShow(playerid, RadialText[playerid][E_RADIAL_TEXT:i]);
    SelectTextDraw(playerid, RADIAL_COLOR_ACTIVE);
    RadialMenuOpen[playerid] = true;
    return 1;
}

stock Radial_Toggle(playerid)
{
    if (RadialMenuOpen[playerid]) return Radial_Close(playerid);
    return Radial_Open(playerid);
}

stock Radial_Destroy(playerid)
{
    if (!Player_IsValidId(playerid)) return 0;
    if (RadialMenuOpen[playerid]) Radial_Close(playerid);
    if (RadialCreated[playerid])
    {
        for (new i = 0; i < RADIAL_TEXT_COUNT; i++) PlayerTextDrawDestroy(playerid, RadialText[playerid][E_RADIAL_TEXT:i]);
    }
    RadialCreated[playerid] = false;
    RadialMenuOpen[playerid] = false;
    return 1;
}

stock Radial_HandleKey(playerid, newkeys, oldkeys)
{
    if ((newkeys & RADIAL_OPEN_KEY) && !(oldkeys & RADIAL_OPEN_KEY))
    {
        if (!Radial_CanUse(playerid)) return 0;
        Radial_Toggle(playerid);
        return 1;
    }
    return 0;
}

stock Radial_HandleClick(playerid, PlayerText:playertextid)
{
    if (!RadialMenuOpen[playerid]) return 0;
    if (playertextid == PlayerText:INVALID_TEXT_DRAW)
    {
        Radial_Close(playerid);
        return 1;
    }
    if (playertextid == RadialText[playerid][RADIAL_INVENTORY])
    {
        Radial_Close(playerid);
        Inventory_Show(playerid);
        return 1;
    }
    if (playertextid == RadialText[playerid][RADIAL_CHARACTER])
    {
        Radial_Close(playerid);
        SendClientMessage(playerid, COLOR_SERVER_MESSAGE, "Gunakan /charinfo untuk melihat identitas character.");
        return 1;
    }
    if (playertextid == RadialText[playerid][RADIAL_CLOSE])
    {
        Radial_Close(playerid);
        return 1;
    }
    if (playertextid == RadialText[playerid][RADIAL_ACTION] || playertextid == RadialText[playerid][RADIAL_PHONE] || playertextid == RadialText[playerid][RADIAL_VEHICLE])
    {
        SendClientMessage(playerid, COLOR_SERVER_MESSAGE, "Fitur ini belum tersedia.");
        return 1;
    }
    return 1;
}
