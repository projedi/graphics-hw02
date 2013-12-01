CXX=clang++
CFLAGS=-g -Wall -std=c++11
#CFLAGS=-O2 -std=c++11

hw02: main.cpp gl_context.cpp camera.cpp model.cpp controller.cpp
	$(CXX) $(CFLAGS) $^ -o $@ -lGL -lGLEW -lglfw -lfreeimageplus
