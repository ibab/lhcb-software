################################################################################
# Package: DecayTreeTupleMuonCalib
################################################################################
gaudi_subdir(DecayTreeTupleMuonCalib v1r6)

gaudi_depends_on_subdirs(Det/MuonDet
                         Phys/DecayTreeTupleBase)

find_package(HepMC)

gaudi_add_module(DecayTreeTupleMuonCalib
                 src/*.cpp
                 INCLUDE_DIRS HepMC
                 LINK_LIBRARIES HepMC MuonDetLib DecayTreeTupleBaseLib)

