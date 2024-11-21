#include <string.h>
#include <openssl/sha.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <sqlite3.h>

#include "user.h"

void readtext_user(char *text){
    fgets(text, 1000, stdin);
}



User *sign_in(){
	printf("Enter username!\n");
	char * username = malloc(200 * sizeof(char));
	readtext_user(username);
	char * master_password = malloc(200 * sizeof(char));
	readtext_user(master_password);

    int id = selectUser(username, master_password);

    if(id != 0){
        User *this = malloc(sizeof(User));
        this->name = username;
        this->user_id = id;

        return this;
    }else{
        printf("Wrong credit!\n");
        return 0;
    }
}

int selectUser(char* name, char* password){
    sqlite3 *db;
    sqlite3_stmt *stmt;


    const unsigned char* hash = malloc(20* sizeof(unsigned char));
    int id;
    // Открываем базу данных SQLite
    int rc = sqlite3_open("password_manager.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    // SQL-запрос для получения данных
    char query[256];
    snprintf(query, sizeof(query), "SELECT password_hash, id FROM users WHERE username='%s';", name);

    // Подготовка SQL-запроса
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    // Выполняем запрос
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        // Преобразуем hex-данные обратно в бинарный формат
        hash = sqlite3_column_text(stmt, 0);
        id = atoi((const char *)sqlite3_column_text(stmt, 1));

    } else {
        fprintf(stderr, "No data found for name: %s\n", name);
        return 0;
    }

    // printf("Hash\n");
    // for (int i = 0; i < 20; i++) {
    //     printf("%02x ", hash[i]);
    // }
    // printf("\n");

    // printf("Pass\n");
    // for (int i = 0; i < 20; i++) {
    //     printf("%02x ", password_hash(password)[i]);
    // }
    // printf("\n");



    for (int i = 0; i < strlen(hash); i++)
    {
        if (hash[i] != password_hash(password)[i]){
            
            printf("They are different");
            return 0;
        }
             // They are different
    }
 // They must be the same


    // Освобождаем ресурсы
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return id;
}


int verifyPassword(int id, char* password){
    sqlite3 *db;
    sqlite3_stmt *stmt;


    const unsigned char* hash = malloc(20* sizeof(unsigned char));
    // Открываем базу данных SQLite
    int rc = sqlite3_open("password_manager.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    // SQL-запрос для получения данных
    char query[256];
    snprintf(query, sizeof(query), "SELECT password_hash FROM users WHERE id='%d';", id);

    // Подготовка SQL-запроса
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    // Выполняем запрос
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        // Преобразуем hex-данные обратно в бинарный формат
        hash = sqlite3_column_text(stmt, 0);

    } else {
        fprintf(stderr, "No data found for name: %d\n", id);
        return 0;
    }

    for (int i = 0; i < strlen(hash); i++)
    {
        if (hash[i] != password_hash(password)[i]){
            
            printf("Wrong password");
            return 0;
        }
             // They are different
    }
     // Освобождаем ресурсы
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;

}


void PrintUser(User * this){
    printf("%s\n", this->name);
    printf("%s\n", this->master_pass);
    for (int i = 0; i < 20; i++) {
        printf("%02x ", this->hash[i]);
    }
    printf("\n");

}



User * NewUser(User * this){
    
    printf("Enter login!\n");
    this->name  = malloc(200 * sizeof(char));
    readtext_user(this->name);
    printf("Enter password!\n");
	this->master_pass= malloc(200 * sizeof(char));
	readtext_user(this->master_pass);
    printf("Renter password!\n");
    char * reenter_master_password = malloc(200 * sizeof(char));
    readtext_user(reenter_master_password);
    if(check_password == 0){
        printf("Your password is weak\n");
        return 0;
    }
    
    
    if (strlen(this->master_pass) != strlen(reenter_master_password)){
        printf("Lenght passwords dont match");
        return 0;
    }

    
         // They must be different
    for (int i = 0; i < strlen(this->master_pass); i++)
    {
        if (this->master_pass[i] != reenter_master_password[i]){
            
            printf("They are different");
            return 0;
        }
             // They are different
    }
 // They must be the same

    this->hash = password_hash(this->master_pass);
    printf("%s", this->hash);
    return this;
}


int check_password(char * password){
    int hasLower = 0, hasUpper = 0, hasDigit = 0; 
  
    for (int i = 0; i < strlen(password); i++) { 
        if (islower(password[i])) 
            hasLower += 1; 
        if (isupper(password[i])) 
            hasUpper += 1; 
        if (isdigit(password[i])) 
            hasDigit += 1; 
    } 
  
    // Displaying the strength of password 
    
    if ( hasUpper>1 && hasDigit>1 && hasLower>1 && (strlen(password) >= 8)){
        printf("Strong password\n");
        return 1;
    } // considering a strong must be of length 6 or more
        
    else{
        printf("Wealk password\n");
        return 0;
    }
} 

unsigned char * password_hash(char * pass){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)pass, strlen(pass), hash);

    // Выделяем память для хранения хеша в шестнадцатеричном формате
    char *hash_string = malloc(SHA256_DIGEST_LENGTH * 2 + 1); // +1 для нуль-терминатора
    if (hash_string == NULL) {
        return NULL; // Обработка ошибки при выделении памяти
    }

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&hash_string[i * 2], "%02x", hash[i]);
    }

    return hash_string;
}
