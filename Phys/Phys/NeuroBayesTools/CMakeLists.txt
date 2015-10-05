################################################################################
# Package: NeuroBayesTools
################################################################################
gaudi_subdir(NeuroBayesTools v1r4)

gaudi_depends_on_subdirs(GaudiAlg
                         GaudiKernel
                         Phys/DaVinciKernel
                         Phys/LoKi
                         Phys/LoKiPhys)

find_package(NeuroBayesExpert)

gaudi_add_module(NeuroBayesTool
                 src/*.cpp
                 INCLUDE_DIRS NeuroBayesExpert AIDA Phys/LoKi
                 LINK_LIBRARIES NeuroBayesExpert GaudiAlgLib GaudiKernel DaVinciKernelLib LoKiPhysLib)

gaudi_install_python_modules()

