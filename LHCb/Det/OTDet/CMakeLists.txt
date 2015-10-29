################################################################################
# Package: OTDet
################################################################################
gaudi_subdir(OTDet v8r17)

gaudi_depends_on_subdirs(Det/DetDescCnv
                         Kernel/LHCbKernel
                         Kernel/LHCbMath
                         Kernel/VectorClass)

find_package(Boost)
find_package(VDT)

gaudi_add_library(OTDetLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS OTDet
                  INCLUDE_DIRS Boost VDT
                  LINK_LIBRARIES Boost DetDescCnvLib LHCbKernel LHCbMathLib)

gaudi_add_module(OTDet
                 src/component/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost DetDescCnvLib LHCbKernel LHCbMathLib OTDetLib)

gaudi_add_dictionary(OTDet
                     dict/OTDetDict.h
                     dict/OTDetDict.xml
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost DetDescCnvLib LHCbKernel LHCbMathLib OTDetLib
                     OPTIONS "-U__MINGW32__")

