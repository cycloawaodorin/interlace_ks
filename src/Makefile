CXX = g++
CXXFLAGS = -Wall -Wextra -Wdeprecated-declarations -Wpointer-arith -Wwrite-strings -Wmissing-noreturn -Wno-unused-parameter -Wsuggest-attribute=format -Wsuggest-attribute=noreturn -Wunused-variable -Winit-self -Wshadow -Wlogical-op -Wconversion -Wold-style-cast -c -O3
INCLUDES = 
LD = g++
LDFLAGS = -shared -Wl,--dll,--enable-stdcall-fixup -static
LIBS = -lstdc++ -lgcc -lwinpthread
RM = rm -f

TARGET = interlace_ks.auf
OBJ = interlace_ks.o
DEF = interlace_ks.def

.SUFFIXES: .cpp .o
.SUFFIXES: .c .o

all: $(TARGET)

$(TARGET): $(OBJ) $(DEF)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJ) $(RES) $(DEF) $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $<

interlace_ks.o: filter.h

clean:
	rm -f $(TARGET) $(OBJ)

release:
	make all
	strip $(TARGET)
	cp -f $(TARGET) ../
	make clean
