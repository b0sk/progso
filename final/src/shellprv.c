#include <getopt.h>
#include <stdio.h>		// for printf ...
#include <stdlib.h>		// for exit ...
#include <string.h>		// for strtok, strcmp ...
#include <sys/wait.h>	// for wait, waitpid, WEXITSTATUS ...
#include <time.h>		// to get the timestamp
#include <unistd.h>		// for chdir, (fork, exec) ...

#define SH_TOK_DELIM " \t\r\n\a"

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


/* ------------------------------------------------------------------------------- */
typedef enum{
	INTERNAL,
	EXTERNAL,
	ND	// Used mainly for newline
} cmd_mode;

typedef struct{
	char *cmd;
	char *args;
	cmd_mode mode;
} command;

command parse_command(char *line){
	command c;
	/* Split the string on the first space. And set c.cmd and c.args */
	c.cmd = strtok_r(line, SH_TOK_DELIM, &c.args);
	/* Remove new line char at the end of args */
	if (c.args[strlen(c.args)-1] == '\n'){
		c.args[strlen(c.args)-1] = '\0';
	}

	if(c.cmd == NULL){
		c.cmd = "-";
		c.args = "-";
		c.mode = ND;
	}else if(c.cmd[0] == '!'){
		c.mode = INTERNAL;
	}else{
		c.mode = EXTERNAL;
	}
	return c;
}


/* Concatenates 2 strings with a spece in between */
char* str_concat(char *s1, char *s2){
    char *result = malloc(strlen(s1)+strlen(s2)+2); //+1 for the zero-terminator + 1 for space
    //error check malloc here!
    strcpy(result, s1);
    strcat(result, " ");
    strcat(result, s2);
    return result;
}


int sh_launch(command c){
	int exit_status;
	if (c.mode == EXTERNAL){
		exit_status = sh_launch_ext(c);
	}else if (c.mode == INTERNAL){
		exit_status = sh_launch_int(c);
	}else {
		exit_status = 0;
	}
	return exit_status;
}

/* Function that executes an internal command. Returns the exit code of the command. */
int sh_launch_int(command c){
	//printf("TODO: launch an internal commmand.\n");

	int exit_code = 1;
	if (strcmp(c.cmd, "!showlevel") == 0){
		//printf("Internal command showlevel\n");
		exit_code = sh_cmd_showlevel();
	}
	else if (strcmp(c.cmd, "!logon") == 0){
		//printf("Internal command logon\n");
		exit_code = sh_cmd_logon();
	}
	else if (strcmp(c.cmd, "!logoff") == 0){
		//printf("Internal command logoff\n");
		exit_code = sh_cmd_logoff();
	}
	else if (strcmp(c.cmd, "!logshow") == 0){
		printf("Internal command logshow\n");
	}
	else if (strcmp(c.cmd, "!setlevel") == 0){
		//printf("Internal command setlevel\n");
		exit_code = sh_cmd_setlevel(c.args);
	}
	else if (strcmp(c.cmd, "!setprompt") == 0){
		//printf("Internal command setprompt\n");
		exit_code = sh_cmd_setprompt(c.args);
	}
	else if (strcmp(c.cmd, "!run") == 0){
		printf("Internal command run\n");
	}
	else if (strcmp(c.cmd, "!quit") == 0){
		//printf("Internal command quit\n");
		exit_code = sh_cmd_quit();
	}
	else if (strcmp(c.cmd, "!help") == 0){
		//printf("Internal command help\n");
		exit_code = sh_cmd_help();
	}
	else {
		printf("Internal command not found!\n");
	}

	return exit_code;
}

/* Function that executes an external command. Returns the exit code of the command. */
int sh_launch_ext(command c){
	int exit_code;

	/* If the command "cd" is found call the builtin function. Else execute the command */
	if(strcmp(c.cmd, "cd") ==0 ){
		exit_code = sh_cmd_cd(c.args);
	}else{
		/* system() returns the exit status in the waitpid() format, 
		 * we use WEXITSTATUS to get the correct value.
		 */
		char *str = str_concat(c.cmd, c.args);
		exit_code = WEXITSTATUS(system(str));
		free(str);
	}

	return exit_code;
}

/* This function returns a string with the current timestamp 
 * in the format "Day, Month DD YYYY - hh:mm:ss"
 */
char *current_timestamp(){
	char *s = malloc(sizeof(char) * 400);//[1000];
	time_t t = time(NULL);
	struct tm * p = localtime(&t);
	strftime(s, 1000, "%A, %B %d %Y - %T", p);
	return s;
}

