################################################################################
# Package: RecInterfaces
################################################################################
gaudi_subdir(RecInterfaces v1r1)

gaudi_depends_on_subdirs(Event/PhysEvent
                         Event/RecEvent)

gaudi_add_dictionary(RecInterfaces
                     dict/RecInterfacesDict.h
                     dict/RecInterfacesDict.xml
                     LINK_LIBRARIES PhysEvent RecEvent
                     OPTIONS "-U__MINGW32__")

gaudi_install_headers(RecInterfaces)

