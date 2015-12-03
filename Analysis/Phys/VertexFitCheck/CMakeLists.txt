################################################################################
# Package: VertexFitCheck
################################################################################
gaudi_subdir(VertexFitCheck v1r10)

gaudi_depends_on_subdirs(Event/LinkerEvent
                         Event/MCEvent
                         Phys/DaVinciAssociators
                         Phys/DaVinciKernel)

gaudi_add_module(VertexFitCheck
                 src/*.cpp
                 LINK_LIBRARIES LinkerEvent MCEvent DaVinciKernelLib)

