GXX = g++
FLAGS = -std=c++1y -Werror -pthread -O0 -g
INCLUDE = -I ../ -I ../../third-party/openmesh/src
LIBS = -l OpenMeshCore -l GL -l GLU -l GLEW -l glut

label_mesh: label_mesh.cpp
	$(GXX) $(FLAGS) $(INCLUDE) $(LIB_DIRS) $(LIBS) -o label_mesh label_mesh.cpp

interactive: interactive_gl.cpp
	$(GXX) $(FLAGS) $(INCLUDE) $(LIB_DIRS) $(LIBS) -o interactive interactive_gl.cpp
