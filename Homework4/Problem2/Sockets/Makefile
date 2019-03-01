all:
	arm-linux-gcc server.c -o server -pthread -lrt
	arm-linux-gcc client.c -o client -pthread -lrt

clean:
	rm -rf server client
