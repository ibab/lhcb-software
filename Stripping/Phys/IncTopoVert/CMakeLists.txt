
################################################################################
# Package: IncTopoVert
################################################################################
gaudi_subdir(IncTopoVert v1r6)

gaudi_depends_on_subdirs(GaudiAlg
                         Event/RecEvent
                         Event/HltEvent
                         Event/PhysEvent
                         Event/L0Event
                         Event/MCEvent
                         Kernel/LHCbMath
                         Phys/DaVinciInterfaces
                         Phys/DaVinciKernel)

gaudi_install_headers(IncTopoVert)

gaudi_add_module(IncTopoVert
                 src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib RecEvent TrackEvent PhysEvent
                                MCEvent HltEvent LHCbMathLib
                                DaVinciInterfacesLib DaVinciKernelLib)
