#include <wup/wup.hpp>
#include <iostream>

#include "../include/wup/wup.h"

#define SELF  self->__impl
#define OTHER other->__impl

using wup::Wisard;

int wisard_importFrom(CWisard * const self, const char * const filename)
{
    wup::sbreader<int> reader(filename);
    if (reader.good()) {
        SELF = new Wisard(reader);
        return 1;
    } else {
        return 0;
    }
}

void wisard_exportTo(const CWisard * const self, const char * const filename)
{
    wup::sbwriter<int> writer(filename);
    ((Wisard*)SELF)->exportTo( writer );
}

void wisard_create(CWisard * const self, const int inputs, const int bits, 
        const int discriminators)
    { SELF = new Wisard( inputs, bits, discriminators ); }

void wisard_clone(CWisard * const self, const CWisard * const other)
    { SELF = new Wisard( *(Wisard*) OTHER ); }

void wisard_destroy(CWisard * const self)
    { delete (Wisard*) SELF; }

void wisard_learn(CWisard * const self, const int * const pattern, 
        const int target)
    { ((Wisard*)SELF)->learn( pattern, target ); }

int wisard_readBinary(CWisard * const self, const int * const pattern)
    { return ((Wisard*)SELF)->readBinary( pattern ); }

int wisard_readCounts(CWisard * const self, const int * const pattern)
    { return ((Wisard*)SELF)->readCounts( pattern ); }

int wisard_readBleaching(CWisard * const self, const int * const pattern)
    { return ((Wisard*)SELF)->readBleaching( pattern ); }

const int * wisard_activations(const CWisard * const self)
    { return ((Wisard*)SELF)->activations( ); }

int wisard_classes(const CWisard * const self)
    { return ((Wisard*)SELF)->classes( ); }

int wisard_ramBits(const CWisard * const self)
    { return ((Wisard*)SELF)->ramBits( ); }

int wisard_inputBits(const CWisard * const self)
    { return ((Wisard*)SELF)->inputBits( ); }

