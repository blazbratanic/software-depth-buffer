GXX = g++
FLAGS = -std=c++1y -pthread -O3 -DSTANDALONE_APP
INCLUDE = -I ./ -I ./openmesh/src
LIBS = -l OpenMeshCore -l opencv_core -l opencv_highgui

label_mesh: label_mesh.o mesh.o
	$(GXX) $(FLAGS) $(INCLUDE) $(LIB_DIRS) $(LIBS)  mesh.o label_mesh.o -o label_mesh -pthread

label_mesh.o: label_mesh.cpp 
	$(GXX) $(FLAGS) $(INCLUDE) $(LIB_DIRS) $(LIBS)  -o label_mesh.o -c label_mesh.cpp 

mesh.o: mesh/mesh.cpp
	$(GXX) $(FLAGS) $(INCLUDE) $(LIB_DIRS) $(LIBS) -o mesh.o -c mesh/mesh.cpp 




interactive: interactive_gl.cpp
	$(GXX) $(FLAGS) $(INCLUDE) $(LIB_DIRS) $(LIBS) -o interactive interactive_gl.cpp
