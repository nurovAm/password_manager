#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdbool.h>

#include "password.h"
#include "db.c"




void readtext(char *text){
    printf("Enter name of password!\n");
    fgets(text, 1000, stdin);
}






int main(){
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
            int ciphertext_len = AesEncryptPassword(new, new->ciphertext);
            printf("Encypted pass: %s\n", new->ciphertext);
            save_to_db(new->key, new->iv, new->ciphertext, strlen(new->ciphertext), new->name);
			break;
		case 2:
			readtext(name);
            Password *decrypt = NewPassword(name);
            int loaded_ciphertext_len;
			// Загружаем данные из базы данных
			load_from_db(name, decrypt->key, decrypt->iv, decrypt->ciphertext, &loaded_ciphertext_len);
            int decryptedtext_len = AesDecryptPassword(decrypt->ciphertext, loaded_ciphertext_len, decrypt->key, decrypt->iv, decrypt->pass);
            // Добавляем null-терминатор к расшифрованному тексту
            decrypt->pass[decryptedtext_len] = '\0';
            printf("Your password: %s\n",  decrypt->pass);
            break;

		default:
			break;
    	}

	}
    return 0;

}