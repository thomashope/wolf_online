CXX := clang++
CXXFLAGS := $(shell sdl2-config --cflags) -std=c++11 -O3
LDFLAGS := $(shell sdl2-config --libs) -lsdl2_net
SERVER_SRCS := $(wildcard ./server/*.cpp)
CLIENT_SRCS := $(wildcard ./client/*.cpp)
SHARED_SRCS := $(wildcard ./shared/*.cpp)

CLIENT_EXE := wolf_client
SERVER_EXE := wolf_server

.PHONY: all clean

all: $(CLIENT_EXE) $(SERVER_EXE)

$(CLIENT_EXE): $(CLIENT_SRCS) $(SHARED_SRCS)
	$(CXX) $(CLIENT_SRCS) $(SHARED_SRCS) $(CXXFLAGS) $(LDFLAGS) -o $(CLIENT_EXE)

$(SERVER_EXE): $(SERVER_SRCS) $(SHARED_SRCS)
	$(CXX) $(SERVER_SRCS) $(SHARED_SRCS) $(CXXFLAGS) $(LDFLAGS) -o $(SERVER_EXE)

clean:
	rm $(CLIENT_EXE)
	rm $(SERVER_EXE)