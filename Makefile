# Copyright 2023 Vasile Alexandru-Gabriel <vasilealexandru37@gmail.com>

# compiler setup
CC=aarch64-elf-gcc
CFLAGS=-Wall -Wextra -std=c99 

# define targets
TARGETS=main

build:
	$(CC) $(CFLAGS) *.c *.h -o main

pack:
	zip -FSr 314CA_VasileAlexandruGabriel_Tema3.zip README Makefile *.c *.h

clean:
	rm -f *.o *.h.gch

.PHONY: pack