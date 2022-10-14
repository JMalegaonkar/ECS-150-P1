# SSHELL: Simple Shell

## Summary

The program `sshell` can accept basic user inputs under the form of command lines and execute them (Similarly to UNIX shells such as `bash` or `zsh`). This basic shell is also configured to support multiple command arguments, command pipelining, as well as input/output redirection.

## Implementation

The implementation of this program followings three distinct steps in an continous loop until the `exit` command is recieved:

1. Parse user input into a workable format (`command_pipeline` struct)
2. Handle specific cases such as `exit`, `cd`, `pwd`, `dirs`, `pushd`, and `popd`
3. Enter `fork`, `wait`, `exec` segment to handle general commands

    a. Handle input (`<`) and output (`>`) redirection using `dup2`

    b. Handle pipelined command through multiple `fork`/`dup2` calls to allow Inter-Process Communication (IPC), using `exec` for individual commands

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

The first task our program does is convert the user input into a `CommandPipeline` object using the `create_command_pipleine()` method. This method has three main helper functions:

1. `parse_output_redirection()` parses through the input string using `strtok` to break the input into
    - A pipelined command substring
    - An output file string (if it exists)

2. `parse_pipelined_commands()` parse through the input, separating on pipelines, and converting each command substring into a `Command` object to store in the `CommandPipeline` object

3. `validate_command_pipeline()`/`validate_raw_command_string()` check for invalid input such as those with:
    - a missing command
    - no output file
    - too many `ls` arguments
    - misplaced output redirection
    - invalid output file


## Handle Special Cases

Our code handles special cases before the `fork`, `wait`, `exec` segment: each special command is handled in a similar fashion

1. Utilize `strcmp` check for `special case commands`
2. Implement functionality of specfic command

`Example:`
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

To implement `popd`, `pushd`, and `dirs`, we desgined a simple stack struct - `CommandStack` - along with specific helper methods interact with it.

```c
typedef struct CommandStack
{
   char** directory_stack;
   int max_size;
   int top;
} CommandStack;
```

## Handle Pipes and Commands

Once the `CommandPipeline` object has been created, the code goes into the `fork`, `wait`, `exec` segment to execute the commands. The flow of this logic is as follow:
1. `fork()` to execute command in child, while waiting for completition in parent
2. In the child, check how many commands there are to execute (pipelined commands may have multiple individual chained commands)
    - If only single command, call `execute_single_command()`
    - If multiple commands, call `execute_pipelined_command`

**`execute_single_command()`**
1. `fork()` child process to execute command using `execute_command()`
2. In parent process, print completion message upon completion of child process

**`execute_pipelined_command()`**
1. `fork()` new processes for each command that needs to be run
2. Utilize `pipe()` to configure IPC between these processes
3. Execute all commands using `execute_command()`
3. Await all processes to complete using `waitpid()`
4. Print completion message with status codes for each individual command

**`execute_command()`**
1. Handle input redirection if necessary using `dup2`
2. Populate argv and pass command to `execvp()` to be executed

### Memory management

One location where objects are dynamically allocated is in `command_pipeline.c` where `CommandPipeline` and `Command` objects are created. These objects are created on the heap since there can be an arbitrary amount of them and the sizes of them vary based on the specific user input. You'll notice that intermediate `malloc()` calls in this file are `free()`'ed as they're only temporarily needed in the construction of the `CommandPipeline` object.

However, `CommandPipeline` and its data members are never `free()`ed. This is because upon the program's completion (i.e. `exit()` or `execvp()`, these objects will automatically be freed).

Another location of dynamic memory allocation is in `command_stack.c` which is used for the directory stack to handle commands such as `dirs()`, `pushd()`, and `popd()`. This also is never freed, but isn't an issue for the same reason as `CommandPipeline` - it is automatically freed upon the program's completion.