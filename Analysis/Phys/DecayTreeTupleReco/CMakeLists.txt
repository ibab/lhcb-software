################################################################################
# Package: DecayTreeTupleReco
################################################################################
gaudi_subdir(DecayTreeTupleReco v1r2)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/HltEvent
                         Phys/DecayTreeTupleBase)

gaudi_add_module(DecayTreeTupleReco
                 src/*.cpp
                 LINK_LIBRARIES MuonDetLib HltEvent DecayTreeTupleBaseLib)

