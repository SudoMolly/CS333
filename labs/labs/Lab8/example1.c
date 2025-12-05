// R Jesse Chaney
// rchaney@pdx.edu

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <libgen.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>

// -n #  for the input number to calc the factorial
// -h    show some, lame, help
// -v    show a little verbosity
// -s    call pause() and await a signal to continue. See the
//           signal_handler function for how signals are handled
#define OPTIONS "n:hvs"
#define ENEG_VALUE 2
#define ETOOOOOOOBIG_VALUE 3
#define SIGNALS 1

static bool is_verbose = false;

long factorial(long n);

#ifdef SIGNALS
void signal_handler(int);

void
signal_handler(int sig)
{
	switch(sig) {
	case SIGINT:
		fprintf(stderr, "%d: a SIGINT was received\n", __LINE__);
		break;
	case SIGQUIT:
		fprintf(stderr, "%d: a SIGQUIT was received\n", __LINE__);
		break;
	case SIGHUP:
		fprintf(stderr, "%d: a SIGHUP was received\n", __LINE__);
		break;
	case SIGUSR1:
		fprintf(stderr, "%d: a SIGUSR1 was received\n", __LINE__);
		[[fallthrough]];  // Explicitly marks intentional fallthrough
	case SIGUSR2:
	default:
		fprintf(stderr, "%d: Signal %d received\n", __LINE__, sig);
		break;
	}
}
#endif // SIGNALS

long
factorial(long n)
{
	long result = 1;
	
	if (is_verbose) {
		fprintf(stderr, "%d: entering %s with input %ld\n"
				, __LINE__, __FUNCTION__, n);
	}
	errno = 0;
	if (n < 0) {
		errno = ENEG_VALUE;
		return -1; // Error for negative input
	}
	if ((n == 0) || (n == 1)) {
		return 1;
	}

	for (int i = 2; i <= n; i++) {
		result *= i;
		if (result < 0) {
			// overflow
			errno = ETOOOOOOOBIG_VALUE;
			break;
		}
	}

	if (is_verbose) {
		fprintf(stderr, "%d: exiting %s with result %ld\n"
				, __LINE__, __FUNCTION__, result);
	}
	
	return result;
}

int
main(int argc, char *argv[])
{
	long value = 0;

#ifdef SIGNALS
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGHUP, signal_handler);
	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);
#endif // SIGNALS
	{
		int opt = 0;

		while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
			switch (opt) {
			case 'n':
				int result = sscanf(optarg, "%ld", &value);
				if (result < 1) {
					fprintf(stderr, "Invalid value for input: %s\n", optarg);
					exit(EXIT_FAILURE);
				}
				break;
			case 'v':
				is_verbose = true;
				break;
			case 'h':
				fprintf(stderr, "Help is on the way\n\tUsage: %s %s\n"
						, basename(argv[0]), OPTIONS);
				exit(EXIT_SUCCESS);
				break;
			case 's':
				pause();
				break;
			default:
				fprintf(stderr, "Bad arguement\n\tUsage: %s %s\n"
						, basename(argv[0]), OPTIONS);
				exit(EXIT_FAILURE);
			}
		}
	}

	{
		long *fact = malloc(sizeof(long));
		if (fact == NULL) {
			perror("malloc failed");
			fprintf(stderr, "%d: malloc failed\n", __LINE__);
			exit(EXIT_FAILURE);
		}
		*fact = factorial(value);
		if (errno != 0) {
			fprintf(stderr, "Invalid input: %ld\n", value);
			exit(EXIT_FAILURE);
		}
		printf("Factorial of %ld: %ld\n", value, *fact);
		free(fact);
	}

	return EXIT_SUCCESS;
}
