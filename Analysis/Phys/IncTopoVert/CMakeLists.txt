
################################################################################
# Package: IncTopoVert
################################################################################
gaudi_subdir(IncTopoVert v1r1p1)

gaudi_depends_on_subdirs(GaudiAlg
                         Event/RecEvent
                         Kernel/LHCbMath
                         Event/HltEvent
                         Event/L0Event
                         Phys/DaVinciInterfaces)

gaudi_install_headers(IncTopoVert)

gaudi_add_module(IncTopoVert
                 src/*.cpp
                 LINK_LIBRARIES GaudiAlgLib RecEventLib TrackEventLib)

