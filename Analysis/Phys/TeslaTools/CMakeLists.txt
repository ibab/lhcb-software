################################################################################
# Package: TeslaTools
################################################################################
gaudi_subdir(TeslaTools v1r2)

gaudi_depends_on_subdirs(Event/MicroDst
                         Kernel/LHCbAlgs
                         Event/RecEvent
                         Phys/DaVinciKernel
                         Phys/DaVinciInterfaces
                         Event/HltEvent
                         Hlt/HltDAQ
                         GaudiConf)

gaudi_add_module(TeslaTools
                 src/*.cpp
                 LINK_LIBRARIES HltEvent RecEvent DaVinciKernelLib
                 DaVinciInterfacesLib)

gaudi_install_python_modules()

