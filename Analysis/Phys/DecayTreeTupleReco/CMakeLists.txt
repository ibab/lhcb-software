################################################################################
# Package: DecayTreeTupleReco
################################################################################
gaudi_subdir(DecayTreeTupleReco v1r5)

gaudi_depends_on_subdirs(Phys/DecayTreeTupleBase
                         Hlt/HltDAQ
                         Event/HltEvent
  )

gaudi_add_module(DecayTreeTupleReco
                 src/*.cpp
                 LINK_LIBRARIES DecayTreeTupleBaseLib HltEvent)
