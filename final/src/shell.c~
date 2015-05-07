#include <getopt.h>
#include <stdio.h>	//for printf
#include <stdlib.h>	//for exit

// The name of this program.
const char* program_name;

/*
 * Prints usage information about this program and exits with exit_code 
 */
void print_usage(int exit_code){
	printf("Usage: %s options\n", program_name);
	printf(
			"  -p --prompt prompt	  Set the prompt of the shell\n"
			"  -l --loglevel level	  Set the loglevel [low, middle, high]\n"
			"  -f --logfile filename   Set the logfile name\n"
	);
	exit(exit_code);
}

int main (int argc, char* argv[]){
	// A string listing valid short options
	const char* const short_options = "p:l:f:";
	// An array describing valid long options
	const struct option long_options[] = {
		{ "prompt",		required_argument, NULL, 'p' },
		{ "loglevel",	required_argument, NULL, 'l' },
		{ "logfile",	required_argument, NULL, 'f' },
		{ 0, 0, 0, 0 } //Required
	};
	
	// Set the program name from argv[0];
	program_name = argv[0];
	
	printf("%s\n", program_name);
	print_usage(-1);
	return 0;
}
