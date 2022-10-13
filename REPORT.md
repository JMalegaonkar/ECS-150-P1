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


## Handle Simple Commands

To handle arguments we created a struct named `Command` that could hold the main command given and its arguments/flags.

```c
typedef struct Command
{
    char* cmd;
    char** args;
    int args_len;
} Command;

```

To populate the attributes of the `Command` struct we create a method called `create_command(input_string)` 

1. Use the `strtok(input_string_copy, " ")` function to parse through the string and find the number of arguments and 
   popluate the `arg_len` attribute.
2. Populate `cmd` atrrubute using a similar `strtok` parser as `(1)` 
3. After the first iteration of `strtok()` in `(2)`, allocate memory for the `args` attribute using malloc and the arg_len 
   found previously;
4. Continue the `strtok()` iteration to populate the `*args` attribute. 
5. Free the necessary memory and return the populated `Command` object.  

## `Special Cases`

`exit`:

`cd`:

1. Check if the `arg_len` is 1 and the command passed into the input is `cd` and store this value in an `int` 
   variable `is_command_cd`. 
2. If `is_command_cd` is equal to 1, check if the directory exists and change the directory using `chdir` else throw an
   error 

`pwd`:
1. Check if the input passed in is `pwd` using `strcmp()`. If true, use `getcwd()` to get the `cwd` and print it

## Implement Piping and Handle Output Redirection
For piping, we created another struct name `CommandPipeline` 

```c
typedef struct CommandPipeline
{
    Command** commands;
    char* output_file;
    int commands_length;
} CommandPipeline;
```
To create a `CommandPipeline` object, the `create_command_pipeline()` method is called. First we validate whether the passed in string is a real command. Next, we parse the string to check if an output redirection exists using `strtok(string, ">")`, and store the string in `*output_file`. As. parse the string for output redirection, we split the string into two segments: the command(s) and the ouput_file; if the output file DNE then segment[1] is `NULL`. Third we take the first segment and use it to parse the pipeline twice using `strtok(string, "|")`. The first parser is used to populate the `command_length` and second is used to create an `Command` object for each command and store it inside `**commands`. Finally ,return the `CommandPipeline` object.

## Standard input redirection

To handle standard input redirection, modified the `Command` struct to include two more attributes: a flag that checks if we are expecting an input file and the input file itself. 

 we added a `strtok()` parser using `<` as the separator to the `create_command` method of the `Command` struct and
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

## Directory stack

To implement `popd`, `pushd`, and `dirs` we desgined a simple stack struct, named `CommandStack` with all the basic methods modified to hold string objects. 

```c
typedef struct CommandStack
{
   char** directory_stack;
   int max_size;
   int top;
} CommandStack;
```
1. `dirs` 
    a. If stack object does not exist print `cwd`. 
    b. If a stack exists the `cwd` is printed and then the stack is printed using a simple for loop ranging from the top value to 0.

2. '`pushd` 
    a. Check if the directory exists else we throw an error 
    b. If directory exists the `push()` method is called -> the top attribute is increment and memory is allocated on the top of the stack for the string `cwd`. (If the stack is at half-capacity, the double_size method will be called and the stack will be readjusted)

3. `popd` 
    a. Change the directory to the top of the stack if top > -1 else throw an error 
    b. If the directory change is sucessful, the `pop()` method is called -> which frees memory for the top command of the stack and then subtract the value of top if top > -1. 


## License

XXX
