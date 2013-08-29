UNAME_S := $(shell uname -s)
CLANG := clang++ -c -Wall -std=c++11 -stdlib=libc++
SOURCES := MsgPack.cpp Core.cpp Socket.cpp SocketManager.cpp
OUT := out/libNetLink

all: build

ifeq ($(UNAME_S),Linux)
OBJS := $(SOURCES:.cpp=.bc)
build: $(OBJS)
	#Asuming that clang-3.4 and libcxx are given.
	mkdir -p out
	llvm-link-3.4 -o $(OUT).bc $(OBJS)
	llvm-ar-3.4 rcs $(OUT).a $(OUT).bc
	rm -f *.bc
endif

%.bc: %.cpp
	$(CLANG) -cxx-isystem ../libcxx/include/ -emit-llvm $<

ifeq ($(UNAME_S),Darwin)
OBJS := $(SOURCES:.cpp=.o)
build: $(OBJS)
	#Asuming that only XCode is installed.
	mkdir -p out
	ar rcs $(OUT).a $(OBJS)
	rm -f *.o
endif

.cpp.o:
	$(CLANG) $<

clean: 
	rm -f *.o *.bc
	rm -rf out/
