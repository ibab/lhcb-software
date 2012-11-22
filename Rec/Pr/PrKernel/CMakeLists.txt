################################################################################
# Package: PrKernel
################################################################################
gaudi_subdir(PrKernel v1r0)

gaudi_depends_on_subdirs(Event/RecEvent
                         GaudiAlg)

gaudi_add_library(PrKernel
                  src/*.cpp
                  PUBLIC_HEADERS PrKernel
                  LINK_LIBRARIES RecEvent GaudiAlgLib)

