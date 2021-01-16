#ifndef INCLUDE_WUP_HPP
#define INCLUDE_WUP_HPP

#define UNUSED(x) (void)(x)

#define _STR(name) #name
#define STR(macro) _STR(macro)

#define WUP_STATICS namespace wup { std::string logPrefix; };

#include <wup/common/exceptions.hpp>
#include <wup/common/msgs.hpp>
#include <wup/common/generic.hpp>
#include <wup/common/dataset.hpp>
#include <wup/common/ranges.hpp>
#include <wup/common/clock.hpp>
#include <wup/common/params.hpp>
#include <wup/common/topk.hpp>
#include <wup/common/kfold.hpp>
#include <wup/common/bundle.hpp>
#include <wup/common/bundle3d.hpp>
#include <wup/common/confusionmatrix.hpp>
#include <wup/common/iterators.hpp>
#include <wup/common/sbio.hpp>
#include <wup/common/seq.hpp>
#include <wup/common/random.hpp>
#include <wup/common/cronus.hpp>
#include <wup/common/threads.hpp>

#ifndef WUP_NO_ZIP
#include <wup/common/zip.hpp>
#endif

#ifndef WUP_NO_OPENCV
#include <wup/common/opencvutils.hpp>
#endif

#ifndef WUP_NO_MPICH
#include <wup/common/mpichutils.hpp>
#endif

#include <wup/models/wisard.hpp>
#include <wup/models/kernelcanvas.hpp>
#include <wup/models/markovlocalization.hpp>
#include <wup/models/pattern.hpp>

#include <wup/nodes/all.hpp>
#include <wup/nodes/streamencoder.hpp>

#include <wup/third_party/json.hpp>

#endif /* INCLUDE_WUP_HPP */
