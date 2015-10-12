################################################################################
# Package: RecreatePIDTools
################################################################################
gaudi_subdir(RecreatePIDTools v1r8)

gaudi_depends_on_subdirs(Event/RecEvent
                         GaudiAlg)

gaudi_add_module(RecreatePIDTools
                 src/*.cpp
                 LINK_LIBRARIES RecEvent GaudiAlgLib)

