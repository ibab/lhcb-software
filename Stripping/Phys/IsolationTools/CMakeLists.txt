################################################################################
# Package: IsolationTools
################################################################################
gaudi_subdir(IsolationTools v1r7)

gaudi_depends_on_subdirs(Phys/DaVinciTools
                         Phys/MVADictTools
                         Phys/DaVinciKernel
                         Phys/LoKiPhys
                         Phys/IncTopoVert
                         Event/HltEvent)

find_package(Boost COMPONENTS regex)

gaudi_add_module(IsolationTools
                 src/*.cpp
                 INCLUDE_DIRS Boost # AIDA
                 LINK_LIBRARIES Boost DaVinciKernelLib MVADictToolsLib
                 LoKiPhysLib HltEvent)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
