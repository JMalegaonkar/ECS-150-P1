CC = gcc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS =
OBJFILES = command.o sshell.o
TARGET = sshell



all: $(TARGET)


$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~