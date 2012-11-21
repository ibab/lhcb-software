################################################################################
# Package: RichMCMonitors
################################################################################
gaudi_subdir(RichMCMonitors v1r9)

gaudi_depends_on_subdirs(Event/MCEvent
                         Kernel/MCInterfaces
                         Rich/RichKernel)

find_package(GSL)

gaudi_add_module(RichMCMonitors
                 src/*.cpp
                 INCLUDE_DIRS GSL Kernel/MCInterfaces
                 LINK_LIBRARIES GSL MCEvent RichKernelLib)

