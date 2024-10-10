#include <sqlite3.h>
// Функция для сохранения данных в SQLite
void save_to_db(unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int ciphertext_len, char *name) {
    sqlite3 *db;
    char *err_msg = 0;
    
    // Открываем базу данных SQLite
    int rc = sqlite3_open("password_manager.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // Преобразуем данные в hex-формат для вставки в базу
    char key_hex[33];
    char iv_hex[33];
    char ciphertext_hex[ciphertext_len * 2 + 1];

    for (int i = 0; i < 16; i++) {
        sprintf(key_hex + (i * 2), "%02x", key[i]);
        sprintf(iv_hex + (i * 2), "%02x", iv[i]);
    }
    for (int i = 0; i < ciphertext_len; i++) {
        sprintf(ciphertext_hex + (i * 2), "%02x", ciphertext[i]);
    }

    // SQL-запрос для вставки данных
    char query[1024];
    snprintf(query, sizeof(query), 
			"CREATE TABLE IF NOT EXISTS encrypted_data (name TEXT, key TEXT, iv TEXT, ciphertext TEXT);"
             "INSERT INTO encrypted_data (key, iv, ciphertext, name) VALUES ('%s', '%s', '%s', '%s');", 
             key_hex, iv_hex, ciphertext_hex, name);

    // Выполнение SQL-запроса
    rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to insert data: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    // Закрываем базу данных
    sqlite3_close(db);
}

// Функция для загрузки данных из SQLite
void load_from_db(const char *name, unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int *ciphertext_len) {
    sqlite3 *db;
    sqlite3_stmt *stmt;

    // Открываем базу данных SQLite
    int rc = sqlite3_open("password_manager.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // SQL-запрос для получения данных
    char query[256];
    snprintf(query, sizeof(query), "SELECT key, iv, ciphertext FROM encrypted_data WHERE name='%s';", name);

    // Подготовка SQL-запроса
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // Выполняем запрос
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        // Преобразуем hex-данные обратно в бинарный формат
        const unsigned char *key_hex = sqlite3_column_text(stmt, 0);
        const unsigned char *iv_hex = sqlite3_column_text(stmt, 1);
        const unsigned char *ciphertext_hex = sqlite3_column_text(stmt, 2);

        for (int i = 0; i < 16; i++) {
            sscanf((const char *)&key_hex[i * 2], "%2hhx", &key[i]);
            sscanf((const char *)&iv_hex[i * 2], "%2hhx", &iv[i]);
        }

        *ciphertext_len = strlen((const char *)ciphertext_hex) / 2;
        for (int i = 0; i < *ciphertext_len; i++) {
            sscanf((const char *)&ciphertext_hex[i * 2], "%2hhx", &ciphertext[i]);
        }
    } else {
        fprintf(stderr, "No data found for name: %s\n", name);
    }

    // Освобождаем ресурсы
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

