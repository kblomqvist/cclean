#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include "functions.h"

int rm_comments(char *str, enum SState *state)
{
	int i = 0, j = 0;

	/* Loop until nul or EOF */
	for (; str[i]; i++) {
		switch (*state) {
			case COMMENT:
				if (str[i] == '*' && str[i+1] == '/') {
					i++;
					*state = CODE;
				}
				break;
			case CODE:
				if (str[i] == '/' && str[i+1] == '*') {
					i++;
					*state = COMMENT;
					break;
				}
				else if (str[i] == '/' && str[i+1] == '/') {
					*state = WAIT_NL;
					break;
				}

				if (str[i] == '"' && str[i-1] != 39) /* 39 == ' */
					*state = STRING;

				str[j++] = str[i]; /* Keep char */
				break;
			case STRING: 
				if (str[i] == '"' && str[i-1] != 92) /* ignore escaped " char */
					*state = CODE;

				str[j++] = str[i]; /* Keep char */
				break;
			case WAIT_NL:
				if (str[i] == '\n') {
					*state = CODE;
					str[j++] = '\n';
				}
				break;
		}
	}

	/* Place ending nul */
	str[j] = '\0';

	return i-j; /* Number of removed chars */
}

int clean_file(const char *pathname, char *buf, size_t nbytes)
{
	int output, ntowrite, nwrite, nremoved = 0;
	struct flock fl = {F_WRLCK, SEEK_SET, 0, 0, 0};
	char *output_pathname;
	enum SState state = CODE;
	FILE *input;

	fl.l_pid = getpid();

	if ((input = fopen(pathname, "r")) == NULL) {
		perror(pathname);
	   	return -1;
	}
	
	output_pathname = malloc(strlen(pathname) + 6 + 1);
	strcpy(output_pathname, pathname);
	strcat(output_pathname, ".clean");

	if ((output = creat(output_pathname, 0644)) < 0) {
		perror(output_pathname);
	   	return -1;
	}

	if (fcntl(output, F_SETLK, &fl) == -1) {
		perror(output_pathname);
		return -1;
	}

	while(fgets(buf, nbytes, input) != NULL) {
		nremoved += rm_comments(buf, &state);

		if (!str_isempty(buf)) {
			ntowrite = strlen(buf);
			if ((nwrite = write(output, buf, ntowrite)) < 0) {
				perror(output_pathname);
				return -1;
			}
		}
		else
			nremoved += strlen(buf);
	}

	fl.l_type = F_UNLCK;
	if (fcntl(output, F_SETLK, &fl) == -1) {
		perror("fcntl");
		return -1;
	}

	fclose(input);
	close(output);
	free(output_pathname);

	return nremoved;
}

int str_isempty(const char *str)
{
	int i = 0;

	while (str[i])
		if (!isspace(str[i++]))
			return 0;

	return 1;
}
