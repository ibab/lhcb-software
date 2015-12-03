################################################################################
# Package: TeslaTools
################################################################################
gaudi_subdir(TeslaTools v1r17)

gaudi_depends_on_subdirs(Event/MicroDst
                         Kernel/LHCbAlgs
                         Event/RecEvent
                         Phys/DaVinciKernel
                         Phys/LoKiArrayFunctors
                         Phys/LoKiPhys
                         Phys/DaVinciInterfaces
                         Event/HltEvent
                         Event/DAQEvent
                         Hlt/HltDAQ
                         Hlt/HltServices
                         Kernel/HltInterfaces
                         GaudiUtils
                         GaudiConf)

gaudi_add_module(TeslaTools
                 src/*.cpp
                 LINK_LIBRARIES DAQEventLib HltEvent RecEvent DaVinciKernelLib HltInterfaces
                 LoKiArrayFunctorsLib LoKiPhysLib
                 DaVinciInterfacesLib)

gaudi_install_headers(TeslaTools)

gaudi_install_python_modules()

