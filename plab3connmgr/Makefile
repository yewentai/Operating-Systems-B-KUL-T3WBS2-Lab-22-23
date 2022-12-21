test_server: test_server.c lib/tcpsock.c
	gcc test_server.c lib/tcpsock.c -o ./build/test_server

sensor_node: sensor_node.c lib/tcpsock.c
	gcc sensor_node.c lib/tcpsock.c -o ./build/sensor_node

runserver: test_server
	./build/test_server

runclient1: sensor_node
	./build/sensor_node 15 2 127.0.0.1 5678

runclient2: sensor_node
	./build/sensor_node 23 5 127.0.0.1 5678