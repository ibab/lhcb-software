################################################################################
# Package: FastVelo
################################################################################
gaudi_subdir(FastVelo v1r24)

gaudi_depends_on_subdirs(Det/STDet
                         Det/VeloDet
                         Event/DigiEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         ST/STKernel
                         Tr/TrackInterfaces
                         Tf/PatKernel)

gaudi_add_module(FastVelo
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Tf/PatKernel, Tr/TrackInterfaces
                 LINK_LIBRARIES STDetLib VeloDetLib RecEvent TrackEvent GaudiAlgLib STKernelLib)

gaudi_install_python_modules()
