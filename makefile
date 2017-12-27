OPT=-std=c++11 
FILES=Haus/*.cpp
OUT=hausapp

debug:
	g++ $(FILES) $(OPT) -o $(OUT) -D_DEBUG

ship:
	g++ $(FILES) $(OPT) -o $(OUT)

all: 	debug ship

clean:
	rm Haus/*.gch *.out

cleaner: clean
	rm $(OUT)

run:
	./$(OUT)

runs:
	./$(OUT)

bar:	debug
	./$(OUT)

bars:	ship
	./$(OUT)
