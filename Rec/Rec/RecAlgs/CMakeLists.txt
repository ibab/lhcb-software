################################################################################
# Package: RecAlgs
################################################################################
gaudi_subdir(RecAlgs v3r4)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/EventBase
                         Event/RecEvent
                         GaudiAlg
                         GaudiUtils
                         Kernel/LHCbKernel
                         Rich/RichKernel)

find_package(AIDA)

gaudi_add_module(RecAlgs
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/EventBase
                 LINK_LIBRARIES DAQEventLib RecEvent GaudiAlgLib GaudiUtilsLib LHCbKernel RichKernelLib)

