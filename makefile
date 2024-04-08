CXX = g++
CXXFLAGS = -std=c++20

SRCS = main.cpp Player.cpp Room.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = main

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp Player.h Room.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Player.o: Player.cpp Player.h
	$(CXX) $(CXXFLAGS) -c Player.cpp

Room.o: Room.cpp Room.h
	$(CXX) $(CXXFLAGS) -c Room.cpp

clean:
	rm -f $(OBJS) $(TARGET)
