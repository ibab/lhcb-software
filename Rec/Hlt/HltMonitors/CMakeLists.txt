################################################################################
# Package: HltMonitors
################################################################################
gaudi_subdir(HltMonitors v1r8)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/HltEvent
                         GaudiAlg
                         GaudiKernel
                         GaudiUtils
                         Phys/LoKiHlt)

find_package(AIDA)

gaudi_add_module(HltMonitors
                 src/*.cpp
                 LINK_LIBRARIES DetDescLib HltEvent GaudiAlgLib GaudiKernel GaudiUtilsLib LoKiHltLib)

gaudi_install_python_modules()

