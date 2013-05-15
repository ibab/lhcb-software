################################################################################
# Package: LinkerInstances
################################################################################
gaudi_subdir(LinkerInstances v3r7)

gaudi_depends_on_subdirs(Event/DigiEvent
                         Event/FTEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         Event/TrackEvent)

gaudi_add_dictionary(LinkerInstances
                     dict/linker_instances.h
                     xml/lcg_selection.xml
                     INCLUDE_DIRS
                     LINK_LIBRARIES LinkerEvent MCEvent PhysEvent RecEvent TrackEvent FTEvent
                     OPTIONS "-U__MINGW32__")

gaudi_install_python_modules()

