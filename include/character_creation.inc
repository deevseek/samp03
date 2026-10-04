#if defined _RP_CHARACTER_CREATION_INCLUDED
    #endinput
#endif
#define _RP_CHARACTER_CREATION_INCLUDED

#define DIALOG_CHARACTER_INTRO (1200)
#define DIALOG_CHARACTER_NAME (1201)
#define DIALOG_CHARACTER_GENDER (1202)
#define DIALOG_CHARACTER_BIRTHPLACE (1203)
#define DIALOG_CHARACTER_BIRTHDATE (1204)
#define DIALOG_CHARACTER_HEIGHT (1205)
#define DIALOG_CHARACTER_SKIN (1206)
#define DIALOG_CHARACTER_CONFIRM (1207)

enum E_ONBOARDING_STATE { ONBOARDING_NONE,ONBOARDING_ACCOUNT,ONBOARDING_CHARACTER_INTRO,ONBOARDING_CHARACTER_NAME,ONBOARDING_CHARACTER_GENDER,ONBOARDING_CHARACTER_BIRTHPLACE,ONBOARDING_CHARACTER_BIRTHDATE,ONBOARDING_CHARACTER_HEIGHT,ONBOARDING_CHARACTER_SKIN,ONBOARDING_CHARACTER_CONFIRM,ONBOARDING_COMPLETE };
new E_ONBOARDING_STATE:PlayerOnboardingState[MAX_PLAYERS];
new PendingCharacterName[MAX_PLAYERS][MAX_PLAYER_NAME+1];
new PendingCharacterGender[MAX_PLAYERS],PendingCharacterBirthplace[MAX_PLAYERS],PendingCharacterDay[MAX_PLAYERS],PendingCharacterMonth[MAX_PLAYERS],PendingCharacterYear[MAX_PLAYERS],PendingCharacterHeight[MAX_PLAYERS],PendingCharacterSkin[MAX_PLAYERS];
new const MaleStarterSkins[]={20,21,22,23,29,60};
new const FemaleStarterSkins[]={40,41,55,56,69,90};

stock CharacterCreation_Reset(playerid){if(!Player_IsValidId(playerid))return 0;PlayerOnboardingState[playerid]=ONBOARDING_NONE;PendingCharacterName[playerid][0]=EOS;PendingCharacterGender[playerid]=0;PendingCharacterBirthplace[playerid]=0;PendingCharacterDay[playerid]=0;PendingCharacterMonth[playerid]=0;PendingCharacterYear[playerid]=0;PendingCharacterHeight[playerid]=0;PendingCharacterSkin[playerid]=0;return 1;}
stock bool:CharacterCreation_CanUse(playerid){return Player_IsOnline(playerid)&&PlayerData[playerid][AccountLoggedIn]&&!PlayerData[playerid][PlayerHasCharacter];}
stock CharacterCreation_ShowIntro(playerid){if(!CharacterCreation_CanUse(playerid))return 0;PlayerOnboardingState[playerid]=ONBOARDING_CHARACTER_INTRO;ShowPlayerDialog(playerid,DIALOG_CHARACTER_INTRO,DIALOG_STYLE_MSGBOX,"LOS SANTOS ROLEPLAY","Selamat datang.\n\nSebelum memasuki kota, silakan lengkapi identitas character Anda.\nData berikut adalah DATA CHARACTER / IC, bukan data pribadi Anda di dunia nyata.","Mulai","Keluar");return 1;}
stock CharacterCreation_ShowName(playerid){PlayerOnboardingState[playerid]=ONBOARDING_CHARACTER_NAME;ShowPlayerDialog(playerid,DIALOG_CHARACTER_NAME,DIALOG_STYLE_INPUT,"PENDAFTARAN WARGA | 1/6 - Nama Character","Masukkan nama lengkap character.\nFormat: Firstname_Lastname\nContoh: William_Tuckman\n\nNama harus terlihat seperti nama manusia yang realistis.","Lanjut","Keluar");return 1;}
stock CharacterCreation_ShowGender(playerid){PlayerOnboardingState[playerid]=ONBOARDING_CHARACTER_GENDER;ShowPlayerDialog(playerid,DIALOG_CHARACTER_GENDER,DIALOG_STYLE_LIST,"PENDAFTARAN WARGA | 2/6 - Jenis Kelamin","Laki-laki\nPerempuan","Pilih","Kembali");return 1;}
stock CharacterCreation_ShowBirthplace(playerid){PlayerOnboardingState[playerid]=ONBOARDING_CHARACTER_BIRTHPLACE;ShowPlayerDialog(playerid,DIALOG_CHARACTER_BIRTHPLACE,DIALOG_STYLE_LIST,"PENDAFTARAN WARGA | 3/6 - Tempat Lahir","Los Santos\nSan Fierro\nLas Venturas\nRed County\nBone County\nWhetstone\nLuar San Andreas","Pilih","Kembali");return 1;}
stock CharacterCreation_ShowBirthdate(playerid){PlayerOnboardingState[playerid]=ONBOARDING_CHARACTER_BIRTHDATE;ShowPlayerDialog(playerid,DIALOG_CHARACTER_BIRTHDATE,DIALOG_STYLE_INPUT,"PENDAFTARAN WARGA | 4/6 - Tanggal Lahir","Masukkan tanggal lahir CHARACTER.\nFormat: DD/MM/YYYY\nContoh: 14/08/1998\nUsia character harus 18 sampai 60 tahun.","Lanjut","Kembali");return 1;}
stock CharacterCreation_ShowHeight(playerid){PlayerOnboardingState[playerid]=ONBOARDING_CHARACTER_HEIGHT;ShowPlayerDialog(playerid,DIALOG_CHARACTER_HEIGHT,DIALOG_STYLE_INPUT,"PENDAFTARAN WARGA | 5/6 - Tinggi Badan","Masukkan tinggi character dalam centimeter (150-210).\nContoh: 175","Lanjut","Kembali");return 1;}
stock CharacterCreation_ShowSkin(playerid){PlayerOnboardingState[playerid]=ONBOARDING_CHARACTER_SKIN;if(PendingCharacterGender[playerid]==CHARACTER_GENDER_MALE)ShowPlayerDialog(playerid,DIALOG_CHARACTER_SKIN,DIALOG_STYLE_LIST,"PENDAFTARAN WARGA | 6/6 - Penampilan","Skin 20\nSkin 21\nSkin 22\nSkin 23\nSkin 29\nSkin 60","Pilih","Kembali");else ShowPlayerDialog(playerid,DIALOG_CHARACTER_SKIN,DIALOG_STYLE_LIST,"PENDAFTARAN WARGA | 6/6 - Penampilan","Skin 40\nSkin 41\nSkin 55\nSkin 56\nSkin 69\nSkin 90","Pilih","Kembali");return 1;}

