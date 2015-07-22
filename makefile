OPT=-std=c++11 
FILES=Haus/*.cpp
OUT=hausapp

debug:
	g++ $(FILES) $(OPT) -o $(OUT) -D_DEBUG

ship:
	g++ $(FILES) $(OPT) -o $(OUT)

clean:
	rm Haus/*.gch *.out

cleaner: clean
	rm $(OUT)

run: debug
	./$(OUT)

runs: ship
	./$(OUT)
