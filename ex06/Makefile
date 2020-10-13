# program executable name
TARGET = ex6

# compiler flags
CFLAGS = -std=gnu11 -g -Wall

# linker flags
LDFLAGS = -lpthread -lcomedi -lm

# list of sources
SOURCES = $(shell find -name "*.c")

# default rule, to compile everything
all: $(TARGET)

# define object files
OBJECTS = $(SOURCES:.c=.o)

# link programs
$(TARGET): $(OBJECTS)
	clang	$(LDFLAGS) -o $@ $^

# compile
%.o : %.c
	clang $(CFLAGS) -c -o $@ $<
	
# cleaning
clean:
	rm	-f $(TARGET) $(OBJECTS)