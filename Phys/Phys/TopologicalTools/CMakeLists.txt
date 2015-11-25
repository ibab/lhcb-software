################################################################################
# Package: TopologicalTools
################################################################################
gaudi_subdir(TopologicalTools v1r2)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Event/HltEvent)

gaudi_add_module(TopologicalTools
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib HltEvent)
