# SSHELL: Simple Shell

## Summary

The program `sshell` can accept basic user inputs under the form of command lines and execute them (Similarly to UNIX shells such as `bash` or `zsh`). This basic shell is also configured to support multiple command arguments, command pipelining, as well as input/output redirection.

## Implementation

The implementation of this program followings XXX distinct steps in an continous loop until the `exit` command is recieved:

1. Parse user input into a workable format (`command_pipeline` struct)
2. Handle specific cases such as `exit`, `cd`, `pwd`, `dirs`, `pushd`, and `popd`
3. Enter `fork`, `wait`, `exec` segment to handle general commands

    a. Handle input (`<`) and output (`>`) redirection using `dup2`

    b. Handle pipelined command through recusive `fork`/`dup2` calls to allow Inter-Process Communication (IPC) between multiple processes handling individual commands using `exec`



## Phase 0 & 1

We transformed the given skeleton code into `fork`+`exec`+`wait` method (expand more)


## Phase 2

To handle arguments we created a struct named `Command` that could hold the main command given and its arguments/flags.

```c
typedef struct Command
{
    char* cmd;
    char** args;
    int args_len;
} Command;

```

To populate the attributes of the `Command` struct we create a method called `create_command()`, which takes in the input passed into the sshell. First we create two copies of the input string. We use the strtok() function and the first string copy to parse through the string, separating by whitespace, and find the number of arguments and popluate the `arg_len` attribute. Next, we populate `cmd` atrrubute using the same `strtok` parser, but with the second copy of the string. After the first iteration of `strtok()`, we allocate memory for the `args` attribute using malloc and the arg_len found previously; then continue the `strtok()` iteration to populate the `*args` attribute. Finally we free the necessary memory and return a populated `Command` object.  

## Phase 3

`cd`:
to implement `cd` , we first check if the `arg_len` is 1 and the command passed into the input is `cd` and store this value in an `int` variable `is_command_cd`. If `is_command_cd` is equal to 1, we check if the directory exists and change the directory using `chdir` else throw an error

`pwd`:
first check if the input passed in is `pwd` using `strcmp()`. If true, use `getcwd()` to get the input and print it

## Phase 4 & 5
For piping, we created another struct name `CommandPipeline` 

```c
typedef struct CommandPipeline
{
    Command** commands;
    char* output_file;
    int commands_length;
} CommandPipeline;
```

## Phase 6
Standard input redirection:
To handle standard input redirection, we added a `strtok()` parser using `<` as the separator to the `create_command` method of the `Command` struct and modified the `Command` struct to include two more attributes: a flag that checks if we are expecting an input file and the input file itself. 

The parser works by first populate `expects_input_file` with the output of `strchr(input_string, '<')`. Next create another copy of the `input_string` and parse through it using `strtok()`

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

Directory stack:

To implement `popd`, `pushd`, and `dirs` we desgined a simple stack struct, named `CommandStack` with all the basic methods modified to hold string objects. 

```c
typedef struct CommandStack
{
   char** stack;
   int max_size;
   int top;
} CommandStack;
```

When '`pushd` is called by the sshell, we first check if the directory exists else we throw an error; if true the `push()` method is called, the top attribute is increment and memory is allocated on the top of the stack for the string `cwd`. (If the stack is at half-capacity, the double_size method will be called and the stack will be readjusted)

The `dirs` command is implemeted such that when a stack does not exist it prints `cwd`. If a stack exists the `cwd` is printed and then the stack is printed using a simple for loop ranging from the top value to 0.

The `popd` command 


## License





XXX
