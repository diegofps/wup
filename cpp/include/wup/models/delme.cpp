#include <iostream>
#include <wup/wup.hpp>

using namespace wup;

int main(int argc, char **argv) {
    int pattern[] = {70, 120, 255, 200};
    int indexes[] = {0, 1, 2, 3};
    
    FactorialInput fac(indexes, 4);

    fac.read(Pattern(pattern));
    
    print();
    
    print(fac.hash());

    return 0;
}

