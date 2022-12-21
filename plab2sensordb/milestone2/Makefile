HEADERS=*.h
SOURCE=*.c
EXE=build/out
CC=gcc

all: $(SOURCE)
	mkdir -p build
	$(CC) -g *.c -o $(EXE) 

.PHONY : clean check

check : $(SOURCE) $(HEADERS)
	@echo "Running cppcheck :"
	cppcheck --enable=all --suppress=missingIncludeSystem --error-exitcode=1 $(SOURCE) $(HEADERS)

clean:
	rm -f *.o $(EXE) *.csv *.log

zip:
	zip milestone2.zip *.c *.h Makefile