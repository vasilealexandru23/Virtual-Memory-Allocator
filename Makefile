# Copyright 2023 Vasile Alexandru-Gabriel <vasilealexandru37@gmail.com>

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 

# define targets
TARGETS=main

build:
	$(CC) $(CFLAGS) *.c *.h -o vma
	
run_vma:
	./vma

pack:
	zip -FSr 314CA_VasileAlexandruGabriel_Tema3.zip README Makefile *.c *.h

clean:
	rm -f *.o *.h.gch

.PHONY: pack