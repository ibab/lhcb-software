################################################################################
# Package: L0Interfaces
################################################################################
gaudi_subdir(L0Interfaces v2r4)

gaudi_depends_on_subdirs(Event/L0Event)

gaudi_add_dictionary(L0Interfaces
                     dict/L0InterfacesDict.h
                     dict/L0InterfacesDict.xml
                     LINK_LIBRARIES L0Event
                     OPTIONS "-U__MINGW32__")

gaudi_install_headers(L0Interfaces)

