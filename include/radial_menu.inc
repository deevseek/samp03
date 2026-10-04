#if defined _RP_RADIAL_MENU_INCLUDED
    #endinput
#endif
#define _RP_RADIAL_MENU_INCLUDED

#define RADIAL_OPEN_KEY KEY_NO
#define RADIAL_COLOR_ACTIVE (0x72C96BFF)
#define RADIAL_COLOR_LOCKED (0x777777FF)
#define RADIAL_COLOR_CLOSE (0xCC5555FF)

enum E_RADIAL_TEXT { RADIAL_CHARACTER,RADIAL_ACTION,RADIAL_PHONE,RADIAL_CENTER,RADIAL_VEHICLE,RADIAL_INVENTORY,RADIAL_CLOSE,RADIAL_TEXT_COUNT };
new PlayerText:RadialText[MAX_PLAYERS][E_RADIAL_TEXT];
new bool:RadialCreated[MAX_PLAYERS],bool:RadialMenuOpen[MAX_PLAYERS];

stock bool:Radial_CanUse(playerid){return Player_IsOnline(playerid)&&PlayerData[playerid][AccountLoggedIn]&&PlayerData[playerid][PlayerHasCharacter]&&PlayerOnboardingState[playerid]==ONBOARDING_COMPLETE;}
stock Radial_CreateButton(playerid,E_RADIAL_TEXT:index,Float:x,Float:y,const label[],color)
{
    RadialText[playerid][index]=CreatePlayerTextDraw(playerid,x,y,label);PlayerTextDrawAlignment(playerid,RadialText[playerid][index],2);PlayerTextDrawLetterSize(playerid,RadialText[playerid][index],0.25,1.2);PlayerTextDrawTextSize(playerid,RadialText[playerid][index],22.0,105.0);PlayerTextDrawUseBox(playerid,RadialText[playerid][index],1);PlayerTextDrawBoxColor(playerid,RadialText[playerid][index],0x111111CC);PlayerTextDrawColor(playerid,RadialText[playerid][index],color);PlayerTextDrawSetOutline(playerid,RadialText[playerid][index],1);PlayerTextDrawSetSelectable(playerid,RadialText[playerid][index],1);return 1;
}
stock Radial_Create(playerid){if(RadialCreated[playerid])return 1;Radial_CreateButton(playerid,RADIAL_CHARACTER,320.0,135.0,"CHARACTER",RADIAL_COLOR_ACTIVE);Radial_CreateButton(playerid,RADIAL_ACTION,205.0,205.0,"ACTION~n~~r~LOCKED",RADIAL_COLOR_LOCKED);Radial_CreateButton(playerid,RADIAL_PHONE,435.0,205.0,"PHONE~n~~r~LOCKED",RADIAL_COLOR_LOCKED);Radial_CreateButton(playerid,RADIAL_CENTER,320.0,220.0,"~b~PLAYER",0xFFFFFFFF);Radial_CreateButton(playerid,RADIAL_VEHICLE,205.0,285.0,"VEHICLE~n~~r~LOCKED",RADIAL_COLOR_LOCKED);Radial_CreateButton(playerid,RADIAL_INVENTORY,435.0,285.0,"INVENTORY",RADIAL_COLOR_ACTIVE);Radial_CreateButton(playerid,RADIAL_CLOSE,320.0,355.0,"CLOSE",RADIAL_COLOR_CLOSE);RadialCreated[playerid]=true;return 1;}
stock Radial_Close(playerid){if(!RadialMenuOpen[playerid])return 0;for(new i=0;i<RADIAL_TEXT_COUNT;i++)PlayerTextDrawHide(playerid,RadialText[playerid][E_RADIAL_TEXT:i]);CancelSelectTextDraw(playerid);RadialMenuOpen[playerid]=false;return 1;}
stock Radial_Open(playerid){if(!Radial_CanUse(playerid))return 0;Radial_Create(playerid);for(new i=0;i<RADIAL_TEXT_COUNT;i++)PlayerTextDrawShow(playerid,RadialText[playerid][E_RADIAL_TEXT:i]);SelectTextDraw(playerid,0x72C96BFF);RadialMenuOpen[playerid]=true;return 1;}
stock Radial_Toggle(playerid){if(RadialMenuOpen[playerid])return Radial_Close(playerid);return Radial_Open(playerid);}
stock Radial_Destroy(playerid){if(!Player_IsValidId(playerid))return 0;if(RadialMenuOpen[playerid])Radial_Close(playerid);if(RadialCreated[playerid])for(new i=0;i<RADIAL_TEXT_COUNT;i++)PlayerTextDrawDestroy(playerid,RadialText[playerid][E_RADIAL_TEXT:i]);RadialCreated[playerid]=false;RadialMenuOpen[playerid]=false;return 1;}
stock Radial_HandleKey(playerid,newkeys,oldkeys){if((newkeys&RADIAL_OPEN_KEY)&&!(oldkeys&RADIAL_OPEN_KEY)){if(!Radial_CanUse(playerid))return 0;Radial_Toggle(playerid);return 1;}return 0;}
stock Radial_HandleClick(playerid,PlayerText:playertextid)
{
    if(!RadialMenuOpen[playerid])return 0;
    if(playertextid==PlayerText:INVALID_TEXT_DRAW){Radial_Close(playerid);return 1;}
    if(playertextid==RadialText[playerid][RADIAL_INVENTORY]){Radial_Close(playerid);Inventory_Show(playerid);return 1;}
    if(playertextid==RadialText[playerid][RADIAL_CHARACTER]){Radial_Close(playerid);SendClientMessage(playerid,COLOR_SERVER_MESSAGE,"Gunakan /charinfo untuk melihat identitas character.");return 1;}
    if(playertextid==RadialText[playerid][RADIAL_CLOSE]){Radial_Close(playerid);return 1;}
    for(new i=0;i<RADIAL_TEXT_COUNT;i++)if(playertextid==RadialText[playerid][E_RADIAL_TEXT:i]){SendClientMessage(playerid,COLOR_SERVER_MESSAGE,"Fitur ini belum tersedia.");return 1;}
    Radial_Close(playerid);return 1;
}
