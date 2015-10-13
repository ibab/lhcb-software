################################################################################
# Package: RichPIDMerge
################################################################################
gaudi_subdir(RichPIDMerge v2r11p1)

gaudi_depends_on_subdirs(Rich/RichGlobalPID
                         Rich/RichRecBase)

gaudi_add_module(RichPIDMerge
                 src/*.cpp
                 INCLUDE_DIRS Rich/RichGlobalPID
                 LINK_LIBRARIES RichRecBase)

gaudi_env(SET RICHPIDMERGEOPTS \${RICHPIDMERGEROOT}/options)

