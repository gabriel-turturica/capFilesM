TARGET = convert

all:
	g++ *.cpp -std=c++17 $(shell root-config --cflags --glibs) -lcapnp -lkj -o $(TARGET)
