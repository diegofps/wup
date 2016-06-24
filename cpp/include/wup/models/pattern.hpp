#ifndef __WUP_PATTERN_HPP
#define __WUP_PATTERN_HPP

namespace wup {

    #ifdef JNI_VERSION_1_1
    class Pattern {
    public:
	    Pattern(int * array) : _env(NULL), _array(NULL), _intArray(array)
	        { }

        Pattern(JNIEnv * env, jintArray & array) : _env(env), _array(&array)
            { _intArray = env->GetIntArrayElements(array, NULL); }
        
        ~Pattern()
        {
		    if (_env != NULL) {
			    _env->ReleaseIntArrayElements(*_array, _intArray, 0);
		    }
        }
        
        int operator[](const int index) const
            { return _intArray[index]; }
        
    private:
        JNIEnv * _env;
        jintArray * _array;
        int * _intArray;
    };

    #else

    class Pattern {
    public:
	    Pattern(const int * const array) : _intArray(array)
	        { }

        int operator[](const int index) const
            { return _intArray[index]; }
        
    private:
        const int * const _intArray;
    };

    #endif

} /* wup */

#endif /* __WUP_PATTERN_HPP */

