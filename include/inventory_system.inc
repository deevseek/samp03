#if defined _RP_INVENTORY_SYSTEM_INCLUDED
    #endinput
#endif
#define _RP_INVENTORY_SYSTEM_INCLUDED

#define INVENTORY_MAX_SLOTS (20)
#define STARTER_CASH_AMOUNT (5000)
#define DIALOG_INVENTORY_MAIN (1300)

enum E_ITEM_TYPE { ITEM_NONE, ITEM_CASH };

new InventoryItem[MAX_PLAYERS][INVENTORY_MAX_SLOTS];
new InventoryQuantity[MAX_PLAYERS][INVENTORY_MAX_SLOTS];
new bool:InventoryLoaded[MAX_PLAYERS];

stock bool:Inventory_IsValidItem(itemType) return (itemType > ITEM_NONE && itemType <= ITEM_CASH);
stock Inventory_GetItemName(itemType, output[], size) { if (itemType == ITEM_CASH) format(output,size,"Cash"); else format(output,size,"Empty"); return 1; }

stock Inventory_Reset(playerid)
{
    if (!Player_IsValidId(playerid)) return 0;
    for (new slot=0; slot<INVENTORY_MAX_SLOTS; slot++) { InventoryItem[playerid][slot]=ITEM_NONE; InventoryQuantity[playerid][slot]=0; }
    InventoryLoaded[playerid]=false;
    return 1;
}

stock bool:Inventory_InitializeDatabase()
{
    new DBResult:r;
    r=db_query(CharacterDatabase,"CREATE TABLE IF NOT EXISTS character_inventory (id INTEGER PRIMARY KEY AUTOINCREMENT, character_id INTEGER NOT NULL, slot INTEGER NOT NULL, item_type INTEGER NOT NULL, quantity INTEGER NOT NULL DEFAULT 0, UNIQUE(character_id,slot), FOREIGN KEY(character_id) REFERENCES characters(character_id))");
    if (_:r==0) return false; db_free_result(r);
    r=db_query(CharacterDatabase,"CREATE UNIQUE INDEX IF NOT EXISTS idx_inventory_character_item ON character_inventory(character_id,item_type) WHERE item_type <> 0");
    if (_:r==0) return false; db_free_result(r); return true;
}

stock Inventory_SaveSlot(playerid, slot)
{
    new query[256],DBResult:r;
    if (!InventoryLoaded[playerid] || slot<0 || slot>=INVENTORY_MAX_SLOTS || PlayerCharacterId[playerid]<=0) return 0;
    format(query,sizeof(query),"INSERT OR REPLACE INTO character_inventory (id,character_id,slot,item_type,quantity) VALUES ((SELECT id FROM character_inventory WHERE character_id=%d AND slot=%d),%d,%d,%d,%d)",PlayerCharacterId[playerid],slot,PlayerCharacterId[playerid],slot,InventoryItem[playerid][slot],InventoryQuantity[playerid][slot]);
    r=db_query(CharacterDatabase,query); if (_:r==0) return 0; db_free_result(r); return 1;
}

stock Inventory_Save(playerid) { if (!InventoryLoaded[playerid]) return 0; for(new slot=0;slot<INVENTORY_MAX_SLOTS;slot++) if(InventoryItem[playerid][slot]!=ITEM_NONE) Inventory_SaveSlot(playerid,slot); return 1; }
stock Inventory_GetCash(playerid) { if(!InventoryLoaded[playerid]) return 0; for(new slot=0;slot<INVENTORY_MAX_SLOTS;slot++) if(InventoryItem[playerid][slot]==ITEM_CASH) return InventoryQuantity[playerid][slot]; return 0; }

stock Inventory_SyncCash(playerid)
{
    if (!InventoryLoaded[playerid]) return 0;
    PlayerData[playerid][PlayerMoney]=Inventory_GetCash(playerid);
    ResetPlayerMoney(playerid); GivePlayerMoney(playerid,PlayerData[playerid][PlayerMoney]);
    return 1;
}

