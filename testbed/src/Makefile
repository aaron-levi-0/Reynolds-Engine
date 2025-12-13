CC = gcc

CFLAGS 		= -I "../../Reynolds_Engine/engine/src" \
			  -I "../vendor/" \
              -I "../../Reynolds_Engine/engine/vendor" -Wall -Wextra -O2
LDFLAGS1 	= -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -lglew32 

# Locate all source files for the engine and Minesweeper project
ENGINE_SOURCES = $(wildcard ../../Reynolds_Engine/engine/src/*.c) \
                 $(wildcard ../../Reynolds_Engine/engine/src/core/*.c) \
                 $(wildcard ../../Reynolds_Engine/engine/src/utils/*.c) \
                 $(wildcard ../../Reynolds_Engine/engine/src/events/*.c) \
				 $(wildcard ../../Reynolds_Engine/engine/src/renderer/*.c) \
				 $(wildcard ../../Reynolds_Engine/engine/vendor/*.c)
				 
MINESWEEPER_SOURCES = 	$(wildcard *.c) \
						$(wildcard ../vendor/*.c) \

# Generate object files from source files
OBJECTS = $(MINESWEEPER_SOURCES:.c=.o) $(ENGINE_SOURCES:.c=.o)

EXECUTABLE = minesweeper

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@$(CC) $^ -o $@ $(LDFLAGS1)

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o $(EXECUTABLE)

#full clean: clean object files in engine as well
fclean:
	@rm -f *.o $(EXECUTABLE)
	@rm -f ../../Reynolds_Engine/engine/src/*.o ../../Reynolds_Engine/engine/src/core/*.o ../../Reynolds_Engine/engine/src/utils/*.o ../../Reynolds_Engine/engine/src/events/*.o ../../Reynolds_Engine/engine/src/renderer/*.o \
			../../Reynolds_Engine/engine/vendor/*.o 

print-debug:
	@echo SOURCES: $(ENGINE_SOURCES)$(MINESWEEPER_SOURCES)
	@echo OBJECTS: $(OBJECTS)