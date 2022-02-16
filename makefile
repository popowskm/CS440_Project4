main: main.cpp
	g++ -std=c++11 main.cpp -o main.out

debug: main.cpp
	g++ -std=c++11 main.cpp -g -o main.out