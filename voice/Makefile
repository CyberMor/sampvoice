OUTPUT_FILE = "../binaries/sampvoice.out"

COMMON_FLAGS = -D _GLIBCXX_USE_CXX11_ABI=0 -m32 -O3 -Ofast -fPIC -w -DNDEBUG
COMPILE_FLAGS = $(COMMON_FLAGS) -c -idirafter "../common"
PRELINK_FLAGS = $(COMMON_FLAGS) -static-libgcc -static-libstdc++

all:
	mkdir -p ../binaries
	g++ $(COMPILE_FLAGS) -std=c++17 *.cpp
	g++ $(PRELINK_FLAGS) -o $(OUTPUT_FILE) *.o -lpthread
	strip -s $(OUTPUT_FILE)
	rm *.o