#ifndef __CWISARD_H
#define __CWISARD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { void *__impl; } CWisard;

int wisard_importFrom(CWisard * const self, const char * const filename);

void wisard_exportTo(const CWisard * const self, const char * const filename);

void wisard_create(CWisard * const self, const int inputs, const int bits, 
        const int discriminators);

void wisard_clone(CWisard * const wisard, const CWisard * other);

void wisard_destroy(CWisard * const self);

void wisard_learn(CWisard * const self, const int * const pattern, 
        const int target);

int wisard_readBinary(CWisard * const self, const int * const pattern);

int wisard_readCounts(CWisard * const self, const int * const pattern);

int wisard_readBleaching(CWisard * const self, const int * const pattern);

const int * wisard_activations(const CWisard * const self);

int wisard_numDiscriminators(const CWisard * const self);

int wisard_numRamBits(const CWisard * const self);

int wisard_numRams(const CWisard * const self);

int wisard_numInputBits(const CWisard * const self);



int graywisard_importFrom(CWisard * const self, const char * const filename);

void graywisard_exportTo(const CWisard * const self, const char * const filename);

void graywisard_create(CWisard * const self, const int inputs, const int bits, 
        const int discriminators);
        
void graywisard_clone(CWisard * const self, const CWisard * const other);

void graywisard_destroy(CWisard * const self);

void graywisard_learn(CWisard * const self, const int * const pattern, 
        const int target);

int graywisard_readBinary(CWisard * const self, const int * const pattern);

int graywisard_readCounts(CWisard * const self, const int * const pattern);

int graywisard_readBleaching(CWisard * const self, const int * const pattern);

const int * graywisard_activations(const CWisard * const self);

int graywisard_numDiscriminators(const CWisard * const self);

int graywisard_numRamBits(const CWisard * const self);

int graywisard_numRams(const CWisard * const self);

int graywisard_numInputBits(const CWisard * const self);

#ifdef __cplusplus
}
#endif

#endif /* __CWISARD_H */

