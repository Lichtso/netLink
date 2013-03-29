SOURCES := Core.cpp Socket.cpp SocketManager.cpp
OBJS := $(SOURCES:.cpp=.o)
OUT = out/libNetLink.a

ifeq ($CC,"gcc")
	CFLAGS = -std=c++0x
else 
	CFLAGS = -std=c++11 -stdlib=libc++
endif

all: build

build: $(OBJS)
	echo $(CXX)
	mkdir -p out
	ar rcs $(OUT) $(OBJS)
	rm -f *.o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean: 
	rm -f *.o
	rm -rf out/