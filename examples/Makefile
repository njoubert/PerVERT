OBJ = array1.o \
			list1.o \
			map1.o \
			stack1.o \
			vector1.o \
		 	array2.o \
			list2.o \
			map2.o \
			vector2.o \
			array3.o \
			list3.o \
			map3.o \
			vector3.o 

HASKELL = haskell1			

SIZE = 512

all: $(OBJ) $(HASKELL)

testdata: $(OBJ) $(HASKELL)
	mkdir -p testdata
	../pervert ./array1.o $(SIZE)
	cp -R /var/tmp/array1.o testdata/
	../pervert ./list1.o $(SIZE)
	cp -R /var/tmp/list1.o testdata/
	../pervert ./map1.o $(SIZE) 
	cp -R /var/tmp/map1.o testdata/
	../pervert ./stack1.o $(SIZE)
	cp -R /var/tmp/stack1.o testdata/
	../pervert ./vector1.o $(SIZE)
	cp -R /var/tmp/vector1.o testdata/
	../pervert ./array2.o $(SIZE)
	cp -R /var/tmp/array2.o testdata/
	../pervert ./list2.o $(SIZE)
	cp -R /var/tmp/list2.o testdata/
	../pervert ./map2.o $(SIZE)
	cp -R /var/tmp/map2.o testdata/
	../pervert ./vector2.o $(SIZE)
	cp -R /var/tmp/vector2.o testdata/
	../pervert ./array3.o $(SIZE)
	cp -R /var/tmp/array3.o testdata/
	../pervert ./list3.o $(SIZE)
	cp -R /var/tmp/list3.o testdata/
	../pervert ./map3.o $(SIZE)
	cp -R /var/tmp/map3.o testdata/
	../pervert ./vector3.o $(SIZE)
	cp -R /var/tmp/vector3.o testdata/
	../pervert ./haskell1
	cp -r /var/tmp/haskell1 testdata/

.cc.o:
	g++ -g $< -o $@

haskell1: haskell1.hs
	ghc $< 

clean:
	rm -rf testdata *.hi pin.log $(HASKELL) $(OBJ)

