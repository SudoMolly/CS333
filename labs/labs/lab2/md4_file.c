// rchaney@pdx.edu

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include <openssl/md4.h>

#define BUFFER_SIZE 10

int
main(int argc, char *argv[])
{
    int ifd = 0;
    unsigned char buffer[BUFFER_SIZE] = {'\0'};
    ssize_t bytes_read = 0;
    MD4_CTX context;
    uint8_t digest[MD4_DIGEST_LENGTH];


	printf("MD4_DIGEST_LENGTH: %d\n", MD4_DIGEST_LENGTH);
    for(int i = 1; i < argc; ++i) {
        ifd = open(argv[i], O_RDONLY);
        if (ifd < 0) {
            perror("cannot open file");
			fprintf(stderr, "Unable to open input file %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
        
        MD4Init(&context);
        for( ; (bytes_read = read(ifd, buffer, BUFFER_SIZE)) > 0; ) {
            MD4Update(&context, buffer, bytes_read);
        }
        MD4Final(digest, &context);
        close(ifd);

        for(int j = 0; j < MD4_DIGEST_LENGTH; ++j) {
            printf("%02x", digest[j]);
        }
        printf("  %s", argv[i]);
        printf("\n");
    }
    
    return EXIT_SUCCESS;
}
