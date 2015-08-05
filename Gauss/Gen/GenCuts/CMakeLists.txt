################################################################################
# Package: GenCuts
################################################################################
gaudi_subdir(GenCuts v3r12)

gaudi_depends_on_subdirs(GaudiAlg
                         Kernel/MCInterfaces
                         Phys/LoKiGen)

find_package(FastJet)

gaudi_add_module(GenCuts
                 src/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces FastJet
                 LINK_LIBRARIES GaudiAlgLib LoKiGenLib FastJet)

