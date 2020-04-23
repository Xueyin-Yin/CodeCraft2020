test: main.cpp
	g++ -O3 main.cpp -o test -lpthread

clean:
	rm -rf test
