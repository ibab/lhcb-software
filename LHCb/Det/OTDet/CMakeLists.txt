################################################################################
# Package: OTDet
################################################################################
gaudi_subdir(OTDet v8r8)

gaudi_depends_on_subdirs(Det/DetDescCnv
                         Kernel/LHCbKernel
                         Kernel/LHCbMath)

find_package(Boost)
find_package(GSL)

gaudi_add_library(OTDetLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS OTDet
                  INCLUDE_DIRS Boost GSL
                  LINK_LIBRARIES Boost GSL DetDescCnvLib LHCbKernel LHCbMathLib)

gaudi_add_module(OTDet
                 src/component/*.cpp
                 INCLUDE_DIRS Boost GSL
                 LINK_LIBRARIES Boost GSL DetDescCnvLib LHCbKernel LHCbMathLib OTDetLib)

gaudi_add_dictionary(OTDet
                     dict/OTDetDict.h
                     dict/OTDetDict.xml
                     INCLUDE_DIRS Boost GSL
                     LINK_LIBRARIES Boost GSL DetDescCnvLib LHCbKernel LHCbMathLib OTDetLib
                     OPTIONS "-U__MINGW32__")

