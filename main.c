// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <openssl/evp.h>
// #include <openssl/rand.h>
// #include <libpq-fe.h>

// #include "password.h"


// void save_to_db(unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int ciphertext_len, char * name) {
//     PGconn *conn = PQconnectdb("user=postgres dbname=password_manager password=123456789");

//     // Проверка подключения
//     if (PQstatus(conn) != CONNECTION_OK) {
//         fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
//         PQfinish(conn);
//         exit(1);
//     }

//     // Преобразуем данные в hex-формат для вставки в базу
//     char key_hex[33];
//     char iv_hex[33];
//     char ciphertext_hex[ciphertext_len * 2 + 1];

//     for (int i = 0; i < 16; i++) {
//         sprintf(key_hex + (i * 2), "%02x", key[i]);
//         sprintf(iv_hex + (i * 2), "%02x", iv[i]);
//     }
//     for (int i = 0; i < ciphertext_len; i++) {
//         sprintf(ciphertext_hex + (i * 2), "%02x", ciphertext[i]);
//     }

//     // Создаем SQL-запрос для вставки
//     char query[1024];
//     snprintf(query, sizeof(query), 
//              "INSERT INTO encrypted_data (key, iv, ciphertext, name) VALUES ('%s', '%s', '%s', '%s');", 
//              key_hex, iv_hex, ciphertext_hex, name);

//     // Выполняем запрос
//     PGresult *res = PQexec(conn, query);

//     // Проверка на успешное выполнение
//     if (PQresultStatus(res) != PGRES_COMMAND_OK) {
//         fprintf(stderr, "Insert failed: %s", PQerrorMessage(conn));
//         PQclear(res);
//         PQfinish(conn);
//         exit(1);
//     }

//     // Освобождаем ресурсы
//     PQclear(res);
//     PQfinish(conn);
// }

// void load_from_db(const char *name, unsigned char *key, unsigned char *iv, unsigned char *ciphertext, int *ciphertext_len) {
//     PGconn *conn = PQconnectdb("user=postgres dbname=password_manager password=123456789");

//     // Проверка подключения
//     if (PQstatus(conn) != CONNECTION_OK) {
//         fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
//         PQfinish(conn);
//         exit(1);
//     }

//     // Создаем SQL-запрос для извлечения данных по имени
//     char query[256];
//     snprintf(query, sizeof(query), "SELECT key, iv, ciphertext FROM encrypted_data WHERE name='%s';", name);

//     // Выполняем запрос
//     PGresult *res = PQexec(conn, query);

//     // Проверяем, вернулись ли данные
//     if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
//         fprintf(stderr, "No data found for name: %s\n", name);
//         PQclear(res);
//         PQfinish(conn);
//         exit(1);
//     }

//     // Преобразуем hex-данные обратно в бинарный формат
//     char *key_hex = PQgetvalue(res, 0, 0);
//     char *iv_hex = PQgetvalue(res, 0, 1);
//     char *ciphertext_hex = PQgetvalue(res, 0, 2);

//     for (int i = 0; i < 16; i++) {
//         sscanf(&key_hex[i * 2], "%2hhx", &key[i]);
//         sscanf(&iv_hex[i * 2], "%2hhx", &iv[i]);
//     }

//     *ciphertext_len = strlen(ciphertext_hex) / 2;
//     for (int i = 0; i < *ciphertext_len; i++) {
//         sscanf(&ciphertext_hex[i * 2], "%2hhx", &ciphertext[i]);
//     }

//     // Освобождаем ресурсы
//     PQclear(res);
//     PQfinish(conn);
// }



// void handleErrors() {
//     fprintf(stderr, "An error occurred\n");
//     exit(1);
// }

// // Функция шифрования
// int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
//             unsigned char *iv, unsigned char *ciphertext) {
//     EVP_CIPHER_CTX *ctx;
//     int len;
//     int ciphertext_len;

//     // Создаем и инициализируем контекст
//     if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

//     // Инициализируем операцию шифрования AES-128-CBC
//     if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) handleErrors();

//     // Шифруем данные
//     if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) handleErrors();
//     ciphertext_len = len;

//     // Завершаем шифрование, добавляем паддинг
//     if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
//     ciphertext_len += len;

//     // Очищаем контекст
//     EVP_CIPHER_CTX_free(ctx);

