# -------------------------------------------------------
# Makefile for C Practice Problems
# -------------------------------------------------------
# USAGE:
#   make run SRC=swap.c       - compile swap.c, run it, delete binary
#   make run SRC=calculator.c - same for any other file
#   make clean SRC=swap.c     - manual cleanup
#
# NOTES:
#   - Binary name is derived from your .c filename automatically
#   - No need to edit this Makefile between problems
#   - -Wall turns on warnings — good habit early on!
# -------------------------------------------------------

CC     = gcc
CFLAGS = -Wall
BIN    = $(basename $(SRC))   # e.g. swap.c → swap

run: $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)
	./$(BIN)
	rm -f $(BIN)

clean:
	rm -f $(BIN)

