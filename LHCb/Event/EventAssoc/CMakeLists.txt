################################################################################
# Package: EventAssoc
################################################################################
gaudi_subdir(EventAssoc v5r3)

gaudi_depends_on_subdirs(Event/GenEvent
                         Event/L0Event
                         Event/MCEvent
                         Event/PhysEvent
                         Event/TrackEvent
                         GaudiPython
                         Kernel/Relations)

gaudi_add_dictionary(EventAssocPhys
                     dict/selPhys.h
                     dict/selPhys.xml
                     LINK_LIBRARIES GenEvent L0Event MCEvent PhysEvent TrackEvent RelationsLib
                     OPTIONS "-U__MINGW32__")

gaudi_add_dictionary(EventAssocMC
                     dict/selMC.h
                     dict/selMC.xml
                     LINK_LIBRARIES GenEvent L0Event MCEvent PhysEvent TrackEvent RelationsLib
                     OPTIONS "-U__MINGW32__")

