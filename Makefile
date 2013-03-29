SOURCES := Core.cpp Socket.cpp SocketManager.cpp
INCLUDES := 
OBJS := $(SOURCES:.cpp=.o)
OUT = out/libNetLink.a
CFLAGS = -std=c++11 -stdlib=libc++

all: build

build: $(OBJS)
	mkdir -p out
	ar rcs $(OUT) $(OBJS)
	rm -f *.o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean: 
	rm -f *.o
	rm -rf out/