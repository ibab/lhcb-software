################################################################################
# Package: GaussMonitor
################################################################################
gaudi_subdir(GaussMonitor v7r0)

gaudi_depends_on_subdirs(Event/GenEvent
                         Kernel/MCInterfaces
                         Phys/LoKiGen
                         Phys/LoKiMC
                         Sim/GaussTools)

gaudi_add_module(GaussMonitor
                 src/*.cpp
                 INCLUDE_DIRS AIDA Kernel/MCInterfaces
                 LINK_LIBRARIES GenEvent LoKiGenLib LoKiMCLib GaussToolsLib
                 GENCONF_PRELOAD GaussToolsGenConfHelperLib)

