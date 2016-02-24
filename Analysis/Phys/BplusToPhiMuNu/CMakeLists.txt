#============================================================================
# Created    : 2016-02-24
# Maintainer : Iwan Smith
# Package    : BplusToPhiMuNu
#============================================================================
gaudi_subdir(BplusToPhiMuNu v1r0)

# FIXME: may very well be too much
gaudi_depends_on_subdirs(Phys/DecayTreeTuple
                         Event/GenEvent
                         )

find_package(HepMC)

find_package(ROOT COMPONENTS Physics)

gaudi_add_module(BplusToPhiMuNu
                 src/*.cpp
                 INCLUDE_DIRS HepMC ROOT
                 LINK_LIBRARIES DecayTreeTupleBaseLib HltEvent HepMC ROOT)
