all: main.o server.o file-manager.o
	g++ build/main.o build/server.o ../file-manager/file-manager.o -o build/server
main.o:
	g++ -c source/main.cpp -o build/main.o
server.o:
	g++ -c source/server.cpp -o build/server.o
file-manager.o:
	g++ -c ../file-manager/file-manager.cpp -o ../file-manager/file-manager.o
clean:
	rm -rf build/
	rm -rf recv-files/
	rm -f pwd.txt