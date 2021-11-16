all: client server
	$(info "Succesfully compiled binaries.")

client:
	g++ client.cpp -o client

server:
	g++ server.cpp -o server

copy:
	scp -P 14501 Makefile .\client.cpp tc@localhost:.
	scp -P 14503 Makefile .\server.cpp tc@localhost:.

clean:
	rm -f client server