#ifndef PASSWORD_H
#define PASSWORD_H


typedef struct
{
    char *                  name;
    char *                  pass;
    unsigned char *         key;
    unsigned char *         iv;
    unsigned char *         ciphertext;
} Password;









char * GeneratePassword();
Password * NewPassword(char * name);
void Print(Password * this);
int AesEncryptPassword(Password * this, unsigned char *ciphertext);
int AesDecryptPassword(unsigned char* ciphertext, int ciphertext_len, unsigned char * key, unsigned char * iv, unsigned char * plaintext);
unsigned char *  GenerateKey(Password* this);
unsigned char *  GenerateIV(Password* this);

#endif