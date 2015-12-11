/*
 * The functions defined in this file are documented in utils.h
 */

#include "utils.h"

char* str_concat(char *s1, char *s2){
    char *result = malloc(strlen(s1)+strlen(s2)+2); //+1 for the zero-terminator + 1 for space
    //error check malloc here!
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}

char *current_timestamp(){
	char *s = malloc(sizeof(char) * 400);//[1000];
	time_t t = time(NULL);
	struct tm * p = localtime(&t);
	strftime(s, 1000, "%A, %B %d %Y - %T", p);
	return s;
}

void remove_leading_spaces(char** line){  
   int i;
   for(i = 0; (((*line)[i] == ' ') || (*line)[i] == '\t' ); i++) { }
   *line += i;
}