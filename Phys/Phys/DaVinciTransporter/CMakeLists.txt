################################################################################
# Package: DaVinciTransporter
################################################################################
gaudi_subdir(DaVinciTransporter v3r13)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Tr/TrackInterfaces)

gaudi_add_module(DaVinciTransporter
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES DaVinciKernelLib)

