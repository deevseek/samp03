#if defined _RP_ACCOUNT_SYSTEM_INCLUDED
    #endinput
#endif
#define _RP_ACCOUNT_SYSTEM_INCLUDED

// SHA256_PassHash is a built-in SA-MP native (0.3.7 R2+) and needs no plugin.
#define ACCOUNT_HASH_PREFIX       "sha256$"
#define ACCOUNT_MIN_PASSWORD      (6)
#define DIALOG_ACCOUNT_USERNAME   (1000)
#define DIALOG_ACCOUNT_LOGIN      (1001)
#define DIALOG_ACCOUNT_REGISTER   (1002)
#define DIALOG_ACCOUNT_CONFIRM    (1003)

new AccountPendingUsername[MAX_PLAYERS][MAX_PLAYER_NAME + 1];
new AccountPendingPassword[MAX_PLAYERS][65];

stock Account_Reset(playerid)
{
    if (!Player_IsValidId(playerid)) return 0;
    AccountPendingUsername[playerid][0] = EOS;
    AccountPendingPassword[playerid][0] = EOS;
    return 1;
}

stock bool:Account_IsUsernameValid(const username[])
{
    return Character_IsNameValid(username);
}

stock bool:Account_IsPasswordValid(const password[])
{
    new length = strlen(password);
    return (length >= ACCOUNT_MIN_PASSWORD && length <= 64);
}

stock Account_ShowUsernameDialog(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_ACCOUNT_USERNAME, DIALOG_STYLE_INPUT, "Account", "Masukkan username account Anda.", "Lanjut", "Keluar");
    return 1;
}

stock Account_BuildPasswordHash(const password[], const username[], output[], outputSize)
{
    new salt[48], digest[65];
    // The username is public but supplies a stable per-account salt; SHA256_PassHash
    // is provided by the SA-MP runtime rather than a custom cryptographic routine.
    format(salt, sizeof(salt), "samp03:%s", username);
    SHA256_PassHash(password, salt, digest, sizeof(digest));
    format(output, outputSize, "%s%s", ACCOUNT_HASH_PREFIX, digest);
    return 1;
}

stock bool:Account_VerifyPassword(const password[], const username[], const storedHash[])
{
    new calculatedHash[80];
    Account_BuildPasswordHash(password, username, calculatedHash, sizeof(calculatedHash));
    return strcmp(calculatedHash, storedHash, false) == 0;
}

stock Account_Authenticate(playerid, accountId, adminLevel, const username[])
{
    new query[128], DBResult:result;
    if (!Player_IsOnline(playerid)) return 0;
    PlayerData[playerid][AccountID] = accountId;
    PlayerData[playerid][AccountLoggedIn] = true;
    PlayerData[playerid][AccountAdminLevel] = adminLevel;
    format(PlayerData[playerid][AccountUsername], MAX_PLAYER_NAME + 1, "%s", username);
    PlayerData[playerid][PlayerAdminLevel] = adminLevel;
    if (!Character_Load(playerid))
    {
        printf("[Account] Character load failure for account ID %d.", accountId);
        SendClientMessage(playerid, COLOR_SERVER_MESSAGE, "Character tidak dapat dimuat. Silakan hubungi admin.");
        Character_Reset(playerid);
        PlayerData[playerid][AccountID] = 0;
        PlayerData[playerid][AccountLoggedIn] = false;
        PlayerData[playerid][AccountAdminLevel] = 0;
        PlayerData[playerid][AccountUsername][0] = EOS;
        PlayerData[playerid][PlayerAdminLevel] = 0;
        return 0;
    }
    format(query, sizeof(query), "UPDATE accounts SET last_login = CURRENT_TIMESTAMP WHERE id = %d", accountId);
    result = db_query(CharacterDatabase, query);
    if (_:result != 0) db_free_result(result);
    SendClientMessage(playerid, COLOR_SERVER_MESSAGE, "Login berhasil. Character aktif telah dimuat.");
    TogglePlayerSpectating(playerid, false);
    SpawnPlayer(playerid);
    return 1;
}

stock Account_CheckUsername(playerid, const username[])
{
    new query[192], DBResult:result;
    if (!Player_IsOnline(playerid) || !Account_IsUsernameValid(username) || !Character_IsDatabaseReady()) return 0;
    format(AccountPendingUsername[playerid], MAX_PLAYER_NAME + 1, "%s", username);
    format(query, sizeof(query), "SELECT id FROM accounts WHERE username = '%s' LIMIT 1", username);
    result = db_query(CharacterDatabase, query);
    if (_:result == 0) { printf("[Account] Account lookup query failure."); SendClientMessage(playerid, COLOR_SERVER_MESSAGE, "Database sedang tidak tersedia."); return 0; }
    if (db_num_rows(result) > 0)
    {
        db_free_result(result);
        ShowPlayerDialog(playerid, DIALOG_ACCOUNT_LOGIN, DIALOG_STYLE_PASSWORD, "Login", "Masukkan password account.", "Login", "Keluar");
        return 1;
    }
    db_free_result(result);
    ShowPlayerDialog(playerid, DIALOG_ACCOUNT_REGISTER, DIALOG_STYLE_PASSWORD, "Registrasi", "Account belum ditemukan. Silakan buat password (minimal 6 karakter).", "Daftar", "Keluar");
    return 1;
}

