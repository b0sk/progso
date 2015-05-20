#include <getopt.h>
#include <stdio.h>		// for printf ...
#include <stdlib.h>		// for exit ...
#include <string.h>		// for strtok, strcmp ...
#include <sys/wait.h>	// for wait, waitpid, WEXITSTATUS ...

#define SH_TOK_DELIM " \t\r\n\a"

// The name of this program.
const char* program_name;

/*
 * Prints usage information about this program and exits with exit_code 
 */
void print_usage(int exit_code){
	printf("Usage: %s <option> <argument>\n", program_name);
	printf(
			"  -p, --prompt <prompt>		Set the prompt of the shell\n"
			"  -l, --loglevel <level>	Set the loglevel [low, middle, high]\n"
			"  -f, --logfile <filename>	Set the logfile name\n"
	);
	exit(exit_code);
}

/*
 * Prints the prompt of the shell
 */
void sh_print_prompt(char *prompt){
	printf("%s ", prompt);
}

/*
 * Reads a line from input
 */
char *sh_read_line(void){
	char *line = NULL;
	ssize_t buffsize = 0; // getline allocates a buffer
	getline(&line, &buffsize, stdin);
	return line;
}

/* Function that executes an internal command. Returns the exit code of the command. */
int sh_launch_int(char *cmd){
	printf("TODO: launch an external commmand.\n");
	return 1;
}

/* Function that executes an external command. Returns the exit code of the command. */
int sh_launch_ext(char *cmd){
	/* system() returns in the waitpid() format, 
	 * we use WEXITSTATUS to get the correct value.
	 */
	return WEXITSTATUS(system(cmd));
}

int main (int argc, char* argv[]){
	/* Set the program name from argv[0]; */
	program_name = argv[0];

	/* 
	 * An int describing log level:
	 * 0 = LOW, 1 = MIDDLE, 2 = HIGH
	 * The default is MIDDLE (1)
	 */
	int loglevel = 1;

	/* The prompt of the shell. Default is "->" */
	char *prompt = "->";

	/* The name of the lofile. Default is "shell.log" */
	char *logfile = "shell.log";

	// A string listing valid short options
	const char* const short_options = "p:l:f:";
	// An array describing valid long options
	const struct option long_options[] = {
		{ "prompt",		required_argument, NULL, 'p' },
		{ "loglevel",	required_argument, NULL, 'l' },
		{ "logfile",	required_argument, NULL, 'f' },
		{ 0, 0, 0, 0 } //Required
	};
	
	int next_opt;
	while(1){
		/* getopt_long stores the option index here. */
		int next_opt = getopt_long (argc, argv, short_options, long_options, NULL);
		/* Detect the end of the options. */
		if (next_opt == -1)
			break;
		switch(next_opt){
			case 0:
				//printf("Case 0!\n");
				break;
			case 'p':	// -p o --prompt
				printf("Option PROMPT with argument: %s\n", optarg);
				/* Set the prompt from argument */ 
				prompt = optarg;
				break;
			case 'l':	// -l o --loglevel
				printf("Option LOGLEVEL with argument: %s\n", optarg);
				
				// if arg is low set loglevel to 0
				if(strcmp(optarg, "low") == 0) 
					loglevel = 0;
				// if arg is middle set loglevel to 1
				else if(strcmp(optarg, "middle") == 0)
					loglevel = 1;
				// if arg is middle set loglevel to 2
				else if(strcmp(optarg, "high") == 0)
					loglevel = 2;
				// else print usage and exit with error
				else{
					printf("Invalid argument for option --loglevel\n");
					print_usage(-1);
				}
				break;
			case 'f':	// -f o --logfile
				printf("Option LOGFILE with argument: %s\n", optarg);
				logfile = optarg;
				break;
			case '?':	// Opzione non valida.
				/* getopt_long prints an error message */
				print_usage(-1); // Print usage and exit with error
				break;
			default:	// Opzione non riconosciuta.
				printf("Invalid option.\n");
				print_usage(-1); // Print usage and exit with error
		}
	}
	
	/* Print any remaining command line arguments (not options). */
	if (optind < argc){
		printf ("non-option ARGV-elements: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		putchar ('\n');
		
		print_usage(-1); // Print usage and return error
    }

    /*
     * Main shell loop
    */
    do {
    	char *line;		/* Contains the line from input */
    	char cmd_mode; /* 'e' external command, 'i' internal command */
    	int exit_status; 

    	sh_print_prompt(prompt);
    	line = sh_read_line();
		/* Remove initial empty chars */
    	/* TODO */ 

    	if (line[0] == '!'){
    		printf("Internal command!\n");
    		cmd_mode = 'i';
    		exit_status = sh_launch_int(line);
    	}else{
    		printf("External command\n");
    		cmd_mode = 'e';
    		exit_status = sh_launch_ext(line);
    		printf("Exit code: %i\n", exit_status);
    	}

    	//printf("-----> %s", line);
    } while(1);

	return 0;
}
