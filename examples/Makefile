OBJ = array1.o \
		 	array2.o \
			array3.o \
			list1.o \
			list2.o \
			list3.o \
			map1.o \
			map2.o \
			map3.o \
			stack1.o \
			vector1.o \
			vector2.o \
			vector3.o

all: $(OBJ)

.cc.o:
	g++ -g $< -o $@

clean:
	rm -f *.o