stock Account_Login(playerid, const password[])
{
    new query[192], value[80], username[MAX_PLAYER_NAME + 1], accountId, adminLevel, DBResult:result;
    if (!Player_IsOnline(playerid) || !Account_IsPasswordValid(password)) return 0;
    format(query, sizeof(query), "SELECT id, username, password_hash, admin_level FROM accounts WHERE username = '%s' LIMIT 1", AccountPendingUsername[playerid]);
    result = db_query(CharacterDatabase, query);
    if (_:result == 0 || db_num_rows(result) == 0) { if (_:result != 0) db_free_result(result); SendClientMessage(playerid, COLOR_SERVER_MESSAGE, "Login gagal. Coba lagi."); return 0; }
    db_get_field_assoc(result, "password_hash", value, sizeof(value));
    db_get_field_assoc(result, "username", username, sizeof(username));
    if (!Account_VerifyPassword(password, username, value)) { db_free_result(result); SendClientMessage(playerid, COLOR_SERVER_MESSAGE, "Password salah. Silakan coba lagi."); ShowPlayerDialog(playerid, DIALOG_ACCOUNT_LOGIN, DIALOG_STYLE_PASSWORD, "Login", "Password salah. Masukkan kembali password account.", "Login", "Keluar"); return 0; }
    db_get_field_assoc(result, "id", value, sizeof(value)); accountId = strval(value);
    db_get_field_assoc(result, "admin_level", value, sizeof(value)); adminLevel = strval(value);
    db_free_result(result);
    return Account_Authenticate(playerid, accountId, adminLevel, username);
}

stock Account_Register(playerid, const password[])
{
    new query[384], hash[80], value[32], accountId, DBResult:result;
    if (!Player_IsOnline(playerid) || !Account_IsUsernameValid(AccountPendingUsername[playerid]) || !Account_IsPasswordValid(password)) return 0;
    Account_BuildPasswordHash(password, AccountPendingUsername[playerid], hash, sizeof(hash));
    format(query, sizeof(query), "INSERT INTO accounts (username, password_hash, admin_level, created_at, last_login) VALUES ('%s', '%s', 0, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP)", AccountPendingUsername[playerid], hash);
    result = db_query(CharacterDatabase, query);
    if (_:result == 0) { SendClientMessage(playerid, COLOR_SERVER_MESSAGE, "Username sudah digunakan atau database gagal. Pilih username lain."); Account_ShowUsernameDialog(playerid); return 0; }
    db_free_result(result);
    result = db_query(CharacterDatabase, "SELECT last_insert_rowid() AS account_id");
    if (_:result == 0 || db_num_rows(result) == 0) { if (_:result != 0) db_free_result(result); printf("[Account] Could not obtain new account ID."); return 0; }
    db_get_field_assoc(result, "account_id", value, sizeof(value));
    accountId = strval(value); db_free_result(result);
    // Adopt a legacy character only when its old identifier exactly matches the
    // newly registered username and it has not already been linked to an account.
    format(query, sizeof(query), "UPDATE characters SET account_id = %d WHERE account_identifier = '%s' AND account_id IS NULL", accountId, AccountPendingUsername[playerid]);
    result = db_query(CharacterDatabase, query);
    if (_:result != 0) db_free_result(result);
    return Account_Authenticate(playerid, accountId, 0, AccountPendingUsername[playerid]);
}

stock bool:Account_InitializeDatabase()
{
    new DBResult:result;
    if (!Character_IsDatabaseReady()) return false;
    result = db_query(CharacterDatabase, "CREATE TABLE IF NOT EXISTS accounts (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT NOT NULL UNIQUE, password_hash TEXT NOT NULL, admin_level INTEGER NOT NULL DEFAULT 0, created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP, last_login TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP)");
    if (_:result == 0) { printf("[Account] Database initialization failure: could not create accounts table."); return false; }
    db_free_result(result);
    // Safe, non-destructive migration for the legacy character table. Existing rows
    // remain intact with NULL account_id and are never loaded until explicitly linked.
    result = db_query(CharacterDatabase, "ALTER TABLE characters ADD COLUMN account_id INTEGER");
    if (_:result != 0) db_free_result(result);
    result = db_query(CharacterDatabase, "CREATE UNIQUE INDEX IF NOT EXISTS idx_characters_account_id ON characters(account_id) WHERE account_id IS NOT NULL");
    if (_:result == 0) { printf("[Account] Character migration/index initialization failure."); return false; }
    db_free_result(result);
    return true;
}
