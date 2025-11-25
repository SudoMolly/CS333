#include "thread_hash.h"
#include <unistd.h>
options_t option_init = {.inputExists = false, .output = false, .directory = false, .threads = 1, .verbose = false, .help = true, .nice = false, .args[0] = NULL, .args[1] = NULL, .args[2] = NULL, .args[3] = NULL};


void SAY(const char* string) 
{
    if (options != NULL && options->verbose) dprintf(VOUT,"%s\n", string);
}
void SHOW(const char* str1, const char* str2) 
{
    if (options != NULL && options->verbose) dprintf(VOUT, "%s%s", str1,str2);
}
void SHOWI(const char* str1, int int1) 
{
    if (options != NULL && options->verbose) dprintf(VOUT,"%s%d\n", str1, int1);
}
void GATEPASS(const char* ident) 
{
    if (options != NULL && options->verbose) dprintf(VOUT, "PASSED GATE %s", ident);
}
void LOOPI(const char* iden, int i) 
{
    if (options != NULL && options->verbose) dprintf(VOUT,"In loop %s%d\n", iden, i);
}

char* strnstr(char* hay, char needle, int max_count, bool flush)
{
    static int count;
    if (flush) count = 0;
    if (count >= max_count) return NULL;
    while (hay[count] != needle)
    {
        ++count;
    }
    return (hay + count);
}

char** count_n_assign(char** buff, int * toAdj)
{
    int count, i;
    char* hold;
    GATEPASS("INTO COUNT N ASSIGN");
    hold = FILE_BUFFER;
    i = count = 0;
    SHOWI("int value is zero? : ", count);
    while ((hold = strstr(hold, "\n")))
    {
        ++hold;
        ++count;
        SHOWI("COUNT OF LINES: ", count);
    }
    *toAdj = count;
    //hold_prev = FILE_BUFFER;
    hold = strtok(FILE_BUFFER,"\n");
    buff = (char**) malloc (count * sizeof(char*));
    i = 0;
    while((hold != NULL) && i < count)
    {
        buff[i] = strdup(hold);
        ++i;
        //hold_prev = hold;
        hold = strtok(NULL, "\n");
    }
    return buff;
}

//THIS IS JESSE'S FUNCTION FROM MM4 IN VID ASSIGNMENT 6
double elapse_time(struct timeval* t0, struct timeval* t1)
{
    double et = ( ( (double) (t1->tv_usec - t0->tv_usec) )/ MICROSECONDS_PER_SECOND) +
                ( (double) (t1->tv_sec - t0->tv_sec) );
    return et;
}

void be_nice(void)
{
    nice(NICE_VALUE);
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

    cleanAllRaggedArrays();

    if (error_occurred) exit_value = EXIT_FAILURE;
    exit(exit_value);
}

void cleanAllRaggedArrays(void)
{
    GATEPASS("INTO CLEAN ALL RAGGED ARRAYS");
    if (GLOBAL_HASH_NUM == 0) SAY("GLOBAL HASH LIST IS EMPTY");
    else
    {
        //SAY("GLOBAL HASH LIST IS NOT EMPTY");
        for (int i = 0; i < GLOBAL_HASH_NUM; ++i)
        {
            SHOWI("GLOBAL HASH NUM: ", i);
            //SHOWI("VERSUS TOTAL NUM: ", GLOBAL_HASH_NUM);
            SAY(GLOBAL_HASH_LIST[i]);
            free(GLOBAL_HASH_LIST[i]);
            GLOBAL_HASH_LIST[i] = NULL;
            SHOWI("GLOBAL CRACKED NUM: ", i);
            SAY(GLOBAL_CRACKED_LIST[i]);
            free(GLOBAL_CRACKED_LIST[i]);
            GLOBAL_CRACKED_LIST[i] = NULL;
        }
    }
    SAY("DEINITIALIZING GLOBAL HASH LIST");
    free(GLOBAL_HASH_LIST);
    GLOBAL_HASH_LIST = NULL;

    SAY("DEINITIALIZING GLOBAL CRACK LIST");
    free(GLOBAL_CRACKED_LIST);
    GLOBAL_CRACKED_LIST = NULL;

    if (GLOBAL_PLAIN_NUM == 0) SAY("GLOBAL PLAIN LIST IS EMPTY");
    else
    {
        for (int i = 0; i < GLOBAL_PLAIN_NUM; ++i)
        {
            SHOWI("GLOBAL PLAIN NUM: ", i);
            SAY(GLOBAL_PLAIN_LIST[i]);
            free(GLOBAL_PLAIN_LIST[i]);
            GLOBAL_PLAIN_LIST[i] = NULL;
        }
    }
    SAY("DEINITIALIZING GLOBAL PLAIN LIST");
    free(GLOBAL_PLAIN_LIST);
    GLOBAL_PLAIN_LIST = NULL;
    GATEPASS("OUT OF CLEAN ALL RAGGED");
}

