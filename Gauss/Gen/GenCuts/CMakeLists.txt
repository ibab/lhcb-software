################################################################################
# Package: GenCuts
################################################################################
gaudi_subdir(GenCuts v3r5)

gaudi_depends_on_subdirs(GaudiAlg
                         Kernel/MCInterfaces
                         Phys/LoKiGen)

gaudi_add_module(GenCuts
                 src/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces
                 LINK_LIBRARIES GaudiAlgLib LoKiGenLib)

