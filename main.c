#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdbool.h>

#include "user.h"
#include "password.h"
#include "db.c"




void readtext(char *text){
    fgets(text, 1000, stdin);
}

int login_user(){
	int choose_ac;
	printf("Do you have an account?\n"
		"1. Yes\n"
		"2. No\n");
	scanf("%d", &choose_ac);
	getchar();
	return choose_ac;

}


int run(){
	bool fl = true;
	int choose = login_user();
	User *user = malloc(sizeof(User));
	if (choose == 2)
	{
		user = malloc(sizeof(User));
		NewUser(user);
		save_user_to_db(user->name, user->hash);
		
	}
	else{
		printf("Error\n");
	}
	user = sign_in();
	
	printf("%s\n", user->name);
	printf("%d\n", user->user_id);
    printf("\n");
	
	while(fl){
		printf(
			"Выберите режим работы\n"
			"1. Создать пароль\n"
			"2. Показать пароль\n"
			"3. Выход\n"
		);
		int choose;
		scanf("%d", &choose);
		getchar();
		char * name = malloc(255 * sizeof(char));
		printf("Enter your password\n");
		char * master_password = malloc(20 * sizeof(char));
		readtext(master_password);
		if(verifyPassword(user->user_id, master_password)==1){
			switch (choose)
			{
				case 1:	
					printf("Enter name of password!\n");
					readtext(name);
					Password *new = NewPassword(name, master_password);
					Print(new);
					int ciphertext_len = AesEncryptPassword(new, new->ciphertext);
					// printf("Encypted pass: %s\n", new->ciphertext);
					save_to_db(new->iv, new->ciphertext, strlen(new->ciphertext), new->name, user->user_id);
					break;
				case 2:
					printf("Enter name of password!\n");
					readtext(name);
					Password *decrypt = NewPassword(name, master_password);
					int loaded_ciphertext_len;
					// Загружаем данные из базы данных
					
					load_from_db(name, decrypt->iv, decrypt->ciphertext, &loaded_ciphertext_len, user->user_id);
					int decryptedtext_len = AesDecryptPassword(decrypt->ciphertext, loaded_ciphertext_len, decrypt->key, decrypt->iv, decrypt->pass);
					// Добавляем null-терминатор к расшифрованному тексту
					decrypt->pass[decryptedtext_len] = '\0';
					printf("Your password: %s\n",  decrypt->pass);
					break;
				case 3:
					return 0;
				default:
					break;
			}
		}else{
			printf("Error\n");
		}
	}
}



int main(){
	run();
    return 0;

}