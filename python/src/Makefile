all:
	python setup.py build_ext --inplace
	clang++ -pthread -shared -Wl,-z,relro ../../c/lib/wup.o build/temp.linux-x86_64-2.7/wup.o -L/usr/lib64 -lpython2.7 -lstdc++ -lm -O3 -o ./wup.so
	cp ./wup.so ../lib/
	cp ./wup.so ../examples/

