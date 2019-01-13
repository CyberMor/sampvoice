OUTFILE = "sampvoice.so"
BASS_PATH = $(shell pwd)

# base compile flags
COMPILE_FLAGS=-m32 -fpack-struct=1 -fpermissive -c -O3 -w -idirafter "sdk"

# base link flags
LINK_FLAGS = -m32 -fpack-struct=1 -fpermissive -shared -O3 -static-libstdc++

# bass link flags
LINK_FLAGS += -L$(BASS_PATH) -lbass -lm -Wl,-rpath,$(BASS_PATH)

all:
	gcc $(COMPILE_FLAGS) sdk/pawn/amx/*.h
	g++ $(COMPILE_FLAGS) -std=c++11 sdk/pawn/*.cpp
	g++ $(COMPILE_FLAGS) -std=c++11 sdk/raknet/*.cpp
	g++ $(COMPILE_FLAGS) -std=c++14 *.cpp
	g++ $(LINK_FLAGS) -o $(OUTFILE) *.o
	rm *.o
	strip -s $(OUTFILE)
