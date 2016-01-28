################################################################################
# Package: RichRecMonitors
################################################################################
gaudi_subdir(RichRecMonitors v2r37)

gaudi_depends_on_subdirs(Event/LinkerEvent
                         Event/MCEvent
                         Kernel/MCInterfaces
                         Rich/RichRecBase)

gaudi_add_module(RichRecMonitors
                 src/*.cpp
                 INCLUDE_DIRS AIDA Kernel/MCInterfaces
                 LINK_LIBRARIES LinkerEvent MCEvent RichRecBase)

gaudi_env(SET RICHRECMONITORSOPTS \${RICHRECMONITORSROOT}/options)

