################################################################################
# Package: DaVinciTools
################################################################################
gaudi_subdir(DaVinciTools v23r15)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Phys/DaVinciCoreTools
                         Phys/RelatedInfoTools
                         Phys/ExtraInfoTools
                         Phys/DaVinciPVTools
                         Phys/DaVinciFilters
                         Phys/DaVinciDecayFinder
                         Phys/TopologicalTools
                         Phys/DaVinciOverlapsAndClones
                         Phys/DaVinciNeutralTools)

#find_package(Boost COMPONENTS regex)

gaudi_add_module(DaVinciTools
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
