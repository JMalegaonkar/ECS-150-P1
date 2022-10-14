# SSHELL: Simple Shell

## Summary

The program `sshell` can accept basic user inputs in the form of command line inputs and execute them (Similar to UNIX shells such as `bash` or `zsh`). This basic shell is also configured to support multiple command arguments, command pipelining, as well as input/output redirection.

## Implementation

The implementation of this program follows three distinct steps in a continuous loop until the `exit` command is received:

1. Parse user input into a workable format (`command_pipeline` struct)
2. Handle specific cases such as `exit`, `cd`, `pwd`, `dirs`, `pushd`, and `popd`
3. Enter `fork`, `wait`, `exec` segment to handle general commands

    a. Handle input (`<`) and output (`>`) redirection using `dup2`

    b. Handle pipelined commands through multiple `fork`/`dup2` calls to allow Inter-Process Communication (IPC), using `exec` for individual commands

## Define Command Object

Each command is converted into a struct named `Command` that holds the main command given and its arguments/flags.

```c
typedef struct Command
{
    char* cmd;
    char* input_file;
    char** args;
    int args_len;
    int expects_input_file;
} Command;
```

The `Command` struct only has one method `create_command()` which parses through the input string to populate the `Command` object. Creating the `Command` struct allows us to format the commands so that it is simple for the `CommandPipeline` to store multiple commands

## Converting Input to Commands

```c
typedef struct CommandPipeline
{
    Command** commands;
    char* output_file;
    int commands_length;
} CommandPipeline;
```

The first task our program performs is the conversion of the user input into a `CommandPipeline` object using the `create_command_pipleine()` method. This method has three main helper methods:

1. `parse_output_redirection()` parses through the input string using `strtok` to break the input into
    - A pipelined command substring
    - An output file string (if it exists)

2. `parse_pipelined_commands()` parses through the input, splits the commands to be piped, and converts each command substring into a `Command` object to store in the `CommandPipeline` object

3. `validate_command_pipeline()`/`validate_raw_command_string()` checks for invalid input such as those including:
    - a missing command
    - no output file
    - too many `ls` arguments
    - misplaced output redirection
    - invalid output file


## Handling Special Cases

Our code handles special cases before the `fork`, `wait`, `exec` segment. Each special command is handled in a similar fashion

1. Utilize `strcmp` to check for special case commands
2. Implement functionality of the specific command

Example:
```c
if (!strcmp(command_input, "pwd"))
{
    getcwd(cwd, sizeof(cwd) * sizeof(char));
    fprintf(stdout, "%s\n", cwd);
    fprintf(stderr, "+ completed '%s' [0]\n", command_input);
    continue;
}
```

## Extra Features: Directory stack

To implement `popd`, `pushd`, and `dirs`, we designed a simple stack struct - `CommandStack` - along with specific helper methods that interact with it.

```c
typedef struct CommandStack
{
   char** directory_stack;
   int max_size;
   int top;
} CommandStack;
```

## Handling Pipes and Commands

Once the `CommandPipeline` object has been created, the code goes into the `fork`, `wait`, `exec` segment to execute the commands. The flow of this logic is as follows:
1. `fork` a child process to execute the command
2. In the child process, check how many commands there are to execute since pipelined commands may consist of several sub-commands
    - If there is only a single command, call `execute_single_command()`
    - If there are multiple commands, call `execute_pipelined_command()`
3. In the parent process, wait for the child process to complete executing

**`execute_single_command()`** - Explained in depth
1. `fork` a child process to execute a command using `execute_command()`
2. In the parent process, print the completion message upon completion of the child process

**`execute_pipelined_command()`** - Explained in depth
1. `fork` new processes for each command that needs to be run
2. Utilize `pipe` to configure IPC between these processes
3. Execute all commands using `execute_command()`
4. Await all processes to complete using `waitpid`
5. Print the completion message with status codes for each individual commands

**`execute_command()`** - Explained in depth
1. Handle input redirection if necessary using `dup2`
2. Populate argv and pass command to `execvp()` to be executed

## Memory management

One location where objects are dynamically allocated is in `command_pipeline.c` where `CommandPipeline` and `Command` objects are created. These objects are created on the heap since there can be an arbitrary amount of them and their sizes may vary based on the user input. You'll notice that intermediate `malloc()` calls in this file are `free()`'ed as they're only temporarily needed in the construction of the `CommandPipeline` object.

However, `CommandPipeline` and its data members are never `free()`'ed. This is because upon the program's completion (i.e. `exit()` or `execvp()`, these objects will automatically be `free()`'ed).

Another location of dynamic memory allocation is in `command_stack.c` which is used for the directory stack to handle commands such as `dirs()`, `pushd()`, and `popd()`. This is also never `free()`'ed, but isn't an issue for the same reason as `CommandPipeline` - it is automatically `free()`'ed upon the program's completion.