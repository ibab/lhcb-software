################################################################################
# Package: VPDet
################################################################################
gaudi_subdir(VPDet v3r4)

gaudi_depends_on_subdirs(Det/DetDesc
                         Det/DetDescCnv
                         GaudiAlg
                         Kernel/LHCbKernel
                         Kernel/LHCbMath)

find_package(Boost)

gaudi_add_library(VPDetLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS VPDet
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost DetDescLib DetDescCnvLib GaudiAlgLib LHCbKernel LHCbMathLib)

gaudi_add_module(VPDet
                 src/component/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost DetDescLib DetDescCnvLib GaudiAlgLib LHCbKernel LHCbMathLib VPDetLib)

gaudi_add_dictionary(VPDet
                     dict/VPDetDict.h
                     dict/VPDetDict.xml
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost DetDescLib DetDescCnvLib GaudiAlgLib LHCbKernel LHCbMathLib VPDetLib
                     OPTIONS "-U__MINGW32__")

