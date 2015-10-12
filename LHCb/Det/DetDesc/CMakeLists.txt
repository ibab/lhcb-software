################################################################################
# Package: DetDesc
################################################################################
gaudi_subdir(DetDesc v18r38)

gaudi_depends_on_subdirs(GaudiKernel
                         GaudiUtils
                         Kernel/LHCbMath
                         Kernel/VectorClass)

find_package(ROOT COMPONENTS MathCore GenVector)

gaudi_add_library(DetDescLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS DetDesc
                  INCLUDE_DIRS ROOT
                  LINK_LIBRARIES ROOT GaudiKernel GaudiUtilsLib LHCbMathLib)

gaudi_add_dictionary(DetDesc
                     dict/DetDescDict.h
                     dict/DetDescDict.xml
                     INCLUDE_DIRS ROOT
                     LINK_LIBRARIES ROOT GaudiKernel GaudiUtilsLib LHCbMathLib DetDescLib
                     OPTIONS "-U__MINGW32__")

