################################################################################
# Package: RichRecMCTools
################################################################################
gaudi_subdir(RichRecMCTools v2r33)

gaudi_depends_on_subdirs(Det/RichDet
                         Event/LinkerEvent
                         Event/MCEvent
                         Kernel/MCInterfaces
                         Rich/RichRecBase)

gaudi_add_module(RichRecMCTools
                 src/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces
                 LINK_LIBRARIES RichDetLib LinkerEvent MCEvent RichRecBase)

