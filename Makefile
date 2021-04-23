# ELF SysV Hash Crack Makefile

CC=gcc
LD=gcc
INDENT_FLAGS=-br -ce -i4 -bl -bli0 -bls -c4 -cdw -ci4 -cs -nbfda -l100 -lp -prs -nlp -nut -nbfde -npsl -nss

all: elfcrack

prepare:
	@mkdir -p bin

elfcrack: prepare
	@echo "  CC    elfcrack.c"
	@$(CC) -Wall -O2 -c src/elfcrack.c -o bin/elfcrack.o
	@echo "  LD    elfcrack"
	@$(LD) bin/elfcrack.o -o bin/elfcrack

analyse:
	@cppcheck src/elfcrack.c
	@scan-build make

install:
	@cp elfcrack /usr/bin/elfcrack

uninstall:
	@rm -f /usr/bin/elfcrack

indent:
	@find ./ -type f -exec touch {} +
	@indent $(INDENT_FLAGS) src/elfcrack.c
	@rm -fv src/*~

clean:
	@echo "  CLEAN ."
	@rm -rf bin
