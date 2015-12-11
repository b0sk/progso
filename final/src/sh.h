#ifndef SH_H
#define SH_H

#include <stdio.h>		// for printf, rename ...
#include <stdlib.h>		// for exit ...
#include <string.h>		// for strtok, strcmp ...
#include <sys/wait.h>	// for wait, waitpid, WEXITSTATUS ...
#include <unistd.h>		// for chdir, (fork, exec) ...

#define SH_TOK_DELIM " \t\r\n\a"

/* Global parameters */

// The name of this program.
const char* program_name;

/* An int describing log level: 0 = LOW, 1 = MIDDLE, 2 = HIGH */
int loglevel;
/* The prompt of the shell. */
char *prompt;
/* The name of the lofile. */
char *logfile;
/* Append and read file descriptors */
FILE *logfp_a;
FILE *logfp_r;
/* The logging status. 1 = ON, 0 = OFF */
int log_status;


/* 
 * cmd_mode can be INTERNAL or EXTERNAL.
 * ND is usually used when the user enters a newline
 * (because it's not a commmand)
*/
typedef enum{
	INTERNAL,
	EXTERNAL,
	ND
} cmd_mode;

/*
 * command represents a command that the user has entered
 * it's composed by cmd, the actual command and args the arguments.
 * cmd_mode represents the type of command (EXTERNAL or INTERNAL)
 */
typedef struct{
	char *cmd;
	char *args;
	cmd_mode mode;
} command;

/*  
 * given a string generates a command with the correct cmd, args and cmd_mode
 * cmd_mode is set to INTERNAL if the first 
*/
command sh_parse_command(char *line);


/*
 * Executes the coorect command (internal or external) depending
 * depending on the command mode.
 * Returns the exit code of the command.
 */
int sh_launch(command c);

/*
 * sh_launch_int executes an internal command. 
 * Returns the exit code of the command. 
 */
int sh_launch_int(command c);

/*
 * sh_launch_ext executes an external command.
 * Returns the exit code of the command.
 */
int sh_launch_ext(command c);


/*
 * sh_log_command takes as input a command c and its exit status
 * and logs it in the logfile using the logfp_a file descriptor
 * and appending a line at the end.
 * If the loglevel is set to low it logs the timestamp, the command
 * and the exit status. If the loglevel is middle, it adds the arguments
 * and if the loglevel is high it adds the command mode ('i' or 'e').
 * The resulting line is in the format 
 * "[Day, Month DD YYYY - hh:mm:ss] command arguments [e|i] (exit_status)"
*/
void sh_log_command(command c, int exit_status);

/*
 * Prints usage information about this program and exits with exit_code 
 */
void sh_print_usage(int exit_code);

/*
 * Prints the prompt of the shell
 */
void sh_print_prompt(char *prompt);

/*
 * Reads a line from input and returns it.
 */
char *sh_read_line(void);

/*
 * sh_open_logfile opens logfp_a and logfp_r using the file name
 * stored in "logfile". Returns 0 on succes 0, 1 on error
 */
int sh_open_logfile();

/*
 * sh_close_logfile closes the logfp_a and logfp_r file pointers
 */
void sh_close_logfile();


/******************** Internal commands definitions ********************/

/*
 * This function implements the "cd" command:
 * changes the working directory and returns
 * the exit code of chdir.
 */
int sh_cmd_cd(char *args);

/*
 * This function prints the current logging level 
 * The format is The logging level is [low|middle|high]
 */
int sh_cmd_showlevel();

/*
 * This function sets the logging level to lv and returns 0.
 * If the argument is wrong it prints a message and returns 1.
 */
int sh_cmd_setlevel(char *lv);

/* This function turns on the logging (log_status = 1) */
int sh_cmd_logon();

/* This function turns off the logging (log_status = 0) */
int sh_cmd_logoff();

/*
 * This functions reads the logfile and prints the content
 * If there is an error returns 1, else returns 0.
*/
int sh_cmd_logshow();

/*
 * This function sets the prompt to pr and returns o on succes
 * if no arguments is specified, prints and error and returns 1.
*/
int sh_cmd_setprompt(char *pr);

/*
 * This function takes a command as a string
 * and executes it in a new process using a fork
 * returns the exit code of the command, 
 * which is returned by the child process
 * if the fork fails returns -1
 */
int sh_cmd_run(char *comm);

/* This function quits the shell */
int sh_cmd_quit();

/* This function prints the current logfile name */
int sh_cmd_showfile();

/* This function prints information about internal commands */
int sh_cmd_help();



#endif