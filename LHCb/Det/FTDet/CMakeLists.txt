################################################################################
# Package: FTDet
################################################################################
gaudi_subdir(FTDet v2r11)

gaudi_depends_on_subdirs(Det/DetDescCnv
                         Event/FTEvent
                         Event/MCEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel
                         Kernel/LHCbMath)

find_package(Boost)
find_package(GSL)

gaudi_add_library(FTDetLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS FTDet
                  INCLUDE_DIRS Boost GSL Event/FTEvent
                  LINK_LIBRARIES Boost GSL DetDescCnvLib MCEvent GaudiAlgLib GaudiKernel LHCbKernel LHCbMathLib)

gaudi_add_module(FTDet
                 src/component/*.cpp
                 INCLUDE_DIRS Boost GSL Event/FTEvent
                 LINK_LIBRARIES Boost GSL DetDescCnvLib MCEvent GaudiAlgLib GaudiKernel LHCbKernel LHCbMathLib FTDetLib)

gaudi_add_dictionary(FTDet
                     dict/FTDetDict.h
                     dict/FTDetDict.xml
                     INCLUDE_DIRS Boost GSL Event/FTEvent
                     LINK_LIBRARIES Boost GSL DetDescCnvLib MCEvent GaudiAlgLib GaudiKernel LHCbKernel LHCbMathLib FTDetLib
                     OPTIONS "-U__MINGW32__")

