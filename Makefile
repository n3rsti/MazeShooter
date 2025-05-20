LIBS=-lGL -lglfw -lGLEW
HEADERS=constants.h lodepng.h myCube.h myTeapot.h shaderprogram.h
FILES=lodepng.cpp main_file.cpp shaderprogram.cpp

# Find system include paths for GLEW and GLFW using pkg-config
# Add local include directory first for LSP to find headers
CPPFLAGS=$(shell pkg-config --cflags glew glfw3) -I.

main_file: $(FILES) $(HEADERS)
	g++ -o main_file $(FILES) $(CPPFLAGS) $(LIBS) -I.

# Generate compile_commands.json for LSP
compile_commands.json: Makefile
	bear -- make

.PHONY: clean
clean:
	rm -f main_file compile_commands.json
