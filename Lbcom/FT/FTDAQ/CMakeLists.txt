################################################################################
# Package: FTDAQ
################################################################################
gaudi_subdir(FTDAQ v1r3)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/FTEvent
                         GaudiAlg)

gaudi_add_module(FTDAQ
                 src/*.cpp
                 INCLUDE_DIRS Event/FTEvent
                 LINK_LIBRARIES DAQEventLib GaudiAlgLib)

