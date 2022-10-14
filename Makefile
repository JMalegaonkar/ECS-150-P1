CC = gcc
CFLAGS = -Wall -Wextra -Werror
OBJFILES = string_utilities.o command_stack.o command_pipeline.o command.o sshell.o
LDFLAGS = -lm
TARGET = sshell


all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS) 

clean:
	rm -f $(OBJFILES) $(TARGET) *~