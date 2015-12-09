#include <wup/wup.hpp>

using namespace wup;

int main() {
    try {
        Dataset ds("../../datasets/uji2");
        
        for (auto &sample : ds)
            print(  "ID:",sample.id(), 
                    ", Target:", sample.target(),
                    ", Group:", sample.group(),
                    ", Subtarget:", sample.subtarget(),
                    ", Size:", sample.size() );
        
        for (auto &feature : ds[0]) {
            for (int i=0;i<feature.size();++i)
                printn(feature[i], " ");
            print();
        }
        
    } catch(wup::WUPException e) {
        print("Problema:", e.what());
    }
    
    return 0;
}

