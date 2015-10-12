################################################################################
# Package: L0Base
################################################################################
gaudi_subdir(L0Base v2r4p1)

gaudi_depends_on_subdirs(Event/DAQEvent
                         DAQ/DAQKernel
                         GaudiAlg)

gaudi_add_library(L0Base
                  src/*.cpp
                  PUBLIC_HEADERS L0Base
                  LINK_LIBRARIES DAQEventLib DAQKernelLib GaudiAlgLib)

