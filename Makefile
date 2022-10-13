CC = gcc
CFLAGS = -Wall -Wextra -Werror
OBJFILES = string_utilities.o command_stack.o command_pipeline.o command.o sshell.o
TARGET = sshell


all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) -lm

clean:
	rm -f $(OBJFILES) $(TARGET) *~