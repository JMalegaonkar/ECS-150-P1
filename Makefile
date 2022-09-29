all: sshell

sshell: sshell.c
	gcc -Wall -Wextra -Werror command.c sshell.c -o sshell

clean:
	rm -f sshell