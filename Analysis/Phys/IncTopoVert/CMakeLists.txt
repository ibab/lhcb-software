
################################################################################
# Package: IncTopoVert
################################################################################
gaudi_subdir(IncTopoVert v1r1)

gaudi_depends_on_subdirs(GaudiAlg
                         Event/RecEvent
                         Kernel/LHCbMath
                         Event/HltEvent
                         Event/L0Event
                         Phys/DaVinciInterfaces)

gaudi_add_module(IncTopoVert
                 src/*.cpp
                 PUBLIC_HEADERS IncTopoVert
                 LINK_LIBRARIES GaudiAlgLib RecEventLib TrackEventLib)

