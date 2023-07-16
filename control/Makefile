OUTPUT_FILE = "../binaries/sampvoice.so"

COMMON_FLAGS = -D _GLIBCXX_USE_CXX11_ABI=0 -m32 -O3 -Ofast -fPIC -w -DNDEBUG -fpermissive
COMPILE_FLAGS = $(COMMON_FLAGS) -c -idirafter "../common" -idirafter "libraries"
PRELINK_FLAGS = $(COMMON_FLAGS) -shared -static-libgcc -static-libstdc++

all:
	mkdir -p ../binaries
	gcc $(COMPILE_FLAGS) libraries/pawn/amx/*.h
	g++ $(COMPILE_FLAGS) -std=c++11 libraries/pawn/*.cpp
	g++ $(COMPILE_FLAGS) -std=c++11 libraries/raknet/*.cpp
	g++ $(COMPILE_FLAGS) -std=c++11 libraries/ysf/*.cpp
	g++ $(COMPILE_FLAGS) -std=c++17 *.cpp
	g++ $(PRELINK_FLAGS) -o $(OUTPUT_FILE) *.o
	strip -s $(OUTPUT_FILE)
	rm *.o