################################################################################
# Package: TeslaTools
################################################################################
gaudi_subdir(TeslaTools v1r10p1)

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
                         GaudiUtils
                         GaudiConf)

gaudi_add_module(TeslaTools
                 src/*.cpp
                 LINK_LIBRARIES DAQEventLib HltEvent RecEvent DaVinciKernelLib
                 LoKiArrayFunctorsLib LoKiPhysLib
                 DaVinciInterfacesLib)

gaudi_install_headers(TeslaTools)

gaudi_install_python_modules()

