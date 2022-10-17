all:
	mkdir -p build/
	mkdir -p recv-files/
	g++ std=C++14 source/*.cpp ../file-manager/file-manager.cpp -o build/server
run:
	mkdir -p build/
	mkdir -p recv-files/
	g++ -std=c++14 source/*.cpp ../file-manager/file-manager.cpp -o build/server
	./build/server
clean:
	rm -rf build/
	rm -rf recv-files/
	rm pwd.txt