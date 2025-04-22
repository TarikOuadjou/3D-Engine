all :
	g++ -I src/include -L src/lib -o main src/main.cpp src/include/core/screen.cpp -lmingw32 -lSDL2main -lSDL2