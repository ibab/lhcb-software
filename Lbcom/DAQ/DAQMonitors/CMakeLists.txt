################################################################################
# Package: DAQMonitors
################################################################################
gaudi_subdir(DAQMonitors v1r9p1)

gaudi_depends_on_subdirs(Event/DAQEvent
                         GaudiAlg
                         Kernel/LHCbKernel)

find_package(AIDA)
find_package(ROOT COMPONENTS Hist Graf RIO)

gaudi_add_module(DAQMonitors
                 src/*.cpp
                 INCLUDE_DIRS ROOT AIDA
                 LINK_LIBRARIES ROOT DAQEventLib GaudiAlgLib LHCbKernel)

