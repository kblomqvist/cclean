/**
 * cclean is a program to clean C source files.
 *
 * The program starts a child process for each given file. The child processes
 * will remove every empty and comment line from the file and write thus the
 * "cleaned" code into a new file with name "original_name.clean".
 * 
 * Comment line can be either:
 * 
 * - One starting with characters "//" and ending with new line
 * - One starting with characters / and * and ending with corresponding * and /.
 *
 * Parent process waits until the child processes have died before quitting.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include "functions.h"

#define VERSION "v0.1"

#define BUFFSIZE 4096

void init_sigaction(struct sigaction *sig);
static void sig_handler(int signo);
void show_help(void);

int main(int argc, char **argv)
{
	int opt = 0, i = 0, pstat, nremoved;
	struct sigaction sig;
	pid_t pid;
	char buf[BUFFSIZE];
	long child_max = 0;

	if (argc == 1) {
		fprintf(stderr, "Usage: cclean [OPTION] [FILE] ...\n");
		exit(1);
	}

	/* Init signal handlers */
	init_sigaction(&sig);

	/* Handle options */
	while ((opt = getopt(argc, argv, "hv")) != -1) {
		switch (opt) {
			case 'h':
				show_help();
				exit(0);
			case 'v':
				printf("cclean %s, compiled on %s %s.\n", VERSION, __TIME__, __DATE__);
				exit(0);
			default:
				fprintf(stderr, "Use -h for more information.\n");
				exit(1);
		}
	}

	/* Check maximum number of child processes */
	if ((child_max = sysconf(_SC_CHILD_MAX)) < (argc - 1)) {
		errno = E2BIG;
		perror(argv[0]);
		exit(1);
	}

	/* Create child processes for input file */
	for (i = 1; i < argc; i++) {
		if ((pid = fork()) < 0) {
			perror(argv[0]);
		}

		/* Child process */
		if (pid == 0) {
			if ((nremoved = clean_file(argv[i], buf, BUFFSIZE)) < 0) {
				perror("clean_file");
				exit(i);
			}
			printf("%s: Cleaned; %d characters removed\n", argv[i], nremoved);
			exit(i);
		}
	}

	/* Wait for child processes */
	while ((pid = wait(&pstat)) >= 0);

	puts("byebye.");
	return 0;
}

void init_sigaction(struct sigaction *sig)
{
	memset(sig, 0x00, sizeof(*sig));
	sigemptyset(&(sig->sa_mask));
	sig->sa_handler = sig_handler;
	sigaction(SIGINT, sig, NULL);
}

static void sig_handler(int signo)
{
	if (signo == SIGINT) {
		kill(0, signo);
		exit(1);
	}
}

void show_help(void)
{
	printf("Usage: cclean [OPTION] [FILE]...\n");
	printf("Clean C source FILE(s) from comment lines and empty lines.\n");
	printf("The cleaned file is written into a new file postfixed by \".clean\".\n\n");

	printf("  -h\tdisplay this help and exit\n");
	printf("  -v\toutput version information and exit\n");
	
	printf("\nExamples:\n");
	printf("  cclean foo.c bar.c\n");
}

