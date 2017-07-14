SRC = f1.cpp f2.cpp f3.cpp f4.cpp
OBJ=$(SRC:.cpp=.o)
CC = g++
CXXFLAGS = -O3
EXEC = prog

$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ)

.cpp.o:
	$(CC) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJ) *~

depend:
	$(CC) -MM $(SRC) >> Makefile

# a partir d'ici ne pas modifier
f1.o: f1.cpp truc.h machin.h
f2.o: f2.cpp truc.h
f3.o: f3.cpp bidule.h
f4.o: f4.cpp truc.h
