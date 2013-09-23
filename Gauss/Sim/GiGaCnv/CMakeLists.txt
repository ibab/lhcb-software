################################################################################
# Package: GiGaCnv
################################################################################
gaudi_subdir(GiGaCnv v20r2)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/GenEvent
                         Event/MCEvent
                         Sim/GiGa
                         Tools/ClhepTools)

gaudi_add_library(GiGaCnvLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS GiGaCnv
                  INCLUDE_DIRS Tools/ClhepTools
                  LINK_LIBRARIES DetDescLib GenEvent MCEvent GiGaLib)

gaudi_add_module(GiGaCnv
                 src/component/*.cpp
                 INCLUDE_DIRS Tools/ClhepTools
                 LINK_LIBRARIES DetDescLib GenEvent MCEvent GiGaLib GiGaCnvLib)

