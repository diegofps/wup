all:
	cxxtestgen --error-printer -o runner.cpp *.hpp
	clang++ runner.cpp -o runner -Wall -std=c++11 -O3 -lz -lpthread `pkg-config opencv --libs` -I../include -DWUP_NO_MPICH -DWUP_UNSAFE
	./runner
