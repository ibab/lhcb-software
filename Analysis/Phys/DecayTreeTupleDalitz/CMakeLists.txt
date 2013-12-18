################################################################################
# Package: DecayTreeTupleDalitz
################################################################################
gaudi_subdir(DecayTreeTupleDalitz v1r1)

gaudi_depends_on_subdirs(Phys/DaVinciMCKernel
                         Phys/DecayTreeTupleBase
                         Phys/LoKiPhysMC)

gaudi_add_module(DecayTreeTupleDalitz
                 src/*.cpp
                 LINK_LIBRARIES DaVinciMCKernelLib DecayTreeTupleBaseLib LoKiPhysMCLib)

