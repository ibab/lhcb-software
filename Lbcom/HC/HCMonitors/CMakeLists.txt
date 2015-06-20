################################################################################
# Package: HCMonitors
################################################################################
gaudi_subdir(HCMonitors v1r0)

gaudi_depends_on_subdirs(Event/DigiEvent
                         GaudiAlg
                         GaudiConf)

find_package(AIDA)

gaudi_add_module(HCMonitors
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent
                 LINK_LIBRARIES GaudiAlgLib)

gaudi_install_python_modules()

