#ifndef USER_H
#define USER_H

typedef struct
{
    int         user_id;
    char *      name;
    char *      master_pass;
    unsigned char *      hash;
} User;


User *sign_in();
User *NewUser();
int check_password(char * password);
unsigned char * password_hash(char * pass);
void PrintUser(User * this);
void readtext_user(char* text);
int selectUser(char * username, char * password);
int verifyPassword(int id, char * password);

#endif