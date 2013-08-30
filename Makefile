UNAME_S := $(shell uname -s)
CLANG := clang++ -c -Wall -std=c++11 -stdlib=libc++
SOURCES := MsgPack.cpp Core.cpp Socket.cpp SocketManager.cpp
OBJS := $(SOURCES:.cpp=.o)
OUT := out/libNetLink.a

all: build
	rm -f *.o

ifeq ($(UNAME_S),Linux)
build: $(OBJS)
	#Asuming that clang-3.4 and libcxx are installed.
	mkdir -p out
	llvm-ar-3.4 rcs $(OUT) $(OBJS)
endif

ifeq ($(UNAME_S),Darwin)
build: $(OBJS)
	#Asuming that XCode is installed.
	mkdir -p out
	ar rcs $(OUT) $(OBJS)
endif

.cpp.o:
	$(CLANG) $<

clean: 
	rm -f *.o
	rm -rf out/
