#ifndef __CCLEAN_FUNCTIONS_H
#define __CCLEAN_FUNCTIONS_H

/* Source state for rm_comments() */
enum SState { CODE, COMMENT, STRING, WAIT_NL };

/**
 * Remove C comments (ansi and c99) from the given dump
 *
 * @param  char* dump to parse, should be one source code line
 * @param  enum  SState state of the code
 * @return int   number of removed characters
 */
int rm_comments(char *s, enum SState *state);

/**
 * This function reads the given file in pathname and creates a new
 * file called "pathname.clean". Empty lines and C comment lines
 * are removed.
 *
 * @param const char* pathname file to be cleaned
 * @param char* buf buffer
 * @param size_t nbytes buffer size in bytes
 * @return int -1 in error
 */
int clean_file(const char *pathname, char *buf, size_t nbytes);

/* Return 1 if string is empty */
int str_isempty(const char *str);

#endif
