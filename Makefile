CC = gcc
CFLAGS = -Wall -Wextra -Werror
OBJFILES = command.o sshell.o
TARGET = sshell


all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

clean:
	rm -f $(OBJFILES) $(TARGET) *~