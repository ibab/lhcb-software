################################################################################
# Package: RichGlobalPID
################################################################################
gaudi_subdir(RichGlobalPID v3r20)

gaudi_depends_on_subdirs(GaudiObjDesc
                         Rich/RichRecBase)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_module(RichGlobalPID
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES RichRecBase)

gaudi_install_headers(RichGlobalPID)

gaudi_env(SET RICHGLOBALPIDOPTS \${RICHGLOBALPIDROOT}/options)

