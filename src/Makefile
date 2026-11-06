# ***************************************************************
#
# Makefile genérico para compilar programas em C
# usando o compilador gcc
#
# ***************************************************************

# CFLAGS = -Wall -ansi -pedantic -O2 -lm
# CFLAGS = -Wall -std=c99 -pedantic -O2 -lm
# CFLAGS = -Wall -std=c99 -pedantic -O2 -lm -fsanitize=address,undefined
# CFLAGS = -std=c99 -g -Wall -pedantic -O2 -lm -fsanitize=address
# CFLAGS = -std=c99 -g -Wall -pedantic -lm -fsanitize=address
# CFLAGS = -std=c99 -g -Wall -pedantic -O2 -lm 
CFLAGS = -std=c99 -Wall -pedantic -O2 -lm -g -D_GNU_SOURCE

RM = /bin/rm -f
CC = gcc
# CC = clang

# ***************************************************************

.c:
	$(CC) $*.c -o $* $(CFLAGS)

# ***************************************************************

clean:
	$(RM) -r "*~" "#*" ".\#*" "*.o core a.out" 
	$(RM) -rf "*.dSYM"
	find . -maxdepth 1 -executable -type f -not -name "*.c" -not -name "Makefile" -delete
