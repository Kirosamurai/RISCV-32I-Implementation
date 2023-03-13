INCDIR = ../include
CXX = g++
CXXFLAGS = -I$(INC)
OBJDIR = obj

_DEP = RISCV.h
DEP = $(patsubset %,$(INCDIR)/%,$(_DEP))

_OBJ = main.o RISCV.o
OBJ = $(patsubset %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: %.cpp $(DEP)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

all: $(OBJ)
	$(CXX) -o ../bin/RISCV $^ $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ *.bak ../bin/RISCV
