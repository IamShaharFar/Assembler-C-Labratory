CC = gcc
CFLAGS = -Wall -ansi -pedantic
SRCDIR = Source_Files
INCDIR = Header_Files

# Source and object files
SOURCES = $(SRCDIR)/assembler.c \
          $(SRCDIR)/errors.c \
          $(SRCDIR)/file_processor.c \
          $(SRCDIR)/preprocessor.c \
          $(SRCDIR)/utils.c \
          $(SRCDIR)/preprocessor_utils.c

OBJECTS = $(SOURCES:.c=.o)

# Header dependencies
HEADERS = $(INCDIR)/errors.h \
          $(INCDIR)/utils.h \
          $(INCDIR)/file_processor.h \
          $(INCDIR)/globals.h \
          $(INCDIR)/preprocessor.h \
          $(INCDIR)/preprocessor_utils.h

# Executable name
EXEC = assembler

# Include path for header files
INC = -I$(INCDIR)

# Default target
all: $(EXEC)

# Linking the executable
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(CFLAGS)

# Generic rule for compiling .c to .o
$(SRCDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Clean target
clean:
	rm -f $(EXEC) $(OBJECTS)

# Print variables for debugging
debug:
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Headers: $(HEADERS)"

.PHONY: all clean debug
