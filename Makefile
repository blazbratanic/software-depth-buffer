GXX = g++
FLAGS = -std=c++1y -pthread -O3 -DSTANDALONE_APP
INCLUDE = -I ./ -I ./openmesh/src
LIBS = -l OpenMeshCore -l opencv_core -l opencv_highgui

all: label_mesh mesh_interface_test

label_mesh: label_mesh.o 
	$(GXX) $(FLAGS) $(INCLUDE) $(LIB_DIRS) $(LIBS)  label_mesh.o -o label_mesh -pthread

label_mesh.o: label_mesh.cpp 
	$(GXX) $(FLAGS) $(INCLUDE) $(LIB_DIRS) $(LIBS)  -o label_mesh.o -c label_mesh.cpp 

mesh_interface_test: mesh_interface_test.cpp
	$(GXX) $(FLAGS) $(INCLUDE) $(LIB_DIRS) $(LIBS)  -o mesh_interface_test.o -c mesh_interface_test.cpp 


