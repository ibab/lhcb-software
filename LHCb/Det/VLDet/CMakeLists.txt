################################################################################
# Package: VLDet
################################################################################
gaudi_subdir(VLDet v1r5)

gaudi_depends_on_subdirs(Det/DetDesc
                         Det/DetDescCnv
                         GaudiAlg
                         Kernel/LHCbKernel
                         Kernel/LHCbMath)

find_package(Boost COMPONENTS regex)
find_package(GSL)

gaudi_add_library(VLDetLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS VLDet
                  INCLUDE_DIRS Boost GSL
                  LINK_LIBRARIES Boost GSL DetDescLib DetDescCnvLib GaudiAlgLib LHCbKernel LHCbMathLib)

gaudi_add_module(VLDet
                 src/component/*.cpp
                 INCLUDE_DIRS Boost GSL
                 LINK_LIBRARIES Boost GSL DetDescLib DetDescCnvLib GaudiAlgLib LHCbKernel LHCbMathLib VLDetLib)

gaudi_add_dictionary(VLDet
                     dict/VLDetDict.h
                     dict/VLDetDict.xml
                     INCLUDE_DIRS Boost GSL
                     LINK_LIBRARIES Boost GSL DetDescLib DetDescCnvLib GaudiAlgLib LHCbKernel LHCbMathLib VLDetLib
                     OPTIONS "-U__MINGW32__")

