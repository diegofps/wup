all:
	cxxtestgen --error-printer -o runner.cpp *.hpp
	clang++ runner.cpp -o runner -Wall -std=c++11 -O3 -lz
	./runner
