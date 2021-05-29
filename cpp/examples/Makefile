all: btest1 btest2 btest3 btest4
	echo "Completed"

t1:
	clang++ -std=c++14 test1.cpp -o test1 -Wall -O3

t2:
	clang++ -std=c++14 test2.cpp -o test2 -Wall -O3

t3:
	clang++ -std=c++14 test3.cpp -o test3 -Wall -O3

t4:
	clang++ -std=c++11 test4.cpp -o test4 -Wall -O3

t5:
	clang++ test5.cpp -o test5 -Wall -std=c++11 -O3 -DWUP_NO_OPENCV -lpthread

dtest1:
	clang++ -std=c++14 test1.cpp -o test1 -Wall -g

dtest2:
	clang++ -std=c++14 test2.cpp -o test2 -Wall -g

dtest3:
	clang++ -std=c++14 test3.cpp -o test3 -Wall -g

dtest4:
	clang++ -std=c++14 test4.cpp -o test4 -Wall -g
	gdb test4

vtest4:
	clang++ -std=c++14 test4.cpp -o test4 -Wall -g
	valgrind --leak-check=full --show-leak-kinds=all ./test4

clean:
	rm -f test1 test2 test3 test4 test5
