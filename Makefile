TARGET = Stack.exe
CPPFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -Werror=vla

.PHONY: all
all: $(TARGET)

$(TARGET): main.o Strings\_Strings.o Stack\Stack.h
	g++ main.o Strings\_Strings.o  -o $(TARGET) $(CPPFLAGS)

bin\main.o: main.cpp
	g++ -c main.cpp -o main.o $(CPPFLAGS)

Strings\_Strings.o: Strings\_Strings.cpp Strings\_Strings.h
	g++ -c Strings\_Strings.cpp -o Strings\_Strings.o $(CPPFLAGS)