//     return ciphertext_len;
// }


// // Функция расшифрования
// int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
//             unsigned char *iv, unsigned char *plaintext) {
//     EVP_CIPHER_CTX *ctx;
//     int len;
//     int plaintext_len;

//     // Создаем и инициализируем контекст
//     if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

//     // Инициализируем операцию расшифрования AES-128-CBC
//     if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) handleErrors();

//     // Расшифровываем данные
//     if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) handleErrors();
//     plaintext_len = len;

//     // Завершаем расшифрование, удаляем паддинг
//     if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
//     plaintext_len += len;

//     // Очищаем контекст
//     EVP_CIPHER_CTX_free(ctx);

//     return plaintext_len;
// }

// void readtext(char *text){
//     printf("Enter name of password!\n");
//     fgets(text, 1000, stdin);
// }



// int main(){
//     Password * new = NewPassword("Google");
//     Print(new);
    
// 	// Ключ шифрования (16 байт для AES-128)
//     // unsigned char key[16];
    
//     // // Инициализационный вектор (IV), размером 16 байт
//     // unsigned char iv[16];

//     // // Буферы для зашифрованного текста и расшифрованного текста
//     // unsigned char ciphertext[128];
//     // unsigned char decryptedtext[128];

//     // // Генерация случайного ключа и IV
//     // if (!RAND_bytes(key, sizeof(key))) handleErrors();
//     // if (!RAND_bytes(iv, sizeof(iv))) handleErrors();

//     // printf("Key: ");
//     // for (int i = 0; i < 16; i++) printf("%02x", key[i]);
//     // printf("\n");

//     // printf("IV: ");
//     // for (int i = 0; i < 16; i++) printf("%02x", iv[i]);
//     // printf("\n");

//     // Шифруем текст
//     // int ciphertext_len = encrypt(new->pass, strlen((char *)new->pass), key, iv, ciphertext);


// 	// save_to_db(key, iv, ciphertext, ciphertext_len, new->name);


//     // Выводим зашифрованный текст в hex-формате
//     // printf("Ciphertext: ");
//     // for (int i = 0; i < ciphertext_len; i++) printf("%02x", ciphertext[i]);
//     // printf("\n");

//     // // Расшифровываем текст
//     // int decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);

//     // // Добавляем null-терминатор к расшифрованному тексту
//     // decryptedtext[decryptedtext_len] = '\0';

//     // // Выводим расшифрованный текст
//     // printf("Decrypted text: %s\n", decryptedtext);

// 	unsigned char loaded_key[16];
//     unsigned char loaded_iv[16];
//     unsigned char loaded_ciphertext[128];
//     int loaded_ciphertext_len;
// 	unsigned char decryptedtext[128];
    
//     load_from_db("Google", loaded_key, loaded_iv, loaded_ciphertext, &loaded_ciphertext_len);
// 	int decryptedtext_len = decrypt(loaded_ciphertext, loaded_ciphertext_len, loaded_key, loaded_iv, decryptedtext);

//     // Добавляем null-терминатор к расшифрованному тексту
//     decryptedtext[decryptedtext_len] = '\0';

//     // Выводим расшифрованный текст
//     printf("Decrypted text: %s\n", decryptedtext);
//     return 0;

// }
// POSTGRES ----------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <sqlite3.h>
#include <stdbool.h>

#include "password.h"

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

// Функция обработки ошибок OpenSSL
void handleErrors() {
    fprintf(stderr, "An error occurred\n");
    exit(1);
}

// Функция шифрования
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    // Создаем и инициализируем контекст
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    // Инициализируем операцию шифрования AES-128-CBC
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) handleErrors();

    // Шифруем данные
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) handleErrors();
    ciphertext_len = len;

    // Завершаем шифрование, добавляем паддинг
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    // Очищаем контекст
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

// Функция расшифрования
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    // Создаем и инициализируем контекст
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    // Инициализируем операцию расшифрования AES-128-CBC
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv)) handleErrors();

    // Расшифровываем данные
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) handleErrors();
    plaintext_len = len;

    // Завершаем расшифрование, удаляем паддинг
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;

    // Очищаем контекст
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

void readtext(char *text){
    printf("Enter name of password!\n");
    fgets(text, 1000, stdin);
}

