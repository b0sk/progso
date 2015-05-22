#include <getopt.h>
#include <stdio.h>		// for printf ...
#include <stdlib.h>		// for exit ...
#include <string.h>		// for strtok, strcmp ...
#include <sys/wait.h>	// for wait, waitpid, WEXITSTATUS ...
#include <time.h>		// to get the timestamp

//#define SH_TOK_DELIM " \t\r\n\a"

// The name of this program.
const char* program_name;

/* Global parameters */
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
/* The logging status. Default is ON (1). 1 = ON, 0 = OFF */
int log_status = 1;

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

/* Parse the command line arguments given by the user
 * and save the result in the global variables.
 */
void parse_args(int argc, char *argv[]){
	// A string listing valid short options
	const char* const short_options = "p:l:f:";
	// An array describing the valid long options
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
					print_usage(EXIT_FAILURE);
				}
				break;
			case 'f':	// -f o --logfile
				printf("Option LOGFILE with argument: %s\n", optarg);
				logfile = optarg;
				break;
			case '?':	// Opzione non valida.
				/* getopt_long prints an error message */
				print_usage(EXIT_FAILURE); // Print usage and exit with error
				break;
			default:	// Opzione non riconosciuta.
				printf("Invalid option.\n");
				print_usage(EXIT_FAILURE); // Print usage and exit with error
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
	printf("TODO: launch an internal commmand.\n");
	return 1;
}

/* Function that executes an external command. Returns the exit code of the command. */
int sh_launch_ext(char *cmd){
	if(cmd[0]=='c' && cmd[1]=='d'){
		printf("CD COMMAND FOUND!\n");
	}
	/* system() returns the exit status in the waitpid() format, 
	 * we use WEXITSTATUS to get the correct value.
	 */
	return WEXITSTATUS(system(cmd));
}

/* Function that removes leading white spaces from line */
static void remove_leading_spaces(char** line) 
{   
   int i;
   for(i = 0; (((*line)[i] == ' ') || (*line)[i] == '\t' ); i++) { }
   *line += i;
}
/* This function returns a string with the current timestamp 
 * in the format "Day, Month DD YYYY - hh:mm:ss"
 */
char *current_timestamp(){
	char *s = malloc(sizeof(char) * 1000);//[1000];
	time_t t = time(NULL);
	struct tm * p = localtime(&t);
	strftime(s, 1000, "%A, %B %d %Y - %T", p);
	return s;
}

void log_command(char cmd_line[], char cmd_mode, int exit_status){
	//printf("TODO: log command <%s> [%c] [%i]\n", cmd_line, cmd_mode, exit_status);
	/* log the current timestamp */
	printf("[%s] ", current_timestamp());

	/* If loglevel is middle or high, log the command + arguments */
	char *pch = strtok(cmd_line, " \n");
	if (loglevel >= 1){
		while (pch != NULL){
			printf("%s ", pch);
			pch = strtok(NULL, " \n");
		}
	} else { /* else log only the command */
		printf("%s ", pch);
	}

	/* If loglevel is high, log command mode (external or internal) */
	if(loglevel == 2){
		printf("[%c] ", cmd_mode);
	}

	/* Log the exit status */
	printf("(%i)\n", exit_status);
}

int main (int argc, char* argv[]){
	/* Set the program name from argv[0]; */
	program_name = argv[0];

	/* Parse command line arguments */
	parse_args(argc, argv);

    /*
     * Main shell loop
    */
    while(!feof(stdin)) {
    	char *line;		// Contains the line from input
    	char cmd_mode; // 'e' = external command, 'i' = internal command
    	int exit_status; 

    	sh_print_prompt(prompt);
    	
    	line = sh_read_line();
		/* Remove initial empty chars */
    	remove_leading_spaces(&line);

    	if (line[0] == '!'){
    		printf("Internal command!\n");
    		cmd_mode = 'i';
    		exit_status = sh_launch_int(line);
    	}else{
    		printf("External command\n");
    		cmd_mode = 'e';
    		exit_status = sh_launch_ext(line);
    	}

    	/* If logging is active log the command */
    	if(log_status == 1){
    		log_command(line, cmd_mode, exit_status);
    	}

    	//printf("-----> %s", line);
    }

	return 0;
}
