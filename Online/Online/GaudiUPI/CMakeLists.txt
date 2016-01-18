################################################################################
# Package: GaudiUPI
################################################################################
gaudi_subdir(GaudiUPI v1r21)

gaudi_depends_on_subdirs(GaudiKernel
                         Online/UPI
                         Online/DIM)

gaudi_add_library(GaudiUPI src/*.cpp
                  PUBLIC_HEADERS GaudiUPI
                  LINK_LIBRARIES GaudiKernel dim UPI)

gaudi_add_module(GaudiUPIComp components/*.cpp
                 LINK_LIBRARIES GaudiUPI)
