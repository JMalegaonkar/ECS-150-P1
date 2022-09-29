all: sshell

sshell: sshell.c
	gcc -Wall -Wextra -Werror sshell.c command.c -o sshell

clean:
	rm -f sshell