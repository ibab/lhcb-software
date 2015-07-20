################################################################################
# Package: SwimmingEvent
################################################################################
gaudi_subdir(SwimmingEvent v1r7)

gaudi_depends_on_subdirs(Event/PhysEvent
                         GaudiKernel
                         Kernel/Relations)

gaudi_add_dictionary(SwimmingEvent
                     dict/SwimmingEvent.h
                     dict/SwimmingEvent.xml
                     LINK_LIBRARIES PhysEvent GaudiKernel RelationsLib
                     OPTIONS "--no_membertypedefs")

gaudi_install_headers(Event)

