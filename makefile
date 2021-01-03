all: databac

databac: clean main.o fileIO.o HTTP1.o TCPServer.o utils.o
	gcc -o databac main.o fileIO.o HTTP1.o TCPServer.o utils.o

main.o:
	gcc -c main.c
	 
fileIO.o:
	gcc -c fileIO.c

HTTP1.o:
	gcc -c HTTP1.c
	
TCPServer.o:
	gcc -c TCPServer.c
	
utils.o:
	gcc -c utils.c
	
clean:
	rm -f databac main.o fileIO.o HTTP1.o TCPServer.o utils.o