void log_command(command c, int exit_status){
	/* log the current timestamp */
	char *timestamp = current_timestamp();
	printf("[%s] ", timestamp);

	/* If loglevel is middle or high, log the command + arguments */
	if (loglevel >= 1){
		char *str = str_concat(c.cmd, c.args);
		printf("%s ", str);
		free(str);
	} else { /* else log only the command */
		printf("%s ", c.cmd);
	}

	/* If loglevel is high, log command mode (external or internal).
	 * If the command is ND (a new line) print "[-]".
	 */
	if(loglevel == 2){
		char cmd_mode;
		switch(c.mode){
			case INTERNAL:
				cmd_mode = 'e';
				break;
			case EXTERNAL:
				cmd_mode = 'i';
				break;
			default:
				cmd_mode = '-';
				break;
		}
		printf("[%c] ", cmd_mode);
	}

	/* Log the exit status */
	printf("(%i)\n", exit_status);

	free(timestamp);
}

/* ------------------------------------------------------------------------------- */

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
	if(getline(&line, &buffsize, stdin) == -1){
		//perror("sh error");
		free(line);
		exit(EXIT_FAILURE);
	}
	return line;
}


/* Internal commands implementation */

/* This function implements the "cd" command:
 * changes the working directory and returns
 * the exit code.
 */
int sh_cmd_cd(char *args){
	char *path = strtok(args, SH_TOK_DELIM);
	int exit_code = chdir(path);
	if (exit_code != 0) {
  		perror("cd");
	}
	return exit_code;
}

/* This function prints the current logging level */
int sh_cmd_showlevel(){
	char *level;
	switch(loglevel){
		case 0:
			level = "low";
			break;
		case 1:
			level = "middle";
			break;
		case 2:
			level = "high";
			break;
		default:
			return 1;
	}
	printf("The logging level is [%s].\n", level);
	return 0;
}

/* This function turns on the logging (log_status = 1) */
int sh_cmd_logon(){
	log_status = 1;
	printf("Logging turned ON.\n");
	return 0;
}

/* This function turns off the logging (log_status = 0) */
int sh_cmd_logoff(){
	log_status = 0;
	printf("Logging turned OFF.\n");
	return 0;
}

/* This function sets the logging level to lv and returns 0.
 * If the argument is wrong it prints a message and returns 1.
 */
int sh_cmd_setlevel(char *lv){
	int ret = 0;
	// if lv is low set loglevel to 0
	if(strcmp(lv, "low") == 0) {
		loglevel = 0;
		printf("Logging level set to [low].\n");
	}
	// if arg is middle set loglevel to 1
	else if(strcmp(lv, "middle") == 0){
		loglevel = 1;
		printf("Logging level set to [middle].\n");
	}
	// if arg is middle set loglevel to 2
	else if(strcmp(lv, "high") == 0){
		loglevel = 2;
		printf("Logging level set to [high].\n");
	}
	// else show how to use command
	else{
		ret = 1;
		printf("Invalid option. Possible levels are: low, middle or high.\n");
	}
	return ret;
}

/* This function sets the prompr to pr */
int sh_cmd_setprompt(char *pr){
	int ret = 0;
	if(pr[0]=='\0'){
		ret = 1;
		printf("!setprompt: expected argument.\n");
	} else {
		prompt = pr;
	}
	return ret;
}

/* This function quits the shell */
int sh_cmd_quit(){
	exit(EXIT_SUCCESS);
}

/* This function prints information about internal commands */
int sh_cmd_help(){
	printf(	"Valid internal commands are:\n"
			"\n!help\n >Show this help message.\n"
			"\n!showlevel\n >Show the current logging level.\n"
			"\n!logon\n >Turns logging ON.\n"
			"\n!logoff\n <Turns logging OFF\n"
			"\n!logshow\n >Shows the log file content.\n"
			"\n!setlevel <low|middle|high>\n >Sets the logging level to the specified value.\n"
			"\n!setprompt <prompt>\n >Set the prompt to the specified string.\n"
			"\n!run <command>\n >Runs a command in a separate process.\n"
			"\n!quit\n >Quits the shell\n"
		);
	return 0;
}


/* Function that removes leading white spaces from a string */
static void remove_leading_spaces(char** line) 
{   
   int i;
   for(i = 0; (((*line)[i] == ' ') || (*line)[i] == '\t' ); i++) { }
   *line += i;
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
    	int exit_status; 

    	sh_print_prompt(prompt);
    	
    	line = sh_read_line();
		/* Remove initial empty chars */
		char *ln = line;
    	remove_leading_spaces(&ln);

    	command c = parse_command(ln);
    	//printf("CMD: %s\n", c.cmd);
    	//printf("ARGS: %s\n", c.args);
		//printf("CMD_MODE: %c\n", c.cmd_mode);
    	
    	exit_status = sh_launch(c);

    	/* If logging is active log the command */
    	if(log_status == 1){
    		log_command(c, exit_status);
    	}

    	free(line); //needed?!?!

    	//printf("-----> %s", line);
    }

	return 0;
}
