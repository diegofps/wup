#include <wup/wup.hpp>
#include <iostream>

#include "../include/wup/wup.h"

#define SELF  self->__impl
#define OTHER other->__impl

using wup::Wisard;
using wup::GrayWisard;

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
{
	SELF = new Wisard( inputs, bits, discriminators );
}

void wisard_clone(CWisard * const self, const CWisard * const other)
{
	SELF = new Wisard( *(Wisard*) OTHER );
}

void wisard_destroy(CWisard * const self)
{
	delete (Wisard*) SELF;
}

void wisard_learn(CWisard * const self, const int * const pattern, 
        const int target)
{
	((Wisard*)SELF)->learn( pattern, target );
}

int wisard_readBinary(CWisard * const self, const int * const pattern)
{
	return ((Wisard*)SELF)->readBinary( pattern );
}

int wisard_readCounts(CWisard * const self, const int * const pattern)
{
	return ((Wisard*)SELF)->readCounts( pattern );
}

int wisard_readBleaching(CWisard * const self, const int * const pattern)
{
	return ((Wisard*)SELF)->readBleaching( pattern );
}

const int * wisard_activations(const CWisard * const self)
{
	return ((Wisard*)SELF)->activations( );
}

int wisard_numDiscriminators(const CWisard * const self)
{
	return ((Wisard*)SELF)->numDiscriminators( );
}

int wisard_numRamBits(const CWisard * const self)
{
	return ((Wisard*)SELF)->numRamBits( );
}

int wisard_numInputBits(const CWisard * const self)
{
	return ((Wisard*)SELF)->numInputBits( );
}

int wisard_numRams(const CWisard * const self)
{
	return ((Wisard*)SELF)->numRams( );
}




int graywisard_importFrom(CWisard * const self, const char * const filename)
{
    wup::sbreader<int> reader(filename);
    if (reader.good()) {
        SELF = new GrayWisard(reader);
        return 1;
    } else {
        return 0;
    }
}

void graywisard_exportTo(const CWisard * const self, const char * const filename)
{
    wup::sbwriter<int> writer(filename);
    ((GrayWisard*)SELF)->exportTo( writer );
}

void graywisard_create(CWisard * const self, const int inputs, const int bits, 
        const int discriminators)
{
	SELF = new GrayWisard( inputs, bits, discriminators );
}

void graywisard_clone(CWisard * const self, const CWisard * const other)
{
	SELF = new GrayWisard( *(GrayWisard*) OTHER );
}

void graywisard_destroy(CWisard * const self)
{
	delete (GrayWisard*) SELF;
}

void graywisard_learn(CWisard * const self, const int * const pattern, 
        const int target)
{
	((GrayWisard*)SELF)->learn( pattern, target );
}

int graywisard_readBinary(CWisard * const self, const int * const pattern)
{
	return ((GrayWisard*)SELF)->readBinary( pattern );
}

int graywisard_readCounts(CWisard * const self, const int * const pattern)
{
	return ((GrayWisard*)SELF)->readCounts( pattern );
}

int graywisard_readBleaching(CWisard * const self, const int * const pattern)
{
	return ((GrayWisard*)SELF)->readBleaching( pattern );
}

const int * graywisard_activations(const CWisard * const self)
{
	return ((GrayWisard*)SELF)->activations( );
}

int graywisard_numDiscriminators(const CWisard * const self)
{
	return ((GrayWisard*)SELF)->numDiscriminators( );
}

int graywisard_numRamBits(const CWisard * const self)
{
	return ((GrayWisard*)SELF)->numRamBits( );
}

int graywisard_numInputBits(const CWisard * const self)
{
	return ((GrayWisard*)SELF)->numInputBits( );
}

int graywisard_numRams(const CWisard * const self)
{
	return ((GrayWisard*)SELF)->numRams( );
}