options_t* getoptions(int argc, char** argv)
{
    const char missing_input[] = "input file(i)";
    const char missing_direc[] = "directory of crackers (d)";
    char c = 0;
    GATEPASS("INTO GETOPTIONS");
    options = (options_t*) malloc(sizeof(options_t));
    *options = option_init;
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
                options->args[3] = strdup(optarg);
                break;
            case 'v': //VERBOSE
                SAY("VERBOSE ON");
                options->verbose = true;
                break;
            case 'h':
                SAY("HELP EXIT");
                options->help = true;
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

void open_file(char* plain_file, char* hash_file, char* outFile)
{
    GATEPASS("OPEN FILE ENTERED");
    if (plain_file  == NULL || hash_file == NULL) 
    {
        perror("PROBLEM, FILENAME IS EMPTY");
        clean_exit();
    }
    PLAIN_IN = open(plain_file, O_RDONLY);
    GATEPASS("OPENED plain FILE");
    if (PLAIN_IN == -1)
    {
        perror("plain file could not be opened");
        clean_exit();
    }
    PLAIN_OPEN = true;
    HASH_IN = open(hash_file, O_RDONLY);
    GATEPASS("OPENED HASH FILE");
    if (HASH_IN == -1)
    {
        perror("hash file could not open");
        clean_exit();
    }
    HASH_OPEN = true;
    if (outFile == NULL)
    {
        SAY("OUT FILE WAS NOT SPECIFIED");
    }
    else
    {
        OUT = open(outFile, O_WRONLY);
        if (OUT == -1)
        {
            perror("Output file could not be open");
            clean_exit();
        }
    }
    OUT_OPEN = true;
}

void close_file(void)
{
    GATEPASS("CLOSE FILE ENTERED");
    if (OUT == STDOUT_FILENO)
        SAY("OUT IS STANDARD OUT, NO CLOSE NECESSARY");
    else
    {
        GATEPASS("CLOSE OUT");
        close(OUT);
    }
}

long init_buf(long buf_size)
{
    GATEPASS("INITIALIZING BUFFER");
    SHOWI("BUFFER SIZE: ", (int) buf_size);
    FILE_BUFFER = (char*) malloc((sizeof(char) * buf_size) + 1);
    FILE_BUFFER[buf_size] = '\0';
    //memset(FILE_BUFFER, 0, buf_size * sizeof(char));
    return buf_size;
}

int read_file(int fd)
{
    int amt_read = 0;
    long size;
    GATEPASS("INTO READ FILE");
    size = init_buf(lseek(fd,0L,SEEK_END));
    lseek(fd,0L,SEEK_SET);
    amt_read = read(fd, FILE_BUFFER, size);
    if (amt_read == -1)
    {
        perror("FAILED TO READ FILE DESCRIPTOR");
        clean_exit();
    }
    FILE_BUFFER[size] = '\0';
    SHOWI("OUT OF READ FILE, AMOUNT READ: ", amt_read);
    return amt_read;
}

int determine_type(char first, char second)
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

int get_next_thread(void)
{
    static int next_row = 0;
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    int curr_row = 0;
    GATEPASS("INTO GET NEXT THREAD");

    pthread_mutex_lock(&lock);
    curr_row = next_row++;
    pthread_mutex_unlock(&lock);

    SHOWI("RETURNING OUT OF GET NEXT THREAD: ", curr_row);
    return curr_row;
}

bool crack_attempt(char* plain, char* hash)
{
    char* cmp;
    struct crypt_data cry;
    if (plain == NULL || hash == NULL) return false;

    memset(&cry, 0, sizeof(struct crypt_data));
    strncpy(cry.setting, hash,CRYPT_OUTPUT_SIZE);
    strncpy(cry.input, hash, CRYPT_MAX_PASSPHRASE_SIZE);
    cmp = crypt_rn(plain,hash,&cry, sizeof(cry));

    if ((strcmp(cry.setting, cmp)) == 0)
    {
        return true;
    }
    return false;
}

void cracked_str(char* key, char* hash, int i)
{
    int size [[maybe_unused]] = 0;
    if (key == NULL)
    {
        GLOBAL_CRACKED_LIST[i] = (char*) malloc((FAILED_ALLOC + strlen(hash) + 2) * (sizeof(char)));
        size = sprintf(GLOBAL_CRACKED_LIST[i], "%s%s\n",PROMPT_FAILED_START,hash);
    }
    else 
    {
        GLOBAL_CRACKED_LIST[i] = (char*) malloc((CRACKED_ALLOC + strlen(key) + strlen(hash) + 3)*(sizeof(char)));
        size = sprintf(GLOBAL_CRACKED_LIST[i], "%s%s  %s\n",CRACKED_START, key, hash);
    }
    SHOW("CRACKED: ", GLOBAL_CRACKED_LIST[i]);
    SHOWI("SIZE WRITTEN: ", size);
}

void* decode(void* vid)
{
    //char *curr_hash, *curr_plain;
    int i,j;
    bool success;
    //curr_hash = curr_plain = NULL;
    j = i = 0;
    for (i = get_next_thread(); i < GLOBAL_HASH_NUM; i = get_next_thread())
    {
        //curr_hash = GLOBAL_HASH_LIST[i];
        //curr_hash = strdup(GLOBAL_HASH_LIST[i]);
        success = false;
        ++(data[(long) vid].total);
        SHOWI("IN DECODE WITH THREAD: ", ((int) ((long) vid)));
        for (j = 0; j < GLOBAL_PLAIN_NUM; ++j)
        {
            //curr_plain = GLOBAL_PLAIN_LIST[i];
            if (crack_attempt(GLOBAL_PLAIN_LIST[j], GLOBAL_HASH_LIST[i]))
            {
                ++(data[(long)vid].encounters[determine_type(GLOBAL_HASH_LIST[i][0], GLOBAL_HASH_LIST[i][1])]);
                SHOWI("THREAD FOUND MATCH: ", ((int) ((long) vid)));
                cracked_str(GLOBAL_PLAIN_LIST[j],GLOBAL_HASH_LIST[i],i);
                success = true;
                break;
            }
        }
        if (!success)
        {
            cracked_str(NULL,GLOBAL_HASH_LIST[i],i);
            ++(data[(long) vid].failures);
        }
    }
    pthread_exit(EXIT_SUCCESS);
}

//void set_thread_data(void)
//{
    //int curr_total, total_total;
    //int num_t = options->threads;
    //int total_i = num_t;
    //total_total = 0;
    //for (int i = 0; i < num_t; ++i)
    //{
        //curr_total = 0;
        //for (int j = 0; j < ALGORITHM_MAX; ++j)
        //{
            //curr_total += data[i].encounters[j];
            //data[total_i].encounters[j] += data[i].encounters[j];
        //}
        //data[total_i].failures += data[i].failures;
        //data[i].total = curr_total += data[i].failures;
        //total_total += data[i].total;
    //}
//}
int length_num(int num)
{
    int next, places;
    places = 0;
    while ((next = num / 10) != 0)
        ++places;
    if (places == 0) return 1;
    return places;
}

void get_thread_data_pretty(thread_data * given, thread_data* last)
{
    int thread, des, nt, md5, sha256, sha512, yes, gyes, b, total, failed;
    double time;
    time = given->time;
    thread = given->thread_num % 1000;
    last->encounters[DES] += des = given->encounters[DES] % 10000;
    last->encounters[NT] += nt = given->encounters[NT]% 10000;
    last->encounters[MD5] += md5 = given->encounters[MD5]% 10000;
    last->encounters[SHA256] += sha256 = given->encounters[SHA256]% 10000;
    last->encounters[SHA512] += sha512 = given->encounters[SHA512]% 10000;
    last->encounters[YESCRYPT] += yes = given->encounters[YESCRYPT]% 10000;
    last->encounters[GOST_YESCRYPT] += gyes = given->encounters[GOST_YESCRYPT]% 10000;
    last->encounters[BCRYPT] += b = given->encounters[BCRYPT]% 10000;
    last->total += total = given->total % 100000000;
    last->failures += failed = given->failures % 100000000;
    snprintf(given->verbose, VERBOSE_LENGTH, "thread:%3d %6.2lf sec              DES: %5d               NT: %5d              MD5: %5d           SHA256: %5d           SHA512: %5d         YESCRYPT: %5d    GOST_YESCRYPT: %5d           BCRYPT: %5d  total: %8d  failed: %8d\n", thread , time, des , nt , md5 , sha256 , sha512 , yes , gyes , b , total, failed); 
}

void display_err(thread_data* datas)
{
    int i;
    for (i = 1; i < options->threads; ++i)
    {
        write(VOUT, datas[i].verbose, VERBOSE_LENGTH - 5);
        write(VOUT, "\n", 1);
    }
    write(VOUT,data[0].verbose,VERBOSE_LENGTH - 5);
    write(VOUT, "\n", 1);
    snprintf(data[i].verbose, VERBOSE_LENGTH, "total: %3d %6.2lf sec              DES: %5d               NT: %5d              MD5: %5d           SHA256: %5d           SHA512: %5d         YESCRYPT: %5d    GOST_YESCRYPT: %5d           BCRYPT: %5d  total: %8d  failed: %8d\n", data[i].thread_num , data[i].time, data[i].encounters[DES] , data[i].encounters[NT]  , data[i].encounters[MD5] , data[i].encounters[SHA256] , data[i].encounters[SHA512] ,data[i].encounters[YESCRYPT]  ,data[i].encounters[GOST_YESCRYPT], data[i].encounters[BCRYPT]  , data[i].total, data[i].failures); 
    write(VOUT,data[i].verbose, VERBOSE_LENGTH - 5);
    write(VOUT, "\n", 1);
}

void display_out(void)
{
    int written;
    for (int i = 0; i < GLOBAL_HASH_NUM; ++i)
    {
        written = write(OUT, GLOBAL_CRACKED_LIST[i], strlen(GLOBAL_CRACKED_LIST[i]));
        if (written == -1) 
        {
            perror("FAILURE TO WRITE TO OUT");
            clean_exit();
        }
    }
}

int main(int argc, char** argv)
{
    long tid;
    long tid_time;
    int count;
    pthread_t * threads [[maybe_unused]] = NULL;
    //----------------OPTIONS---------------------//
    options = getoptions(argc,argv);
    if (options->nice) be_nice();
    if (options->help) help_exit();
    //----------------FILE IO---------------------//
    GATEPASS("PASS OPTIONS IN MAIN");
    open_file(options->args[2], options->args[0], options->args[1]);

    GATEPASS("FILES OPENED");

    count = read_file(PLAIN_IN);
    GLOBAL_PLAIN_LIST = count_n_assign(GLOBAL_PLAIN_LIST, &count);
    GLOBAL_PLAIN_NUM = count;
    if (count == 0) {SAY("COUNT DIDNT TRANSFER");}
    else {SHOWI("COUNT TRANSFERED, NUMBER OF PLAINS: ", GLOBAL_PLAIN_NUM);}

    free(FILE_BUFFER);
    FILE_BUFFER = NULL;
    close(PLAIN_IN);
    PLAIN_OPEN = false;
    GATEPASS("PAST PLAIN");

    count = read_file(HASH_IN);
    GLOBAL_HASH_LIST = count_n_assign(GLOBAL_HASH_LIST, &count);
    GLOBAL_HASH_NUM = count;
    if (count == 0) {SAY("COUNT DIDNT TRANSFER");}
    else {SHOWI("COUNT TRANSFERED, NUMBER OF HASHES: ", GLOBAL_HASH_NUM);}
    free(FILE_BUFFER);
    FILE_BUFFER = NULL;
    close(HASH_IN);
    HASH_OPEN = false;
    GATEPASS("PAST HASH");
    //----------------THREAD MAKING---------------------//
    threads = (pthread_t*) malloc(sizeof(pthread_t) * options->threads);
    data = (thread_data*) calloc(options->threads + 1,sizeof(thread_data));
    times = (struct timeval*) calloc((options->threads * 2) + 2, sizeof(struct timeval));
    GLOBAL_CRACKED_LIST = (char**) malloc(sizeof(char*) * GLOBAL_HASH_NUM);
    GATEPASS("STARTING THREADS");
    data[options->threads].thread_num = options->threads;
    gettimeofday(&times[options->threads * 2], NULL);
    for (tid = 0, tid_time = 0; tid < options->threads; ++tid, tid_time += 2)
    {
        data[tid].thread_num = tid;
        gettimeofday(&times[tid_time], NULL);
        pthread_create(&threads[tid],NULL,decode, (void*) tid);
    }
    for (tid = 0, tid_time = 1; tid < options->threads; ++tid, tid_time += 2)
    {
        pthread_join(threads[tid], NULL);
        gettimeofday(&times[tid_time], NULL);
        data[tid].time = elapse_time(&times[tid_time - 1], &times[tid_time]);
    }
    gettimeofday(&times[(options->threads * 2) + 1], NULL);
    data[options->threads].time = elapse_time(&times[(options->threads) * 2], &times[((options->threads) * 2) + 1]);

    for (int i = 0; i < options->threads; ++i)
    {
        get_thread_data_pretty(&data[i], &data[options->threads]);
    }
    
    display_err(data);
    display_out();

    GATEPASS("ENDED THREADS");
    free(times);
    times = NULL;
    free(threads);
    threads = NULL;
    free(data);
    data = NULL;

    GATEPASS("PASS ALL, EXIT SUCCESS!");
    error_occurred = false;
    clean_exit();
}
    