stock bool:CharacterCreation_ParseDate(const text[],&day,&month,&year)
{
    if(strlen(text)!=10||text[2]!='/'||text[5]!='/')return false;
    for(new i=0;i<10;i++)if(i!=2&&i!=5&&(text[i]<'0'||text[i]>'9'))return false;
    day=(text[0]-'0')*10+text[1]-'0';month=(text[3]-'0')*10+text[4]-'0';year=(text[6]-'0')*1000+(text[7]-'0')*100+(text[8]-'0')*10+text[9]-'0';
    if(month<1||month>12||day<1)return false;
    new days=31;if(month==4||month==6||month==9||month==11)days=30;else if(month==2)days=((year%400==0)||(year%4==0&&year%100!=0))?29:28;
    if(day>days)return false;
    new age=Character_GetAge(day,month,year); return(age>=18&&age<=60);
}

stock bool:CharacterCreation_NameAvailable(playerid,const name[])
{
    new query[192],DBResult:r;
    if(!Character_IsNameValid(name))return false;
    format(query,sizeof(query),"SELECT character_id FROM characters WHERE character_name='%s' COLLATE NOCASE LIMIT 1",name);r=db_query(CharacterDatabase,query);
    if(_:r==0){printf("[Onboarding] Name lookup failure for player %d.",playerid);return false;}new bool:available=(db_num_rows(r)==0);db_free_result(r);return available;
}

stock CharacterCreation_ShowConfirm(playerid)
{
    new body[512],gender[24],place[32],displayName[MAX_PLAYER_NAME+1];
    format(displayName,sizeof(displayName),"%s",PendingCharacterName[playerid]);for(new i=0;i<strlen(displayName);i++)if(displayName[i]=='_')displayName[i]=' ';
    Character_GetGenderName(PendingCharacterGender[playerid],gender,sizeof(gender));Character_GetBirthplaceName(PendingCharacterBirthplace[playerid],place,sizeof(place));
    format(body,sizeof(body),"Nama: %s\nJenis Kelamin: %s\nTempat Lahir: %s\nTanggal Lahir: %02d/%02d/%04d\nUmur: %d Tahun\nTinggi: %d cm\nSkin: %d\n\nPastikan seluruh data character sudah benar.",displayName,gender,place,PendingCharacterDay[playerid],PendingCharacterMonth[playerid],PendingCharacterYear[playerid],Character_GetAge(PendingCharacterDay[playerid],PendingCharacterMonth[playerid],PendingCharacterYear[playerid]),PendingCharacterHeight[playerid],PendingCharacterSkin[playerid]);
    PlayerOnboardingState[playerid]=ONBOARDING_CHARACTER_CONFIRM;ShowPlayerDialog(playerid,DIALOG_CHARACTER_CONFIRM,DIALOG_STYLE_MSGBOX,"KONFIRMASI DATA CHARACTER",body,"DAFTAR","KEMBALI");return 1;
}

