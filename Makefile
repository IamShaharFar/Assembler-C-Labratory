CC = gcc
CFLAGS = -Wall -ansi -pedantic
SRCDIR = Source_Files
INCDIR = Header_Files

# Source and object files
SOURCES = $(SRCDIR)/assembler.c \
          $(SRCDIR)/errors.c \
          $(SRCDIR)/preprocessor.c \
          $(SRCDIR)/preprocessor_utils.c\
          $(SRCDIR)/first_pass.c\
          $(SRCDIR)/first_pass_utils.c\
          $(SRCDIR)/command_utils.c\
          $(SRCDIR)/label_utils.c\
          $(SRCDIR)/second_pass.c\
          $(SRCDIR)/output_builder.c\
          $(SRCDIR)/utils.c \
          $(SRCDIR)/vpc_utils.c \
          $(SRCDIR)/globals.c

OBJECTS = $(SOURCES:.c=.o)

# Header dependencies
HEADERS = $(INCDIR)/errors.h \
          $(INCDIR)/first_pass.h \
          $(INCDOIR)/first_pass_utils.h \
          $(INCDOIR)/command_utils.h \
          $(INCDOIR)/label_utils.h \
          $(INCDOIR)/second_pass.h \
          $(INCDOIR)/output_builder.h \
          $(INCDIR)/preprocessor.h \
          $(INCDIR)/preprocessor_utils.h \
          $(INCDIR)/utils.h \
          $(INCDIR)/structs.h \
          $(INCDIR)/globals.h \
          $(INCDIR)/vpc_utils.h 

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
