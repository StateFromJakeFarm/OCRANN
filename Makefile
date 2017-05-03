run: main.o  ann.o
	g++ -o run main.o  ann.o
list.o: ann.cpp ann.h  
	g++ -c -Wall -pedantic -g -std=c++11 ann.cpp
main.o: main.cpp  ann.h 
	g++ -c -Wall -pedantic -g -std=c++11 main.cpp
clean: 
	rm main.o ann.o run
