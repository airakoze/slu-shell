# SLU Shell


slush - SLU shell

slush is a very simple command-line interpreter. It uses a different syntax than shells like bash, and has much less functionality. slush executes in a loop in which it displays a prompt, reads in a command line from standard input, and executes the command.

There are two types of commands: built in commands which are executed by slush itself, and program execution commands which are carried out by separate processes.

A built in command must appear on a line by itself. The only built in command is:

cd dir - change current directory to dir

Program execution commands have the form:

prog_n [args] ( ... prog_3 [args] ( prog_2 [args] ( prog_1 [args]

This command runs the programs prog_n, ... , prog_2, prog_1 (each of which may have zero or more arguments) as separate processes in a "pipeline". This means the standard output of each process is connected to the standard input of the next.

The syntax of slush is backwards from shells you're used to, and is intended to emphasize the functional nature of pipeline commands. As an example, the command line:

more ( sort ( ps aux

should produce a paginated, sorted list of processes.

slush should catch ^C typed from the keyboard. If a command is running, this should interrupt the command. If the user is entering a line of input, slush should respond with a new prompt and a clean input line.

slush will exit when it reads an end-of-file on input.
