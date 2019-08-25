TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
DEFINES += "WUP_NO_MPICH"

INCLUDEPATH += ./cpp/include ./c/include

SOURCES += \
    c/examples/test1.c \
    c/src/wup.cpp \
    cpp/examples/test1.cpp \
    cpp/examples/test2.cpp \
    cpp/examples/test3.cpp \
    cpp/examples/test4.cpp

DISTFILES += \
    README.md \
    cpp/examples/README

HEADERS += \
    c/include/wup/cwisard.h \
    c/include/wup/wup.h \
    cpp/include/wup/common/bundle.hpp \
    cpp/include/wup/common/bundle3d.hpp \
    cpp/include/wup/common/clock.hpp \
    cpp/include/wup/common/confusionmatrix.hpp \
    cpp/include/wup/common/cronus.hpp \
    cpp/include/wup/common/dataset.hpp \
    cpp/include/wup/common/exceptions.hpp \
    cpp/include/wup/common/generic.hpp \
    cpp/include/wup/common/iterators.hpp \
    cpp/include/wup/common/kfold.hpp \
    cpp/include/wup/common/mpichutils.hpp \
    cpp/include/wup/common/msgs.hpp \
    cpp/include/wup/common/opencvutils.hpp \
    cpp/include/wup/common/params.hpp \
    cpp/include/wup/common/random.hpp \
    cpp/include/wup/common/ranges.hpp \
    cpp/include/wup/common/ref_vector.hpp \
    cpp/include/wup/common/sbio.hpp \
    cpp/include/wup/common/seq.hpp \
    cpp/include/wup/common/threads.hpp \
    cpp/include/wup/common/topk.hpp \
    cpp/include/wup/models/decoders/basedecoder.hpp \
    cpp/include/wup/models/decoders/basendecoder.hpp \
    cpp/include/wup/models/decoders/binarydecoder.hpp \
    cpp/include/wup/models/decoders/graydecoder.hpp \
    cpp/include/wup/models/decoders/intdecoder.hpp \
    cpp/include/wup/models/decoders/snakedecoder.hpp \
    cpp/include/wup/models/kernelcanvas.hpp \
    cpp/include/wup/models/kernelgens.hpp \
    cpp/include/wup/models/markovlocalization.hpp \
    cpp/include/wup/models/oldkernelcanvas.hpp \
    cpp/include/wup/models/pattern.hpp \
    cpp/include/wup/models/wisard.hpp \
    cpp/include/wup/nodes/all.hpp \
    cpp/include/wup/nodes/box2d.hpp \
    cpp/include/wup/nodes/delta.hpp \
    cpp/include/wup/nodes/direction.hpp \
    cpp/include/wup/nodes/export.hpp \
    cpp/include/wup/nodes/kernelcanvas.hpp \
    cpp/include/wup/nodes/kernelwisard.hpp \
    cpp/include/wup/nodes/multikernelcanvas.hpp \
    cpp/include/wup/nodes/node.hpp \
    cpp/include/wup/nodes/replicate.hpp \
    cpp/include/wup/nodes/rotate.hpp \
    cpp/include/wup/nodes/shortMemory.hpp \
    cpp/include/wup/nodes/show.hpp \
    cpp/include/wup/nodes/shuffler.hpp \
    cpp/include/wup/nodes/smooth4.hpp \
    cpp/include/wup/nodes/steps.hpp \
    cpp/include/wup/nodes/streamencoder.hpp \
    cpp/include/wup/nodes/tanh.hpp \
    cpp/include/wup/nodes/unary.hpp \
    cpp/include/wup/nodes/zscore.hpp \
    cpp/include/wup/wup.hpp \
    cpp/tests/test_generic.hpp \
    cpp/tests/test_opencvutils.hpp \
    cpp/tests/test_sbio_intreader_and_intwriter.hpp \
    cpp/tests/test_zip.hpp