stock bool:CharacterCreation_Commit(playerid)
{
    new query[768],value[24],DBResult:r;
    if(!CharacterCreation_CanUse(playerid)||PlayerOnboardingState[playerid]!=ONBOARDING_CHARACTER_CONFIRM||!Character_IsNameValid(PendingCharacterName[playerid])||!CharacterCreation_NameAvailable(playerid,PendingCharacterName[playerid]))return false;
    format(query,sizeof(query),"SELECT character_id FROM characters WHERE account_id=%d LIMIT 1",PlayerData[playerid][AccountID]);r=db_query(CharacterDatabase,query);if(_:r==0)return false;if(db_num_rows(r)>0){db_free_result(r);return false;}db_free_result(r);
    r=db_query(CharacterDatabase,"BEGIN IMMEDIATE TRANSACTION");if(_:r==0)return false;db_free_result(r);
    format(query,sizeof(query),"INSERT INTO characters(account_identifier,account_id,character_name,money,skin,health,armour,spawn_x,spawn_y,spawn_z,spawn_angle,gender,birthplace,birth_day,birth_month,birth_year,height_cm,starter_pack_claimed,creation_version)VALUES('%s',%d,'%s',0,%d,100.0,0.0,%.4f,%.4f,%.4f,%.4f,%d,%d,%d,%d,%d,%d,0,1)",PlayerData[playerid][AccountUsername],PlayerData[playerid][AccountID],PendingCharacterName[playerid],PendingCharacterSkin[playerid],PLAYER_SPAWN_X,PLAYER_SPAWN_Y,PLAYER_SPAWN_Z,PLAYER_SPAWN_ANGLE,PendingCharacterGender[playerid],PendingCharacterBirthplace[playerid],PendingCharacterDay[playerid],PendingCharacterMonth[playerid],PendingCharacterYear[playerid],PendingCharacterHeight[playerid]);
    r=db_query(CharacterDatabase,query);if(_:r==0){db_query(CharacterDatabase,"ROLLBACK");return false;}db_free_result(r);
    r=db_query(CharacterDatabase,"SELECT last_insert_rowid() character_id");if(_:r==0||!db_num_rows(r)){if(_:r!=0)db_free_result(r);db_query(CharacterDatabase,"ROLLBACK");return false;}db_get_field_assoc(r,"character_id",value,sizeof(value));new characterId=strval(value);db_free_result(r);
    format(query,sizeof(query),"INSERT INTO character_inventory(character_id,slot,item_type,quantity)VALUES(%d,0,%d,%d)",characterId,ITEM_CASH,STARTER_CASH_AMOUNT);r=db_query(CharacterDatabase,query);if(_:r==0){db_query(CharacterDatabase,"ROLLBACK");return false;}db_free_result(r);
    format(query,sizeof(query),"UPDATE characters SET starter_pack_claimed=1,money=%d WHERE character_id=%d",STARTER_CASH_AMOUNT,characterId);r=db_query(CharacterDatabase,query);if(_:r==0){db_query(CharacterDatabase,"ROLLBACK");return false;}db_free_result(r);
    r=db_query(CharacterDatabase,"COMMIT");if(_:r==0){db_query(CharacterDatabase,"ROLLBACK");return false;}db_free_result(r);
    if(Character_Load(playerid)!=CHARACTER_LOAD_FOUND||!Inventory_Load(playerid)){printf("[Onboarding] Runtime load failed for new character %d.",characterId);return false;}
    PlayerOnboardingState[playerid]=ONBOARDING_COMPLETE;TogglePlayerSpectating(playerid,false);SpawnPlayer(playerid);return true;
}

