# Simple shell with logger

This simple shell was implemented as the final poject for the *Operating Systems* course.

### Usage

```
./shell [<option> <argument>]
  -p, --prompt <prompt>
  -l, --loglevel <low|middle|high>
  -f, --logfile <filename>
```

### Special commands

- **!help**: shows a list of special commands
- **!showlevel**: shows the logging level (low|middle|high)
- **!logon**: turns logging on
- **!logoff**: turns logging off
- **!logshow**: shows the log file name and content
- **!setlevel \<loglevel\>**: changes the log level
- **!setprompt \<prompt\>**: changes the shells prompt
- **!run \<command\>**: runs the (normal) command in a different process
- **!quit**: quits the shell
