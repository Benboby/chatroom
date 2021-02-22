server: server.o
		g++ server.o -o server
server.o:
		g++ -c server.cpp server.h log.h
client: client.o
		g++ client.o -o client
client.o:
		g++ -c client.cpp client.h log.h
clean:
		rm -rf *.o