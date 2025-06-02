CXX = g++
CXXFLAGS = -O -std=c++20 -Wall -I ../lexertl17/include -I ../parsertl17/include

LDFLAGS = -O

LIBS = 

all: format_json

format_json: main.o parser.o process.o types.o
	$(CXX) $(LDFLAGS) -o format_json main.o parser.o process.o types.o $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -o main.o -c main.cpp

parser.o: parser.cpp
	$(CXX) $(CXXFLAGS) -o parser.o -c parser.cpp

process.o: process.cpp
	$(CXX) $(CXXFLAGS) -o process.o -c process.cpp

types.o: types.cpp
	$(CXX) $(CXXFLAGS) -o types.o -c types.cpp

library:

binary:

clean:
	- rm *.o
	- rm format_json

