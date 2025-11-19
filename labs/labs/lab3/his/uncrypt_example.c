// R Jesse Chaney
// rchaney@pdx.edu

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>

#define BUF_SIZE 1000

// run this as:
//    uncrypt_example < keys-10.txt
//        all the passwords will be cracked
//
//    uncrypt_example b < keys-10.txt
//        all the passwords will fail

int
main(int argc, char *argv[])
{
    char buf[BUF_SIZE] = {'\0'};
    char *plain_text = NULL;
    char *setting = NULL;
    char *crypt_return = NULL;
    struct crypt_data crypt_stuff;

    while(fgets(buf, BUF_SIZE, stdin) != NULL) {
		// This time, I make use of the pesky newline.
		// split the input line into the password and the hash.
		// the colon acts as the field seperator
		plain_text = strtok(buf, ":");
		
		if (argc > 1) {
			// forces a failure
			// corrupts the plain text password
			plain_text[0]++;
		}
		setting = strtok(NULL, "\n");

		// make sure everything is nice and clean
		memset(&crypt_stuff, 0, sizeof(crypt_stuff));

		// copy in the "setting". this is the provided hash
		strncpy(crypt_stuff.setting, setting, CRYPT_OUTPUT_SIZE);

		// copy in the plain text password
		strncpy(crypt_stuff.input, plain_text, CRYPT_MAX_PASSPHRASE_SIZE);

		// call crypt
		// look at
		//     man crypt
		// or
		//     man 5 crypt
		// for more information
		///////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////
		///////////////  YOU DO NOT NEED TO PARSE APART THE HASH //////////////////////
		///////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////
		crypt_return = crypt_rn(plain_text, setting, &crypt_stuff, sizeof(crypt_stuff));
		
		if (strcmp(crypt_stuff.setting, crypt_return) == 0) {
			printf("cracked %s\t%s\n", plain_text, crypt_stuff.output);
			printf("\t%s\t%s\n", crypt_stuff.setting, crypt_stuff.output);
		}
		else {
			// the hashed plain text password does not hash to the same value
			// as the provided hash
			printf("failed %s\t%s\n", plain_text, crypt_stuff.output);
			printf("\t%s\t%s\n", crypt_stuff.setting, crypt_stuff.output);
		}
    }
        
    return EXIT_SUCCESS;
}
