//hold

#include <stdio.h> //printf
#include <stdlib.h> //macros
#include <gmp.h>
                    //

int main(int argc, char* argv[])
{
    int n = 0;
    int result = 0;
    mpz_t t1;
    mpz_t t2;
    mpz_t next_fib;

    mpz_init_set_ui(t1,0);
    mpz_init_set_ui(t2,1);
    mpz_init_set_ui(next_fib, 1);

    if (argc < 2)
    {
        printf("Argument must be given on command line");
        exit(EXIT_FAILURE);
    }

    result = sscanf(argv[1], "%d", &n);

    if ((result == 0) || (n < 1)){
        printf("Enter number greater than 0");
        exit(EXIT_FAILURE);
    }

    gmp_printf("%6d: %Zd\n",0,t1);
    gmp_printf("%6d: %Zd\n",1,t2);


    for (int i = 2; i <= n; ++i){
        gmp_printf("%6ld: %Zd\n", i, next_fib);
        mpz_set(t1,t2);
        mpz_set(t2,next_fib);
        mpz_add(next_fib, t1,t2);
    }
    return EXIT_SUCCESS;
}
