from libc.stdlib cimport malloc, free

cdef extern from "wup/wup.h" :
    ctypedef struct CWisard:
        pass
    
    void wisard_importFrom(CWisard * const self, const char * const filename)

    void wisard_create(CWisard * const self, const int inputs, const int bits, 
            const int discriminators)

    void wisard_clone(CWisard * const wisard, const CWisard * other)

    void wisard_destroy(CWisard * const self)

    void wisard_learn(CWisard * const self, const int * const pattern, 
            const int target)

    int wisard_readBinary(CWisard * const self, const int * const pattern)

    int wisard_readCounts(CWisard * const self, const int * const pattern)

    int wisard_readBleaching(CWisard * const self, const int * const pattern)

    int * wisard_activations(const CWisard * const self)

    int wisard_numDiscriminators(const CWisard * const self)

    int wisard_numRamBits(const CWisard * const self)

    int wisard_numInputBits(const CWisard * const self)

    void wisard_exportTo(const CWisard * const self, const char * const filename)
    
    int wisard_numRams(const CWisard * const self)



    void graywisard_importFrom(CWisard * const self, const char * const filename)

    void graywisard_create(CWisard * const self, const int inputs, const int bits, 
            const int discriminators)

    void graywisard_clone(CWisard * const wisard, const CWisard * other)

    void graywisard_destroy(CWisard * const self)

    void graywisard_learn(CWisard * const self, const int * const pattern, 
            const int target)

    int graywisard_readBinary(CWisard * const self, const int * const pattern)

    int graywisard_readCounts(CWisard * const self, const int * const pattern)

    int graywisard_readBleaching(CWisard * const self, const int * const pattern)

    int * graywisard_activations(const CWisard * const self)

    int graywisard_numDiscriminators(const CWisard * const self)

    int graywisard_numRamBits(const CWisard * const self)

    int graywisard_numInputBits(const CWisard * const self)

    void graywisard_exportTo(const CWisard * const self, const char * const filename)
    
    int graywisard_numRams(const CWisard * const self)


cdef class Wisard:
    
    cdef int _inputBits
    
    cdef int * _array
    
    cdef CWisard _wisard
    
    def __cinit__(self, inputs, bits=8, classes=2):
        if isinstance(inputs, str):
            wisard_importFrom(&self._wisard, inputs)
            self._inputBits = wisard_numInputBits(&self._wisard)
        else:
            wisard_create(&self._wisard, inputs, bits, classes)
            self._inputBits = inputs
        
        self._array = <int*> malloc(sizeof(int) * self._inputBits)
        if not self._array:
            raise MemoryError()
    
    def __dealloc__(self):
        free(self._array)
        wisard_destroy(&self._wisard)

    def learn(self, pattern, target):
        self._validate_pattern(pattern)
        self._validate_target(target)
        self._copy_pattern(pattern)
        wisard_learn(&self._wisard, self._array, target)

    def read_counts(self, pattern):
        self._validate_pattern(pattern)
        self._copy_pattern(pattern)
        return wisard_readCounts(&self._wisard, self._array)
    
    def read_binary(self, pattern):
        self._validate_pattern(pattern)
        self._copy_pattern(pattern)
        return wisard_readBinary(&self._wisard, self._array)
    
    def read_bleaching(self, pattern):
        self._validate_pattern(pattern)
        self._copy_pattern(pattern)
        return wisard_readBleaching(&self._wisard, self._array)
    
    def num_discriminators(self):
        return wisard_numDiscriminators(&self._wisard)

    def num_ram_bits(self):
        return wisard_numRamBits(&self._wisard)

    def num_rams(self):
        return wisard_numRams(&self._wisard)

    def num_input_bits(self):
        return self._numInputBits
    
    def export_to(self, filename):
        wisard_exportTo(&self._wisard, filename)

    def activations(self):
        cdef const int * activations = wisard_activations(&self._wisard)
        cdef int discriminators = wisard_numDiscriminators(&self._wisard)
        cdef int i = 0;
        
        result = [0] * discriminators
        while i<discriminators:
            result[i] = activations[i]
            i = i + 1
        
        return result
    
    def _validate_pattern(self, pattern):
        if not len(pattern) == self._inputBits:
            raise IndexError("Invalid length on input pattern")
        
    def _validate_target(self, target):
        if target < 0:
            raise IndexError("Target may not be negative")
    
    def _copy_pattern(self, pattern):
        for i, v in enumerate(pattern):
            self._array[i] = v



cdef class GrayWisard:
    
    cdef int _inputBits
    
    cdef int * _array
    
    cdef CWisard _wisard
    
    def __cinit__(self, inputs, bits=8, classes=2):
        if isinstance(inputs, str):
            graywisard_importFrom(&self._wisard, inputs)
            self._inputBits = graywisard_numInputBits(&self._wisard)
        else:
            graywisard_create(&self._wisard, inputs, bits, classes)
            self._inputBits = inputs
        
        self._array = <int*> malloc(sizeof(int) * self._inputBits)
        if not self._array:
            raise MemoryError()
    
    def __dealloc__(self):
        free(self._array)
        graywisard_destroy(&self._wisard)

    def learn(self, pattern, target):
        self._validate_pattern(pattern)
        self._validate_target(target)
        self._copy_pattern(pattern)
        graywisard_learn(&self._wisard, self._array, target)

    def read_counts(self, pattern):
        self._validate_pattern(pattern)
        self._copy_pattern(pattern)
        return graywisard_readCounts(&self._wisard, self._array)
    
    def read_binary(self, pattern):
        self._validate_pattern(pattern)
        self._copy_pattern(pattern)
        return graywisard_readBinary(&self._wisard, self._array)
    
    def read_bleaching(self, pattern):
        self._validate_pattern(pattern)
        self._copy_pattern(pattern)
        return graywisard_readBleaching(&self._wisard, self._array)
    
    def num_discriminators(self):
        return graywisard_numDiscriminators(&self._wisard)

    def num_ram_bits(self):
        return graywisard_numRamBits(&self._wisard)

    def num_rams(self):
        return graywisard_numRams(&self._wisard)

    def num_input_bits(self):
        return self._inputBits
    
    def export_to(self, filename):
        graywisard_exportTo(&self._wisard, filename)

    def activations(self):
        cdef const int * activations = graywisard_activations(&self._wisard)
        cdef int discriminators = graywisard_numDiscriminators(&self._wisard)
        cdef int i = 0;
        
        result = [0] * discriminators
        while i<discriminators:
            result[i] = activations[i]
            i = i + 1
        
        return result

    def _validate_pattern(self, pattern):
        if not len(pattern) == self._inputBits:
            raise IndexError("Invalid length on input pattern")
        
    def _validate_target(self, target):
        if target < 0:
            raise IndexError("Target may not be negative")
    
    def _copy_pattern(self, pattern):
        for i, v in enumerate(pattern):
            self._array[i] = v

