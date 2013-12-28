CLANG := clang++ -Wall -std=c++11 -stdlib=libc++
SOURCES := MsgPack.cpp Socket.cpp SocketManager.cpp
OUT := out/libNetLink

all: buildStatic

.cpp.o:
	$(CLANG) -c $<

OBJS_O := $(SOURCES:.cpp=.o)
buildStatic: $(OBJS_O)
	mkdir -p out
	ar rcs $(OUT).a $(OBJS_O)
	ranlib $(OUT).a
	rm -f *.o

%.bc: %.cpp
	$(CLANG) -c -emit-llvm $<

OBJS_BC := $(SOURCES:.cpp=.bc)
buildLLVM: $(OBJS_BC)
	mkdir -p out
	llvm-link-3.4 -o $(OUT).bc $(OBJS_BC)
	rm -f *.bc

examples: buildStatic
	$(CLANG) -o out/udp.o $(OUT).a examples/udp.cpp
	$(CLANG) -o out/tcp.o $(OUT).a examples/tcp.cpp

clean:
	rm -f *.o *.bc
	rm -rf out/
