################################################################################
# Package: LoKiProtoParticles
################################################################################
gaudi_subdir(LoKiProtoParticles v2r1p3)

gaudi_depends_on_subdirs(Event/RecEvent
                         Phys/DaVinciInterfaces
                         Phys/LoKiCore
                         Phys/LoKiUtils)

gaudi_add_library(LoKiProtoParticles
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  LINK_LIBRARIES RecEvent DaVinciInterfacesLib LoKiCoreLib LoKiUtils)

gaudi_add_dictionary(LoKiProtoParticles
                     dict/LoKiProtoParticlesDict.h
                     dict/LoKiProtoParticles.xml
                     LINK_LIBRARIES RecEvent DaVinciInterfacesLib LoKiCoreLib LoKiUtils LoKiProtoParticles
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
