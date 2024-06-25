# IFJ projekt 2023
# Varianta TRP - Hash table
# @author Petr Plíhal, xpliha02

# Pro evu: gcc -Wall -Wextra -pedantic -std=c99 -g $(ls *.c | grep -v tests.c) -o comp
#		   gcc -Wall -Wextra -pedantic -std=c99 -g tests.c -o tests
# Překladač a parametry
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -g

# Soubory které se nemají překládat
FILTER_OUT = 

# Soubory - separátní překlad test.c a všeho ostatního
SRCS = $(filter-out tests.c $(FILTER_OUT), $(wildcard *.c))
TEST_SRCS = tests.c

# Názvy spustitelných souborů
EXEC = comp
TEST_EXEC = tests

# Targety
# Zkompiluje jen comp
all: $(EXEC)

$(EXEC): $(SRCS)
	@$(CC) $(CFLAGS) $^ -o $@

$(TEST_EXEC): $(TEST_SRCS)
	@$(CC) $(CFLAGS) $^ -o $@

# Zkompiluje (zkontroluje) všechno a spustí testy
test: $(EXEC) $(TEST_EXEC)
	@./$(TEST_EXEC) $(filter-out $@,$(MAKECMDGOALS))
%:
	@:
# Smaže všechny spustitelné soubory
clean:
	@rm -f $(EXEC) $(TEST_EXEC)

.PHONY: all clean