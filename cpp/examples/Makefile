CC=clang++
PARAMS=-std=c++11 -Wall
LIBS=-lpthread -I/usr/include/opencv4/ -DWUP_NO_MPICH

all: btest1 btest2 btest3 btest4
	echo "Completed"

t1:
	$(CC) test1.cpp -o test1 $(PARAMS) $(LIBS) -O3

t2:
	$(CC) test2.cpp -o test2 $(PARAMS) $(LIBS) -O3

t3:
	$(CC) test3.cpp -o test3 $(PARAMS) $(LIBS) -O3

t4:
	$(CC) test4.cpp -o test4 $(PARAMS) $(LIBS) -O3

t5:
	$(CC) test5.cpp -o test5 $(PARAMS) $(LIBS) -O3 -DWUP_NO_OPENCV -lpthread


d1:
	$(CC) test1.cpp -o test1 $(PARAMS) $(LIBS) -g

d2:
	$(CC) test2.cpp -o test2 $(PARAMS) $(LIBS) -g

d3:
	$(CC) test3.cpp -o test3 $(PARAMS) $(LIBS) -g

d4:
	$(CC) test4.cpp -o test4 $(PARAMS) $(LIBS) -g
	gdb test4


v4:
	clang++ test4.cpp -o test4 $(PARAMS) $(LIBS) -g
	valgrind --leak-check=full --show-leak-kinds=all ./test4


clean:
	rm -f test1 test2 test3 test4 test5
