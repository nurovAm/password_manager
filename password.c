#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>


#include "password.h"



void Print(Password * this){
    printf("Name of service: %s\n", this->name);
    printf("Password: %s\n", this->pass);
}

static char* GetName(void *this){
    Password *_this = (Password*)this;
    return _this->name;
}

char* GeneratePassword(){
    srand(time(NULL));
    char * new_pass = (char *)malloc(16 * sizeof(char));
    char chars[] = "+-/*!&$#?=@<>abcdefghijklnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    int range = strlen(chars);
    for(int i = 0; i < 16; i++){
        new_pass[i] = chars[rand() % range];   
    }
    return new_pass;
}

char * AesEncryptPassword(char * pass){
    
}


Password * NewPassword(char * name){
    Password * _this = (Password *)malloc(sizeof(Password));
    if (_this != NULL)
    {
        _this->GetName = GetName;
        _this->pass = GeneratePassword();
        _this->name = (char*)malloc(strlen(name) + 1);
        strcpy(_this->name, name);
    }

    return (Password *) _this;
}