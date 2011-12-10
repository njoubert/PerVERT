all:
	make -C backend
	make -C pintool/source/tools/Pervert

clean:
	make -C backend clean
	make -C pintool/source/tools/Pervert clean
