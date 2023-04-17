OUTPUT_FILE = "sampvoice.so"

COMMON_FLAGS = -m32 -O3 -Ofast -w -fpermissive
COMPILE_FLAGS = $(COMMON_FLAGS) -c -idirafter "include"
PRELINK_FLAGS = $(COMMON_FLAGS) -shared -static-libstdc++

all:
	gcc-9 $(COMPILE_FLAGS) include/pawn/amx/*.h
	g++-9 $(COMPILE_FLAGS) -std=c++11 include/pawn/*.cpp
	g++-9 $(COMPILE_FLAGS) -std=c++11 include/raknet/*.cpp
	g++-9 $(COMPILE_FLAGS) -std=c++17 include/util/*.cpp
	g++-9 $(COMPILE_FLAGS) -std=c++11 include/ysf/*.cpp
	g++-9 $(COMPILE_FLAGS) -std=c++17 *.cpp
	g++-9 $(PRELINK_FLAGS) -o $(OUTPUT_FILE) *.o
	strip -s $(OUTPUT_FILE)
	rm *.o
