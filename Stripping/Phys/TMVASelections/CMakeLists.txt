################################################################################
# Package: TMVASelections
################################################################################
gaudi_subdir(TMVASelections v1r6)

gaudi_depends_on_subdirs(GaudiAlg
                         GaudiKernel
                         Phys/DaVinciKernel
                         Phys/LoKi
                         Phys/LoKiPhys)

find_package(ROOT COMPONENTS MLP TMVA Graf Hist Matrix TreePlayer Gpad Graf3d)

gaudi_add_module(TMVASelections
                 src/*.cpp
                 INCLUDE_DIRS ROOT Phys/LoKi
                 LINK_LIBRARIES ROOT GaudiAlgLib GaudiKernel DaVinciKernelLib LoKiPhysLib)

