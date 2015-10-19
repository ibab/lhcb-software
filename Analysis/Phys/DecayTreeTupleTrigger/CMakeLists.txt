################################################################################
# Package: DecayTreeTupleTrigger
################################################################################
gaudi_subdir(DecayTreeTupleTrigger v1r9)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/HltEvent
                         Event/L0Event
                         Kernel/HltInterfaces
                         Phys/DecayTreeTupleBase)

string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

gaudi_add_module(DecayTreeTupleTrigger
                 src/*.cpp
                 LINK_LIBRARIES DAQEventLib HltEvent L0Event HltInterfaces DecayTreeTupleBaseLib)

