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

int wisard_classes(const CWisard * const self);

int wisard_ramBits(const CWisard * const self);

int wisard_inputBits(const CWisard * const self);

#ifdef __cplusplus
}
#endif

#endif /* __CWISARD_H */

