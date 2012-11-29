################################################################################
# Package: LoKiUtils
################################################################################
gaudi_subdir(LoKiUtils v1r3)

gaudi_depends_on_subdirs(Phys/DaVinciInterfaces
                         Phys/LoKiCore
                         Tr/TrackInterfaces)

gaudi_add_library(LoKiUtils
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  INCLUDE_DIRS Tr/TrackInterfaces
                  LINK_LIBRARIES DaVinciInterfacesLib LoKiCoreLib)

gaudi_add_dictionary(LoKiUtils
                     dict/LoKiUtilsDict.h
                     dict/LoKiUtils.xml
                     INCLUDE_DIRS Tr/TrackInterfaces
                     LINK_LIBRARIES DaVinciInterfacesLib LoKiCoreLib LoKiUtils
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()

