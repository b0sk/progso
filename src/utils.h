#ifndef UTILS_H
#define UTILS_H

#include <string.h>		// for strtok, strcmp ...
#include <time.h>		// to get the timestamp
#include <stdlib.h>		// for exit ...

/*
 * Concatenates 2 strings with a spece in between 
 * and returns the resulting string.
 */
char* str_concat(char *s1, char *s2);

/*
 * This function returns a string with the current timestamp 
 * in the format "Day, Month DD YYYY - hh:mm:ss"
 */
char *current_timestamp();

/* Function that removes leading white spaces from the string line */
void remove_leading_spaces(char** line);

#endif