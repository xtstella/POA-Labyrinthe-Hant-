SRC = f1.cpp f2.cpp f3.cpp f4.cpp
OBJ=$(SRC:.cpp=.o)
CC = g++
CXXFLAGS = -O3
EXEC = prog

$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ)

.cc.o:
	$(CC) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJ) *~

depend:
	$(CC) -MM $(SRC) >> Makefile

# a partir d'ici ne pas modifier
