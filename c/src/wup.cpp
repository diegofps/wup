#include <wup/wup.hpp>
#include <iostream>

#include "../include/wup/wup.h"

#define SELF  self->__impl
#define OTHER other->__impl

using wup::Wisard;
using wup::BinaryInput;
using wup::FactorialInput;

typedef Wisard<BinaryInput> BWisard;
typedef Wisard<FactorialInput> FWisard;

int wisard_importFrom(CWisard * const self, const char * const filename)
{
    wup::sbreader<int> reader(filename);
    if (reader.good()) {
        SELF = new BWisard(reader);
        return 1;
    } else {
        return 0;
    }
}

void wisard_exportTo(const CWisard * const self, const char * const filename)
{
    wup::sbwriter<int> writer(filename);
    ((BWisard*)SELF)->exportTo( writer );
}

void wisard_create(CWisard * const self, const int inputs, const int bits, 
        const int discriminators)
    { SELF = new BWisard( inputs, bits, discriminators ); }

void wisard_clone(CWisard * const self, const CWisard * const other)
    { SELF = new BWisard( *(BWisard*) OTHER ); }

void wisard_destroy(CWisard * const self)
    { delete (BWisard*) SELF; }

void wisard_learn(CWisard * const self, const int * const pattern, 
        const int target)
    { ((BWisard*)SELF)->learn( pattern, target ); }

int wisard_readBinary(CWisard * const self, const int * const pattern)
    { return ((BWisard*)SELF)->readBinary( pattern ); }

int wisard_readCounts(CWisard * const self, const int * const pattern)
    { return ((BWisard*)SELF)->readCounts( pattern ); }

int wisard_readBleaching(CWisard * const self, const int * const pattern)
    { return ((BWisard*)SELF)->readBleaching( pattern ); }

const int * wisard_activations(const CWisard * const self)
    { return ((BWisard*)SELF)->activations( ); }

int wisard_classes(const CWisard * const self)
    { return ((BWisard*)SELF)->classes( ); }

int wisard_ramBits(const CWisard * const self)
    { return ((BWisard*)SELF)->ramBits( ); }

int wisard_inputBits(const CWisard * const self)
    { return ((BWisard*)SELF)->inputBits( ); }




int fwisard_importFrom(CWisard * const self, const char * const filename)
{
    wup::sbreader<int> reader(filename);
    if (reader.good()) {
        SELF = new FWisard(reader);
        return 1;
    } else {
        return 0;
    }
}

void fwisard_exportTo(const CWisard * const self, const char * const filename)
{
    wup::sbwriter<int> writer(filename);
    ((FWisard*)SELF)->exportTo( writer );
}

void fwisard_create(CWisard * const self, const int inputs, const int bits, 
        const int discriminators)
    { SELF = new FWisard( inputs, bits, discriminators ); }

void fwisard_clone(CWisard * const self, const CWisard * const other)
    { SELF = new FWisard( *(FWisard*) OTHER ); }

void fwisard_destroy(CWisard * const self)
    { delete (FWisard*) SELF; }

void fwisard_learn(CWisard * const self, const int * const pattern, 
        const int target)
    { ((FWisard*)SELF)->learn( pattern, target ); }

int fwisard_readBinary(CWisard * const self, const int * const pattern)
    { return ((FWisard*)SELF)->readBinary( pattern ); }

int fwisard_readCounts(CWisard * const self, const int * const pattern)
    { return ((FWisard*)SELF)->readCounts( pattern ); }

int fwisard_readBleaching(CWisard * const self, const int * const pattern)
    { return ((FWisard*)SELF)->readBleaching( pattern ); }

const int * fwisard_activations(const CWisard * const self)
    { return ((FWisard*)SELF)->activations( ); }

int fwisard_classes(const CWisard * const self)
    { return ((FWisard*)SELF)->classes( ); }

int fwisard_ramBits(const CWisard * const self)
    { return ((FWisard*)SELF)->ramBits( ); }

int fwisard_inputBits(const CWisard * const self)
    { return ((FWisard*)SELF)->inputBits( ); }

