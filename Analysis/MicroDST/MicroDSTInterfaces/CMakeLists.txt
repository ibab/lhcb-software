################################################################################
# Package: MicroDSTInterfaces
################################################################################
gaudi_subdir(MicroDSTInterfaces v1r7)

gaudi_depends_on_subdirs(Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         Event/SwimmingEvent
                         GaudiKernel)

gaudi_add_dictionary(MicroDSTInterfaces
                     dict/MicroDSTInterfacesDict.h
                     dict/MicroDSTInterfacesDict.xml
                     INCLUDE_DIRS Event/SwimmingEvent
                     LINK_LIBRARIES MCEvent PhysEvent RecEvent GaudiKernel
                     OPTIONS "-U__MINGW32__")

gaudi_install_headers(MicroDST)

