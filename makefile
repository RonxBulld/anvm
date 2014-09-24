SRC=$(wildcard src/*.cpp)
HEAD=$(wildcard src/*.h)
OBJ=$(SRC:src/%.cpp=obj/%.o)
TARGET=anvm
CXXFLAG=-g -I./libiconv/include
LDFLAG=-g -lm -lSDL2 -lSDL2_ttf -L./libiconv/lib -liconv -shared
$(TARGET): obj $(OBJ)
	$(CXX) $(LDFLAG) -o $(TARGET) $(OBJ)
obj/%.o: src/%.cpp $(HEAD)
	$(CXX) $(CXXFLAG) $< -c -o $@
obj:
	mkdir $@
clean:
	rm -f $(OBJ) $(TARGET)