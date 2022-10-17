all:
	mkdir -p build/
	mkdir -p recv-files/
	g++ -std=c++14 source/*.cpp ../file-manager/file-manager.cpp -o build/client
run:
	mkdir -p build/
	mkdir -p recv-files/
	g++ -std=c++14 source/*.cpp ../file-manager/file-manager.cpp -o build/client
	./build/client
clean:
	rm -rf build/
	rm -rf recv-files/
	rm pwd.txt