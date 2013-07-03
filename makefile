objs = read_dxf.o entities.o entities2elements.o tables.o tables2svg_info.o blocks.o
flags = -g -O3 -Wall

dxf2svg : dxf2svg.cpp $(objs)
	clang++ $(flags) -o dxf2svg dxf2svg.cpp $(objs)

test : test_dxf.cpp $(objs)
	clang++ $(flags) -o test test_dxf.cpp $(objs)
	
%.o : %.cpp %.h 
	clang++ $(flags) -c $<
	
clean :
	rm -f dxf2svg $(objs)
	
