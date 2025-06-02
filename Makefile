LIBS=-lGL -lglfw -lGLEW
HEADERS=constants.h lodepng.h static/models/myCube.h static/models/myTeapot.h shaderprogram.h src/player/movement.h src/player/camera.h src/maze/maze.h
FILES=lodepng.cpp main_file.cpp shaderprogram.cpp src/player/movement.cpp src/player/camera.cpp src/maze/maze.cpp

# Find system include paths for GLEW and GLFW using pkg-config
# Add local include directory first for LSP to find headers
CPPFLAGS=$(shell pkg-config --cflags glew glfw3) -I.

main_file: $(FILES) $(HEADERS)
	g++ -o main_file $(FILES) $(CPPFLAGS) $(LIBS) -I.

# Generate compile_commands.json for LSP
compile_commands.json: Makefile
	bear -- make $(LIBS)

.PHONY: clean
clean:
	rm -f main_file compile_commands.json