int main(){
    // // Создаем новый объект пароля (пример с использованием структуры Password)
    // Password *new = NewPassword("Google");
    // Print(new);

    // // Ключ шифрования (16 байт для AES-128)
    // unsigned char key[16];
    
    // // Инициализационный вектор (IV), размером 16 байт
    // unsigned char iv[16];

    // // Буферы для зашифрованного текста и расшифрованного текста
    // unsigned char ciphertext[128];
    // unsigned char decryptedtext[128];

    // // Генерация случайного ключа и IV
    // if (!RAND_bytes(key, sizeof(key))) handleErrors();
    // if (!RAND_bytes(iv, sizeof(iv))) handleErrors();

    // printf("Key: ");
    // for (int i = 0; i < 16; i++) printf("%02x", key[i]);
    // printf("\n");

    // printf("IV: ");
    // for (int i = 0; i < 16; i++) printf("%02x", iv[i]);
    // printf("\n");

    // // Шифруем текст
    // int ciphertext_len = encrypt(new->pass, strlen((char *)new->pass), key, iv, ciphertext);

    // // Сохраняем в базу данных
    // save_to_db(key, iv, ciphertext, ciphertext_len, new->name);

    // // Буферы для загруженных данных
    // unsigned char loaded_key[16];
    // unsigned char loaded_iv[16];
    // unsigned char loaded_ciphertext[128];
    // int loaded_ciphertext_len;

    // // Загружаем данные из базы данных
    // load_from_db("Google", loaded_key, loaded_iv, loaded_ciphertext, &loaded_ciphertext_len);

    // // Расшифровываем текст
    // int decryptedtext_len = decrypt(loaded_ciphertext, loaded_ciphertext_len, loaded_key, loaded_iv, decryptedtext);

    // // Добавляем null-терминатор к расшифрованному тексту
    // decryptedtext[decryptedtext_len] = '\0';

    // // Выводим расшифрованный текст
   	// printf("Decrypted text: %s\n", decryptedtext);
	bool fl = true;
	while(fl){
		printf(
			"Выберите режим работы\n"
			"1. Создать пароль\n"
			"2. Показать пароль\n"
		);
		int choose;
		scanf("%d", &choose);
		getchar();
		char * name = malloc(255 * sizeof(char));
		switch (choose)
		{
		case 1:
			readtext(name);
			Password *new = NewPassword(name);
    		Print(new);
			// Ключ шифрования (16 байт для AES-128)
			unsigned char key[16];
			
			// Инициализационный вектор (IV), размером 16 байт
			unsigned char iv[16];

			// Буферы для зашифрованного текста и расшифрованного текста
			unsigned char ciphertext[128];
			unsigned char decryptedtext[128];

			// Генерация случайного ключа и IV
			if (!RAND_bytes(key, sizeof(key))) handleErrors();
			if (!RAND_bytes(iv, sizeof(iv))) handleErrors();

			printf("Key: ");
			for (int i = 0; i < 16; i++) printf("%02x", key[i]);
			printf("\n");

			printf("IV: ");
			for (int i = 0; i < 16; i++) printf("%02x", iv[i]);
			printf("\n");

			// Шифруем текст
			int ciphertext_len = encrypt(new->pass, strlen((char *)new->pass), key, iv, ciphertext);

			// Сохраняем в базу данных
			save_to_db(key, iv, ciphertext, ciphertext_len, new->name);
			break;
		case 2:
			readtext(name);
			// Буферы для загруженных данных
    		unsigned char loaded_key[16];
			unsigned char loaded_iv[16];
			unsigned char loaded_ciphertext[128];
			int loaded_ciphertext_len;

			// Загружаем данные из базы данных
			load_from_db(name, loaded_key, loaded_iv, loaded_ciphertext, &loaded_ciphertext_len);

			// Расшифровываем текст
			int decryptedtext_len = decrypt(loaded_ciphertext, loaded_ciphertext_len, loaded_key, loaded_iv, decryptedtext);

			// Добавляем null-терминатор к расшифрованному тексту
			decryptedtext[decryptedtext_len] = '\0';

			// Выводим расшифрованный текст
			printf("Decrypted text: %s\n", decryptedtext);
			break;
		default:
			break;
		}

	}
    return 0;

}