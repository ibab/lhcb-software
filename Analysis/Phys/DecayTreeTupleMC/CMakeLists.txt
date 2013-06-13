################################################################################
# Package: DecayTreeTupleMC
################################################################################
gaudi_subdir(DecayTreeTupleMC v1r3)

gaudi_depends_on_subdirs(Phys/DaVinciMCKernel
                         Phys/DecayTreeTupleBase
                         Phys/LoKiPhysMC)

gaudi_add_module(DecayTreeTupleMC
                 src/*.cpp
                 LINK_LIBRARIES DaVinciMCKernelLib DecayTreeTupleBaseLib LoKiPhysMCLib)

