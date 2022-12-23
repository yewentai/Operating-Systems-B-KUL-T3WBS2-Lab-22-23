TITLE_COLOR = \033[33m
NO_COLOR = \033[0m

server : main.c connmgr.c datamgr.c sbuffer.c sensor_db.c ./lib/dplist.c ./lib/tcpsock.c
	gcc main.c connmgr.c datamgr.c sbuffer.c sensor_db.c ./lib/dplist.c ./lib/tcpsock.c -o server -Wall -fdiagnostics-color=auto -std=c11 -Werror -DSET_MIN_TEMP=10 -DSET_MAX_TEMP=20 -DTIMEOUT=5

file_creator : file_creator.c
	gcc file_creator.c -o file_creator -Wall -fdiagnostics-color=auto

client : sensor_node.c ./lib/tcpsock.c
	gcc sensor_node.c ./lib/tcpsock.c -o client -Wall -fdiagnostics-color=auto

# do not look for files called clean, clean-all or this will be always a target
.PHONY : clean clean-all run zip

clean:
	rm -rf *.o sensor_data sensor_data_text gateway.log sensor_data.csv ./build/out ./lib/*.o

clean-all: clean
	rm -rf lib/*.so

runserver : ./server
	./server 5678

runclient : ./client
	./client 15 1 127.0.0.1 5678

zip:
	zip lab_final.zip main.c connmgr.c connmgr.h datamgr.c datamgr.h sbuffer.c sbuffer.h sensor_db.c sensor_db.h config.h lib/dplist.c lib/dplist.h lib/tcpsock.c lib/tcpsock.h