#ifndef PASSWORD_H
#define PASSWORD_H

typedef struct
{
    char *          name;
    char *          pass;
    char *          (*GetName)(void*);
} Password;


char * GeneratePassword();
Password * NewPassword(char * name);
void Print(Password * this);
char * AesEncryptPassword();

#endif