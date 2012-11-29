################################################################################
# Package: PhysDict
################################################################################
gaudi_subdir(PhysDict v4r2p2)

gaudi_depends_on_subdirs(Event/HltEvent
                         Event/RecEvent)

gaudi_add_dictionary(Phys
                     dict/PhysDict.h
                     dict/PhysDict.xml
                     LINK_LIBRARIES HltEvent RecEvent
                     OPTIONS "-U__MINGW32__")

