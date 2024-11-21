#include <sqlite3.h>
// Функция для сохранения данных в SQLite
void save_to_db(unsigned char *iv, unsigned char *ciphertext, int ciphertext_len, char *name, int user_id) {
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
    char iv_hex[33];
    char ciphertext_hex[ciphertext_len * 2 + 1];

    for (int i = 0; i < 16; i++) {
        sprintf(iv_hex + (i * 2), "%02x", iv[i]);
    }
    for (int i = 0; i < ciphertext_len; i++) {
        sprintf(ciphertext_hex + (i * 2), "%02x", ciphertext[i]);
    }

    // SQL-запрос для вставки данных
    char query[1024];
    snprintf(query, sizeof(query), 
			"CREATE TABLE IF NOT EXISTS encrypted_data (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, iv TEXT, ciphertext TEXT, user_id INTEGER, FOREIGN KEY(user_id) REFERENCES users(id));"
             "INSERT INTO encrypted_data (iv, ciphertext, name, user_id) VALUES ('%s', '%s', '%s', '%d');", 
             iv_hex, ciphertext_hex, name, user_id);

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
void load_from_db(const char *name, unsigned char *iv, unsigned char *ciphertext, int *ciphertext_len, int id) {
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
    snprintf(query, sizeof(query), "SELECT iv, ciphertext FROM encrypted_data WHERE name='%s' and id='%d';", name, id);

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
        const unsigned char *iv_hex = sqlite3_column_text(stmt, 0);
        const unsigned char *ciphertext_hex = sqlite3_column_text(stmt, 1);

        for (int i = 0; i < 16; i++) {
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


void save_user_to_db(char* name, char * password_hash){
    sqlite3 *db;
    char *err_msg = 0;
    
    // Открываем базу данных SQLite
    int rc = sqlite3_open("password_manager.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }


    char query[1024];
    snprintf(query, sizeof(query), 
			"CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password_hash TEXT);"
             "INSERT INTO users (username, password_hash) VALUES ('%s', '%s');", 
             name, password_hash);

    // Выполнение SQL-запроса
    rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to insert data: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    // Закрываем базу данных
    sqlite3_close(db);
}