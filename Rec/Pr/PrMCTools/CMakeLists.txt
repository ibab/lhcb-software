################################################################################
# Package: PrMCTools
################################################################################
gaudi_subdir(PrMCTools v1r2)

gaudi_depends_on_subdirs(Event/FTEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg
                         Pr/PrKernel)

gaudi_add_module(PrMCTools
                 src/*.cpp
                 INCLUDE_DIRS Event/FTEvent
                 LINK_LIBRARIES LinkerEvent MCEvent GaudiAlgLib PrKernel)

