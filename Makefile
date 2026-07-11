TARGET 		:= build/lang
SOURCES 	:= $(wildcard src/*.c)
OBJECTS 	:= $(SOURCES:src/%.c=build/%.o)
FLAGS			:= -Wall -Wextra -g -MMD -MP

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(FLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf build

-include $(OBJECTS:.o=.d)
