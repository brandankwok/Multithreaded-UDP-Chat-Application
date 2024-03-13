CC=gcc
CFLAGS=-g -Wall -Werror -D _POSIX_C_SOURCE=200809L 

# Define the object files for each source file
OBJECTS=main.o threads.o routines.o list.o

# The default target (executable)
all: s-talk

# Rule to compile each source file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

s-talk: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lpthread


# Target to run valgrind
valgrind:
	valgrind --leak-check=full ./s-talk

# Target to clean up generated files
clean:
	rm s-talk main.o threads.o routines.o
