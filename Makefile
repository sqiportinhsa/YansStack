
all:
	g++ -c Strings/_Strings.cpp

	g++ Main.cpp _Strings.o -o Start.exe
	./Start.exe
