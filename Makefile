CXX=g++
CXXFLAGS=-std=c++11 
objects=query.o sublist.o

query : $(objects)
	$(CXX) $(CXXFLAGS) $(objects) -o $@

query.o :
sublist.o : sublist.h

.PHONY : clean
clean :
	-rm -f query $(objects)