stock CharacterCreation_HandleDialog(playerid,dialogid,response,listitem,const inputtext[])
{
    if(dialogid<DIALOG_CHARACTER_INTRO||dialogid>DIALOG_CHARACTER_CONFIRM)return 0;
    if(!CharacterCreation_CanUse(playerid)){SendClientMessage(playerid,COLOR_SERVER_MESSAGE,"Sesi pendaftaran character tidak valid.");return 1;}
    if(dialogid==DIALOG_CHARACTER_INTRO&&PlayerOnboardingState[playerid]==ONBOARDING_CHARACTER_INTRO){if(!response){Kick(playerid);return 1;}CharacterCreation_ShowName(playerid);return 1;}
    if(dialogid==DIALOG_CHARACTER_NAME&&PlayerOnboardingState[playerid]==ONBOARDING_CHARACTER_NAME){if(!response){Kick(playerid);return 1;}if(!Character_IsNameValid(inputtext)||!CharacterCreation_NameAvailable(playerid,inputtext)){SendClientMessage(playerid,COLOR_SERVER_MESSAGE,"Nama tidak valid atau sudah digunakan. Gunakan format Firstname_Lastname.");CharacterCreation_ShowName(playerid);return 1;}format(PendingCharacterName[playerid],MAX_PLAYER_NAME+1,"%s",inputtext);CharacterCreation_ShowGender(playerid);return 1;}
    if(dialogid==DIALOG_CHARACTER_GENDER&&PlayerOnboardingState[playerid]==ONBOARDING_CHARACTER_GENDER){if(!response){CharacterCreation_ShowName(playerid);return 1;}if(listitem<0||listitem>1){CharacterCreation_ShowGender(playerid);return 1;}PendingCharacterGender[playerid]=listitem+1;CharacterCreation_ShowBirthplace(playerid);return 1;}
    if(dialogid==DIALOG_CHARACTER_BIRTHPLACE&&PlayerOnboardingState[playerid]==ONBOARDING_CHARACTER_BIRTHPLACE){if(!response){CharacterCreation_ShowGender(playerid);return 1;}if(listitem<0||listitem>6){CharacterCreation_ShowBirthplace(playerid);return 1;}PendingCharacterBirthplace[playerid]=listitem+1;CharacterCreation_ShowBirthdate(playerid);return 1;}
    if(dialogid==DIALOG_CHARACTER_BIRTHDATE&&PlayerOnboardingState[playerid]==ONBOARDING_CHARACTER_BIRTHDATE){if(!response){CharacterCreation_ShowBirthplace(playerid);return 1;}if(!CharacterCreation_ParseDate(inputtext,PendingCharacterDay[playerid],PendingCharacterMonth[playerid],PendingCharacterYear[playerid])){SendClientMessage(playerid,COLOR_SERVER_MESSAGE,"Tanggal tidak valid atau usia character harus 18-60 tahun.");CharacterCreation_ShowBirthdate(playerid);return 1;}CharacterCreation_ShowHeight(playerid);return 1;}
    if(dialogid==DIALOG_CHARACTER_HEIGHT&&PlayerOnboardingState[playerid]==ONBOARDING_CHARACTER_HEIGHT){if(!response){CharacterCreation_ShowBirthdate(playerid);return 1;}if(!strlen(inputtext)){CharacterCreation_ShowHeight(playerid);return 1;}for(new i=0;i<strlen(inputtext);i++)if(inputtext[i]<'0'||inputtext[i]>'9'){CharacterCreation_ShowHeight(playerid);return 1;}PendingCharacterHeight[playerid]=strval(inputtext);if(PendingCharacterHeight[playerid]<150||PendingCharacterHeight[playerid]>210){SendClientMessage(playerid,COLOR_SERVER_MESSAGE,"Tinggi harus antara 150 dan 210 cm.");CharacterCreation_ShowHeight(playerid);return 1;}CharacterCreation_ShowSkin(playerid);return 1;}
    if(dialogid==DIALOG_CHARACTER_SKIN&&PlayerOnboardingState[playerid]==ONBOARDING_CHARACTER_SKIN){if(!response){CharacterCreation_ShowHeight(playerid);return 1;}if(listitem<0||listitem>=6){CharacterCreation_ShowSkin(playerid);return 1;}PendingCharacterSkin[playerid]=(PendingCharacterGender[playerid]==CHARACTER_GENDER_MALE)?MaleStarterSkins[listitem]:FemaleStarterSkins[listitem];CharacterCreation_ShowConfirm(playerid);return 1;}
    if(dialogid==DIALOG_CHARACTER_CONFIRM&&PlayerOnboardingState[playerid]==ONBOARDING_CHARACTER_CONFIRM){if(!response){CharacterCreation_ShowName(playerid);return 1;}if(!CharacterCreation_Commit(playerid)){SendClientMessage(playerid,COLOR_SERVER_MESSAGE,"Pembuatan character gagal. Silakan sambungkan kembali dan coba lagi.");if(PlayerData[playerid][PlayerHasCharacter])Kick(playerid);else CharacterCreation_ShowName(playerid);}return 1;}
    SendClientMessage(playerid,COLOR_SERVER_MESSAGE,"Tahap pendaftaran tidak sesuai. Silakan mulai kembali.");CharacterCreation_ShowIntro(playerid);return 1;
}