stock bool:Inventory_SetCash(playerid, amount)
{
    if (!InventoryLoaded[playerid] || amount<0) return false;
    new slot=-1;
    for(new i=0;i<INVENTORY_MAX_SLOTS;i++) { if(InventoryItem[playerid][i]==ITEM_CASH){slot=i;break;} if(slot==-1&&InventoryItem[playerid][i]==ITEM_NONE)slot=i; }
    if(slot==-1)return false;
    InventoryItem[playerid][slot]=ITEM_CASH; InventoryQuantity[playerid][slot]=amount;
    if(!Inventory_SaveSlot(playerid,slot))return false;
    Inventory_SyncCash(playerid); return true;
}
stock bool:Inventory_GiveCash(playerid, amount) { if(amount<0)return false; return Inventory_SetCash(playerid,Inventory_GetCash(playerid)+amount); }
stock bool:Inventory_TakeCash(playerid, amount) { if(amount<0||Inventory_GetCash(playerid)<amount)return false; return Inventory_SetCash(playerid,Inventory_GetCash(playerid)-amount); }
stock bool:Inventory_AddItem(playerid,itemType,quantity) { if(itemType==ITEM_CASH)return Inventory_GiveCash(playerid,quantity); return false; }

stock bool:Inventory_Load(playerid)
{
    new query[256],value[24],DBResult:r,slot,item,quantity,hasCash=false;
    if(PlayerCharacterId[playerid]<=0)return false;
    Inventory_Reset(playerid);
    format(query,sizeof(query),"SELECT slot,item_type,quantity FROM character_inventory WHERE character_id=%d ORDER BY slot",PlayerCharacterId[playerid]);
    r=db_query(CharacterDatabase,query); if(_:r==0)return false;
    for(new row=0;row<db_num_rows(r);row++)
    {
        db_get_field_assoc(r,"slot",value,sizeof(value)); slot=strval(value);
        db_get_field_assoc(r,"item_type",value,sizeof(value)); item=strval(value);
        db_get_field_assoc(r,"quantity",value,sizeof(value)); quantity=strval(value);
        if(slot>=0&&slot<INVENTORY_MAX_SLOTS&&Inventory_IsValidItem(item)&&quantity>=0){InventoryItem[playerid][slot]=item;InventoryQuantity[playerid][slot]=quantity;if(item==ITEM_CASH)hasCash=true;}
        db_next_row(r);
    }
    db_free_result(r); InventoryLoaded[playerid]=true;
    // One-time legacy migration: the compatibility money value becomes inventory cash.
    if(!hasCash && !Inventory_SetCash(playerid,PlayerData[playerid][PlayerMoney])) { Inventory_Reset(playerid); return false; }
    Inventory_SyncCash(playerid); return true;
}

stock Inventory_FormatMoney(amount,output[],size)
{
    if(amount>=1000000) format(output,size,"$%d,%03d,%03d",amount/1000000,(amount/1000)%1000,amount%1000);
    else if(amount>=1000) format(output,size,"$%d,%03d",amount/1000,amount%1000);
    else format(output,size,"$%d",amount);
    return 1;
}

stock Inventory_Show(playerid)
{
    new body[512]="Item\tJumlah\n",name[24],money[32],used;
    if(!InventoryLoaded[playerid])return 0;
    for(new slot=0;slot<INVENTORY_MAX_SLOTS;slot++) if(InventoryItem[playerid][slot]!=ITEM_NONE){Inventory_GetItemName(InventoryItem[playerid][slot],name,sizeof(name));if(InventoryItem[playerid][slot]==ITEM_CASH)Inventory_FormatMoney(InventoryQuantity[playerid][slot],money,sizeof(money));else format(money,sizeof(money),"%d",InventoryQuantity[playerid][slot]);format(body,sizeof(body),"%s%s\t%s\n",body,name,money);used++;}
    format(body,sizeof(body),"%s\nSlots Used: %d/%d",body,used,INVENTORY_MAX_SLOTS);
    ShowPlayerDialog(playerid,DIALOG_INVENTORY_MAIN,DIALOG_STYLE_TABLIST_HEADERS,"INVENTORY",body,"TUTUP",""); return 1;
}
stock Inventory_HandleDialog(playerid,dialogid) { if(dialogid!=DIALOG_INVENTORY_MAIN)return 0; return 1; }
