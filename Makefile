all:
	cd src && make && cd ..
	cd example && make && cd ..

clean:
	cd src && make clean && cd ..
	cd example && make clean && cd ..

