all: btest1 btest2 btest3

btest1:
	clang++ -std=c++14 test1.cpp -o test1 -Wall -O3

btest2:
	clang++ -std=c++14 test2.cpp -o test2 -Wall -O3

btest3:
	clang++ -std=c++14 test3.cpp -o test3 -Wall -O3

debug: dtest1 dtest2 dtest3

dtest1:
	clang++ -std=c++14 test1.cpp -o test1 -Wall -g

dtest2:
	clang++ -std=c++14 test2.cpp -o test2 -Wall -g

dtest3:
	clang++ -std=c++14 test3.cpp -o test3 -Wall -g


clean:
	rm test1

