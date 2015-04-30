#include <stdio.h> 
#include <time.h> 
void main() { 
	char s[1000]; 
	time_t t = time(NULL); 
	struct tm * p = localtime(&t); 
	strftime(s, 1000, "%A, %B %d %Y", p); 
	printf("[%s]\n", s); 
} 
