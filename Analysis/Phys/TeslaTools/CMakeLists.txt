################################################################################
# Package: TeslaTools
################################################################################
gaudi_subdir(TeslaTools v1r0)

gaudi_depends_on_subdirs(Event/MicroDst
                         Kernel/LHCbAlgs
                         Event/RecEvent
                         Phys/DaVinciKernel
                         Phys/DaVinciInterfaces
                         Event/HltEvent
                         GaudiConf)

gaudi_add_module(TeslaTools
                 src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost HltEvent RecEvent DaVinciKernelLib
                 MicroDST DaVinciInterfacesLib LHCbAlgs)

gaudi_install_python_modules()

