# program executable name
TARGET = ex3

# compiler flags
CFLAGS = -g -lpthread -fsanitize=thread

# linker flags
LDFLAGS = -g -lpthread -fsanitize=thread

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
