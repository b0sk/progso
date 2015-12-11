# sets the C compilr to gcc
CC = gcc

# it's a list of paths separated by columns that
# tells make where to look for required files
VPATH = src:bin:obj

# this targets are not files
.PHONY: bin clean test interactivetest

# prints a brief desription of the project and a list of the targets
# this target will be invoked if make is called without parameters
usage:
	@echo "Interactive shell: executes given system commands and some internal commands."
	@echo "Available targets:"
	@echo "- usage (or no target): shows this message"
	@echo "- bin: creates the shell binary in the bin/ folder"
	@echo "- obj: creates object files in the obj/ folder"
	@echo "- assets: creates input files used as example in the assets/ folder"
	@echo "- test: automaticaly test the program"
	@echo "- interactivetest: starts the interactive shell for testing"
	@echo "- cleantemp: removes temporary files levaing only source, bin and assets files"
	@echo "- clean: removes assets, executables, objects and log files"


# builds the shell binary in bin/
bin: shell
	$(MAKE) cleantemp

# builds object fiels in obj/
obj: sh.o utils.o

# removes the directories leaving only the source files and deletes logfiles
clean:
	@rm -rf bin
	@rm -rf obj
	@rm -rf assets
	@rm -f *.log

# generates asset files and creates the assets/ directory if necessary
assets:
	mkdir -p assets
	@echo "echo STARTTING TEST 1" > assets/asset1
	@echo "echo This test creates a a new fodler and then deletes it." >> assets/asset1
	@echo "echo -- ls" >> assets/asset1
	@echo "ls" >> assets/asset1
	@echo "echo - mkdir prova" >> assets/asset1
	@echo "mkdir prova" >> assets/asset1
	@echo "echo -- ls" >> assets/asset1
	@echo "ls" >> assets/asset1
	@echo "echo -- rm -r prova" >> assets/asset1
	@echo "rm -r prova" >> assets/asset1
	@echo "echo -- ls" >> assets/asset1
	@echo "ls" >> assets/asset1
	@echo "!quit" >> assets/asset1

# removes all the temporary files and directories but leaves the bin and assets folders
cleantemp:
	@rm -rf bin/*.log
	@rm -rf obj
	@rm -rf assets
	@rm -f *.log

# builda the shell binary and creates the bin/ directory if necessary
shell: clean shell.c sh.o utils.o
	@mkdir -p bin
	$(CC) src/shell.c obj/utils.o obj/sh.o -o bin/shell

# builds the shell library and creates the obj/ directory if necessary
sh.o: clean sh.c sh.h utils.o
	@mkdir -p obj
	$(CC) -c src/sh.c -o obj/sh.o

# builds the utils library and creates the obj/ directory if necessary
utils.o: clean utils.c utils.h
	@mkdir -p obj
	$(CC) -c src/utils.c -o obj/utils.o

# builds the shell and start the program for testing, clears everything after the test
test: clean bin assets
	clear
	@./bin/shell -p "--" -f bin/shell.log < assets/asset1
	$(MAKE) clean

# builds the shell and start an interactive test, clears everything after the test
interactivetest: clean bin
	$(MAKE) cleantemp
	clear
	@./bin/shell -f bin/shell.log
	$(MAKE) clean
