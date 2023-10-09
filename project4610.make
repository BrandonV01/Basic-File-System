project4610: sdisk.o main.o filesys.o block.o shell.o table.o
	g++ sdisk.o main.o filesys.o block.o shell.o table.o -o project4610

sdisk.o: sdisk.cpp
	g++ -c sdisk.cpp

main.o: main.cpp
	g++ -c main.cpp

filesys.o: filesys.cpp
	g++ -c filesys.cpp

block.o: block.cpp
	g++ -c block.cpp

shell.o: shell.cpp
	g++ -c shell.cpp

table.o: table.cpp
	g++ -c table.cpp

clean:
	rm *.o