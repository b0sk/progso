/*
 * The functions defined in this file are documented in utils.h
 */

#include "sh.h"
#include "utils.h"

command sh_parse_command(char *line){
	command c;
	/*
	 * Split the string in two parts on the first space
	 * and set c.cmd and c.args
	 */
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

int sh_launch_int(command c){
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
		//printf("Internal command logshow\n");
		exit_code = sh_cmd_logshow();
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
		//printf("Internal command run\n");
		exit_code = sh_cmd_run(c.args);
	}
	else if (strcmp(c.cmd, "!quit") == 0){
		//printf("Internal command quit\n");
		exit_code = sh_cmd_quit();
	}
	else if (strcmp(c.cmd, "!showfile") == 0){
		//printf("Internal command help\n");
		exit_code = sh_cmd_showfile();
	}	
	else if (strcmp(c.cmd, "!help") == 0){
		//printf("Internal command help\n");
		exit_code = sh_cmd_help();
	}
	else {
		printf("Internal command not found. Use !help to see a list of commands.\n");
	}

	return exit_code;
}

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


void sh_log_command(command c, int exit_status){
	/* Check if the file descriptor logfp_a is NULL and print warning message */
	if(logfp_a == NULL){
		printf("Warning: can't log the command.\n");
	} else {
		/* log the current timestamp */
		char *timestamp = current_timestamp();
		fprintf(logfp_a, "[%s] ", timestamp);

		/* If loglevel is middle or high, log the command + arguments */
		if (loglevel >= 1){
			char *str = str_concat(c.cmd, c.args);
			fprintf(logfp_a, "%s ", str);
			free(str);
		} else { /* else log only the command */
			fprintf(logfp_a, "%s ", c.cmd);
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
			fprintf(logfp_a, "[%c] ", cmd_mode);
		}

		/* Log the exit status */
		fprintf(logfp_a, "(%i)\n", exit_status);

		fflush(logfp_a);
		free(timestamp);
	}
}

void sh_print_usage(int exit_code){
	printf("Usage: %s <option> <argument>\n", program_name);
	printf(
			"  -p, --prompt <prompt>		Set the prompt of the shell\n"
			"  -l, --loglevel <level>	Set the loglevel [low, middle, high]\n"
			"  -f, --logfile <filename>	Set the logfile name\n"
	);
	exit(exit_code);
}

void sh_print_prompt(char *prompt){
	printf("%s ", prompt);
}

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

int sh_open_logfile(){
	int retval = 0;
	logfp_a = fopen(logfile, "a");
	logfp_r = fopen(logfile, "r");
	if (logfp_r == NULL){
		perror("Error opening the logfile in reading mode");
		retval = 1;
	}
	if (logfp_a == NULL){
		perror("Error opening the logfile. Commands won't be logged");
		retval = 1;
	}
	return retval;
}

void sh_close_logfile(){
	if(logfp_a != NULL){
		fclose(logfp_a);
	}
	if (logfp_r != NULL)	{
		fclose(logfp_r);
	}
}


/*********************** Internal commands implementations ***********************/

int sh_cmd_cd(char *args){
	char *path = strtok(args, SH_TOK_DELIM);
	int exit_code = chdir(path);
	if (exit_code != 0) {
  		perror("cd");
	}
	return exit_code;
}

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

int sh_cmd_logon(){
	log_status = 1;
	printf("Logging turned ON.\n");
	return 0;
}

int sh_cmd_logoff(){
	log_status = 0;
	printf("Logging turned OFF.\n");
	return 0;
}

int sh_cmd_logshow(){
	/* Check if the file descriptor logfp_r is NULL and print warning message */
	if (logfp_r == NULL){
		printf("Can't read the logfile.\n");
		return 1;
	} else {
		char *line = NULL;
		size_t len = 0;

		/* Go back to the beginning of the file */
		rewind(logfp_r);
		//fseek(logfp_r, 0, SEEK_SET) // This is the same

		/* Read file line by line and print it */
		while(getline(&line, &len, logfp_r) != -1){
			printf("%s", line);
		}

		if(line)
			free(line);
		return 0;
	}
}

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

int sh_cmd_run(char *comm){
	pid_t pid;
	int status = 0;
	pid = fork();
	if(pid == 0){
		/* Child proccess */
		printf("Running command in new process.\n");
		int exit_status = WEXITSTATUS(system(comm));
		exit(exit_status);
	} else if(pid < 0) {
		/* Error forking */
		perror("error creating child process");
		return -1;
	} else {
		/* Parent process */
		/* wait for the children and save the exit status */
		waitpid(pid, &status, WUNTRACED);
		return WEXITSTATUS(status);
	}
}

int sh_cmd_quit(){
	sh_close_logfile();
	exit(EXIT_SUCCESS);
}

int sh_cmd_showfile(){
	printf("The logfile name is [%s]\n", logfile);
	return 0;
}

int sh_cmd_help(){
	printf(	"Valid internal commands are:\n"
			"\n!help\n >Show this help message.\n"
			"\n!logon\n >Turns logging ON.\n"
			"\n!logoff\n >Turns logging OFF\n"
			"\n!logshow\n >Shows the log file content.\n"
			"\n!showfile\n >Shows the logfile name.\n"
			"\n!showlevel\n >Show the current logging level.\n"
			"\n!setlevel <low|middle|high>\n >Sets the logging level to the specified value.\n"
			"\n!setprompt <prompt>\n >Set the prompt to the specified string.\n"
			"\n!run <command>\n >Runs a command in a separate process.\n"
			"\n!quit\n >Quits the shell\n"
		);
	return 0;
}
