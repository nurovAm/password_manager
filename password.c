#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include "password.h"

#define PASSWORD_SYMBOLS "+-/*!&$#?=@<>abcdefghijklnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"
#define PASSWORD_LEN 16
#define KEY_LEN 16
#define IV_LEN 16

// Отслеживание ошибок
void handleErrors() {
    fprintf(stderr, "An error occurred\n");
    exit(1);
}

// Вывод данных о пароле
void Print(Password * this){
    printf("Name of service: %s\n", this->name);
    printf("Password: %s\n", this->pass);
    

    // printf("Key: ");
    // for (int i = 0; i < 16; i++) printf("%02x", this->key[i]);
    // printf("\n");

    // printf("IV: ");
    // for (int i = 0; i < 16; i++) printf("%02x", this->iv[i]);
    // printf("\n");
    
}



// Генерация пароля
char* GeneratePassword(){
    // Добавление рандомайзера при помощи текущего времени
    srand(time(NULL));
    char * new_pass = (char *)malloc(PASSWORD_LEN * sizeof(char));
    char chars[] = PASSWORD_SYMBOLS;
    int range = strlen(chars);
    for(int i = 0; i < PASSWORD_LEN; i++){
        new_pass[i] = chars[rand() % range];   
    }
    return new_pass;
}

// Класс пароля
Password * NewPassword(char * name){
    Password * _this = (Password *)malloc(sizeof(Password));
    if (_this != NULL)
    {
        _this->pass = GeneratePassword();
        _this->name = (char*)malloc(strlen(name) + 1);
        strcpy(_this->name, name);
        _this->key = GenerateKey(_this);
        _this->iv = GenerateIV(_this);
        _this->ciphertext = malloc(128*sizeof(unsigned char));
    }

    return _this;
}

Password * LoadPassword(char * name){
    Password * _this = (Password *)malloc(sizeof(Password));
    if (_this != NULL)
    {
        _this->pass = (char *)malloc(PASSWORD_LEN * sizeof(char));
        _this->name = (char*)malloc(strlen(name) + 1);
        strcpy(_this->name, name);
        _this->key = malloc(KEY_LEN * sizeof(unsigned char));
        _this->iv = malloc(IV_LEN * sizeof(unsigned char));
        _this->ciphertext = malloc(128*sizeof(unsigned char));
    }

    return _this;
}

// Генерация рандомного ключа при помощи библиотеки openssl
unsigned char * GenerateKey(Password * this){
    unsigned char * key = malloc(KEY_LEN * sizeof(unsigned char));
   
    if(!RAND_bytes(key, sizeof(key))) handleErrors();

    return key;

}


// Генерация вектора инициализации при помощи библиотеки openssl
unsigned char * GenerateIV(Password * this){
    unsigned char *iv = malloc(IV_LEN * sizeof(unsigned char));
    if(!RAND_bytes(iv, sizeof(iv))) handleErrors();

    return iv;


}

int AesEncryptPassword(Password * this, unsigned char *ciphertext){
    // Контекст который используется для хранения информации о текущем состоянии оперции шифрования
     EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    // Создаем и инициализируем контекст
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    // Инициализируем операцию шифрования AES-128-CBC
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, this->key, this->iv)) 
        handleErrors();

    // Шифруем данные
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, this->pass, strlen(this->pass))) 
        handleErrors();
    ciphertext_len = len;

    // Завершаем шифрование, добавляем паддинг
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) 
        handleErrors();
    ciphertext_len += len;

    // Очищаем контекст
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;

}

int AesDecryptPassword(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
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