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

    int wisard_classes(const CWisard * const self)

    int wisard_ramBits(const CWisard * const self)

    int wisard_inputBits(const CWisard * const self)

    void wisard_exportTo(const CWisard * const self, const char * const filename)

cdef class Wisard:
    
    cdef int _inputBits
    
    cdef int * _array
    
    cdef CWisard _wisard
    
    def __cinit__(self, inputs, bits=8, classes=2):
        if isinstance(inputs, str):
            wisard_importFrom(&self._wisard, inputs)
            self._inputBits = wisard_inputBits(&self._wisard)
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
    
    def classes(self):
        return wisard_classes(&self._wisard)

    def ram_bits(self):
        return wisard_ramBits(&self._wisard)

    def input_bits(self):
        return self._inputBits
    
    def export_to(self, filename):
        wisard_exportTo(&self._wisard, filename)

    def activations(self):
        cdef const int * activations = wisard_activations(&self._wisard)
        cdef int discriminators = wisard_classes(&self._wisard)
        cdef int i = 0;
        
        result = [0] * discriminators
        while i<discriminators:
            result[i] = activations[i]
            i = i + 1
        
        return result
    
    #void wisard_clone(CWisard * const wisard, const CWisard * other)

    def _validate_pattern(self, pattern):
        if not len(pattern) == self._inputBits:
            raise IndexError("Invalid length on input pattern")
        
    def _validate_target(self, target):
        if target < 0:
            raise IndexError("Target may not be negative")
    
    def _copy_pattern(self, pattern):
        for i, v in enumerate(pattern):
            self._array[i] = v


