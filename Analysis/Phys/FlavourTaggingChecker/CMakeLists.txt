################################################################################
# Package: FlavourTaggingChecker
################################################################################
gaudi_subdir(FlavourTaggingChecker v7r10)

gaudi_depends_on_subdirs(Event/GenEvent
                         Event/HltEvent
                         Event/L0Event
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         GaudiKernel
                         Kernel/HltInterfaces
                         Kernel/MCInterfaces
                         Phys/DaVinciKernel
                         Phys/DaVinciMCKernel
                         Phys/LoKiPhys)

gaudi_add_module(FlavourTaggingChecker
                 src/*.cpp
                 INCLUDE_DIRS AIDA Kernel/MCInterfaces
                 LINK_LIBRARIES GenEvent HltEvent L0Event LinkerEvent MCEvent RecEvent GaudiKernel HltInterfaces DaVinciKernelLib DaVinciMCKernelLib LoKiPhysLib)

gaudi_env(SET FLAVOURTAGGINGCHECKEROPTS \${FLAVOURTAGGINGCHECKERROOT}/options)

