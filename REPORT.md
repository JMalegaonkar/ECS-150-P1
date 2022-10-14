# SSHELL: Simple Shell

## Summary

The program `sshell` can accept basic user inputs under the form of command lines and execute them (Similarly to UNIX shells such as `bash` or `zsh`). This basic shell is also configured to support multiple command arguments, command pipelining, as well as input/output redirection.

## Implementation

The implementation of this program followings three distinct steps in an continous loop until the `exit` command is recieved:

1. Parse user input into a workable format (`command_pipeline` struct)
2. Handle specific cases such as `exit`, `cd`, `pwd`, `dirs`, `pushd`, and `popd`
3. Enter `fork`, `wait`, `exec` segment to handle general commands

    a. Handle input (`<`) and output (`>`) redirection using `dup2`

    b. Handle pipelined command through recusive `fork`/`dup2` calls to allow Inter-Process Communication (IPC) between multiple processes handling individual commands using `exec`

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

The `Command` struct only has one method `create_command()` which does two things:
1. first parses through the input string and checks if input redirection exists and populates the `input_file` atrribute
2. parses through the in the input string using `strtok` and populates the attributes of `Command`

Creating the `Command` struct allows us to format the commands so that it is simple for the `CommandPipeline` to store multiple commands

## Converting Input to Commands

```c
typedef struct CommandPipeline
{
    Command** commands;
    char* output_file;
    int commands_length;
} CommandPipeline;
```

The first task our program does is convert the user input into a `CommandPipeline` object using the `create_command_pipleine` method. This method has three main helper functions:
    
1. `parse_output_redirection()` parses through the input string using `strtok` and returns an array of size 2 with the command or pipeline and the output file

2. `parse_pipelined_commands()` parses through array[0] and separates the pipeline,converts each command into a `Command` object and stores it in the `CommandPipeline` object 

3. `validate_command_pipeline()` checks for invalid input such as having `ls` having multiple directories as arguments or the output file being nonexistant


## Handle Special Cases

Our code handles special cases before the `fork`, `wait`, `exec` segment: each special command is handled in a similar fashion

1. use `strcmp` to check if input command and `the special case command` are the same
2. implement what the command should do inside the if statement 

`Example:`
```c
if (!strcmp(command_pipeline->commands[0]->cmd, "dirs"))
{
        get_commands(command_stack, getcwd(cwd, sizeof(cwd) * sizeof(char)));
        fprintf(stderr, "+ completed '%s' [0]\n", command_input);

        continue;
}
```

## Handle Pipes and Commands

### TALK ABOUT THE RECURSION AND WHEN TO USE RECURSION AND WHEN NOT TO, LOGIC BEHIND IT ###

Once the `CommandPipeline` object has been created, the code goes into the `fork`, `wait`, `exec` segment to execute the commands:

1. If the object's command_length is 1, we call the `execute_single_command()` function which 

2.

3.

4.




## Special Case: Directory stack

To implement `popd`, `pushd`, and `dirs` we desgined a simple stack struct, named `CommandStack` with all the basic methods modified to hold string objects. 

```c
typedef struct CommandStack
{
   char** directory_stack;
   int max_size;
   int top;
} CommandStack;
```



## License

XXX
