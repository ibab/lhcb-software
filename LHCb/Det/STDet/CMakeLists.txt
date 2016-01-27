################################################################################
# Package: STDet
################################################################################
gaudi_subdir(STDet v5r0)

gaudi_depends_on_subdirs(Det/DetDesc
                         Det/DetDescCnv
                         Kernel/LHCbKernel)

find_package(Boost)

gaudi_add_library(STDetLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS STDet
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost DetDescLib DetDescCnvLib LHCbKernel)

gaudi_add_module(STDet
                 src/Component/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost DetDescLib DetDescCnvLib LHCbKernel STDetLib)

gaudi_add_dictionary(STDet
                     dict/STDetDict.h
                     dict/STDetDict.xml
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost DetDescLib DetDescCnvLib LHCbKernel STDetLib
                     OPTIONS "-U__MINGW32__")

