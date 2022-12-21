FLAGS+=-pthread
SOURCE=main.c sbuffer.c
EXE=build/out
CC=gcc

all: $(SOURCE)
	mkdir -p build
	$(CC) -g *.c -o $(EXE) $(FLAGS)

.PHONY : clean check zip

check : $(SOURCE) 
	@echo "Running cppcheck :"
	cppcheck --enable=all --suppress=missingIncludeSystem --error-exitcode=1 $(SOURCE) 

clean:
	rm -f  $(EXE)  sensor_data sensor_data_out sensor_data.txt sensor_data_out.txt

zip:
	zip milestone3.zip *.c *.h