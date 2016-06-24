from distutils.core import setup
from Cython.Build import cythonize
from distutils.extension import Extension
from Cython.Distutils import build_ext

setup(
        name = "wup",
        #ext_modules = cythonize("wup.pyx")
        cmdclass = {'build_ext': build_ext},
        ext_modules = [Extension("wup", 
                    sources            = ["wup.pyx", "../../c/src/wup.cpp"], 
                    language           = "c++",
                    extra_compile_args = ["-std=c++14", "-O3"],
                    include_dirs       = ["../../c/include"])
        ]
        
        #ext_modules = cythonize(Extension(
        #    "wup",
        #    ["wup.pyx"],
        #    language="c++",
        #    extra_compile_args=["-std=c++14"],
        #    extra_link_args=["-std=c++14"]
        #))
)

