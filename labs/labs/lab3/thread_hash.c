#include "thread_hash.h"

options_t option_init = {.inputExists = false, .output = false, .directory = false, .threads = 1, .verbose = false, .help = true, .nice = false};

void assign_hashes(void)
{
    char* buf;
    const char* hold;
    int count;
    while ((hold = strstr(FILE_BUFFER, "\n")))
    {
        ++hold;
        ++count;
    }


}

void be_nice(void)
{
}

void help_exit(void)
{
    printf("help text\n\
\t./thread_hash ...\n\
\tOptions: i:o:d:hvt:n\n\
\t\t-i file		hash file name (required)\n\
\t\t-o file		output file name (default stdout)\n\
\t\t-d file		dictionary file name (required)\n\
\t\t-t #		number of threads to create (default == 1)\n\
\t\t-n		renice to 10\n\
\t\t-v		enable verbose mode\n\
\t\t-h		helpful text\n\
");
    error_occurred = false;
    clean_exit();
}

void clean_exit(void)
{
    int exit_value = EXIT_SUCCESS;
    GATEPASS("INTO CLEAN EXIT");
    if (options != NULL)
    {
        if (options->inputExists)
            free(options->args[0]);
        if (options->output)
            free(options->args[1]);
        if (options->directory)
            free(options->args[2]);
        if (options->threads)
            free(options->args[3]);
        for (int i = 0; i < 4; ++i) options->args[i] = NULL;
        free(options);
        options = NULL;
        GATEPASS("OPTIONS FREED SUCCESSFULLY");
    }
    if (error_occurred) exit_value = EXIT_FAILURE;
    exit(exit_value);
}


options_t* getoptions(int argc, char** argv)
{
    const char missing_input[] = "input file(i)";
    const char missing_direc[] = "directory of crackers (d)";
    char c = 0;
    GATEPASS("INTO GETOPTIONS");
    options = (options_t*) malloc(sizeof(options_t));
    *options = option_init;
    perror("BAD OPTARG");
    while ((c = getopt(argc,argv,":i:o:d:t:vhn")) != -1)
    {
        switch(c)
        {
            case 'i': //INPUT
                SAY("INPUT FOUND");
                options->inputExists = true;
                options->args[0] = strdup(optarg);
                break;
            case 'o': //OUTPUT
                SAY("OUTPUT FOUND");
                options->output = true;
                options->args[1] = strdup(optarg);
                break;
            case 'd': //DIRECTORIES
                SAY("DIRECTORY FOUND");
                options->directory = true;
                options->args[2] = strdup(optarg);
                break;
            case 't': //THREADS
                SAY("THREADS FOUND");
                options->threads = atoi(optarg);
                options->args[3] = optarg;
                break;
            case 'v': //VERBOSE
                SAY("VERBOSE ON");
                options->verbose = true;
                break;
            case 'h':
                SAY("HELP EXIT");
                options->help = true;
                help_exit();
                break;
            case 'n':
                SAY("NICE TOGGLED");
                options->nice = true;
                break;
            case '?':
                fprintf(stderr, "*** Unrecognized command line option ***\n*** Showing help and exiting...\n");
                help_exit();
                break;
            case ':':
                fprintf(stderr, "*** Missing command line option for: %c, exiting...\n", c);
                clean_exit();
                break;
            default:
                fprintf(stderr, "UNKNOWN ERROR OCCURRED, exiting ");
                SHOWI("OPT WAS: ", c);
                GATEPASS("UNKNOWN ERROR IN GETOPTIONS");
                clean_exit();
                break;
        }
    }
    if (c == 0) 
    {
        SAY("NO ARGUMENTS FOUND? EXITING OPTARGUMENT");
        fprintf(stderr, "*** No arguments provided, showing help and exiting...\n");
        help_exit();
    }
    if ((!options->inputExists || !options->directory))
    {
        SAY("MISSING REQUIRED OPTION");
        fprintf(stderr, "Missing command line options");
        if (!options->inputExists && !options->directory)
            fprintf(stderr, ", both %s AND %s\n", missing_input, missing_direc);
        else if (!options->inputExists)
            fprintf(stderr, ", %s, Need passwords in order to hash!\n", missing_input);
        else
            fprintf(stderr, ", %s, Need dictionary to brute force passwords, else locked!\n", missing_direc);
        fprintf(stderr, "*** Showing help and exiting...\n");
        help_exit();
    }
    return options;
}

void open_file(char* key_file, char* dir_file)
{
    GATEPASS("OPEN FILE ENTERED");
    if (key_file  == NULL || dir_file == NULL) 
    {
        perror("PROBLEM, FILENAME IS EMPTY");
        clean_exit();
    }
    KIN = open(key_file, O_RDONLY);
    GATEPASS("OPENED KEY FILE");
    if (KIN == -1)
    {
        perror("Key file could not be opened");
        clean_exit();
    }
    KIN_OPEN = true;
    DIN = open(dir_file, O_RDONLY);
    GATEPASS("OPENED DICT FILE");
    if (DIN == -1)
    {
        perror("Dictionary file could not open");
        clean_exit();
    }
    DIN_OPEN = true;
}

void init_buf(void)
{
    memset(FILE_BUFFER, 0, BUF_SIZE * sizeof(char));
}

int read_buff(int fd)
{
    int amt_read = 0;
    GATEPASS("INTO READ LINE");
    init_buf();
    amt_read = read(fd, FILE_BUFFER, BUF_SIZE);
    if (amt_read == -1)
    {
        perror("FAILED TO READ FILE DESCRIPTOR");
        clean_exit();
    }
    return amt_read;
}

int determine_type(char first, char second, char third)
{
    if (first != '$') return DES;
    switch(second)
    {
        case '3': //NT
            GATEPASS("NT");
            return NT;
            break;
         case '1': //MD5
            GATEPASS("MD5");
            return MD5;
            break;
         case '5': //SHA256
            GATEPASS("SHA256");
            return SHA256;
            break;
         case '6':
            GATEPASS("SHA512");
            return SHA512;
            break;
         case 'y':
            GATEPASS("YESCRYPT");
            return YESCRYPT;
            break;
         case 'g':
            GATEPASS("GYESCRYPT");
            return GOST_YESCRYPT;
            break;
         case '2':
            GATEPASS("BCRYPT");
            return BCRYPT;
            break;
    }
    GATEPASS("UNKNOWN TYPE OF HASH");
    perror("UNIDENTIFABLE PASSWORD HASH");
    clean_exit();
    return -1;
}

int main(int argc, char** argv)
{
    options = getoptions(argc,argv);
    error_occurred = false;
    clean_exit();
}
    
