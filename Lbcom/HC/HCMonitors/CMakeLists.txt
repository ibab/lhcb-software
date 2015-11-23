################################################################################
# Package: HCMonitors
################################################################################
gaudi_subdir(HCMonitors v1r2)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/DigiEvent
                         GaudiAlg
                         GaudiConf)

find_package(AIDA)

gaudi_add_module(HCMonitors
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent
                 LINK_LIBRARIES DetDescLib GaudiAlgLib)

gaudi_install_python_modules()

