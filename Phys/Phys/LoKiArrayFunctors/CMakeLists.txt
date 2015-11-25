################################################################################
# Package: LoKiArrayFunctors
################################################################################
gaudi_subdir(LoKiArrayFunctors v11r5)

gaudi_depends_on_subdirs(Phys/LoKiPhys
                         Phys/LoKiProtoParticles)

gaudi_add_library(LoKiArrayFunctorsLib
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  LINK_LIBRARIES LoKiPhysLib LoKiProtoParticles)

gaudi_add_module(LoKiArrayFunctors
                 src/Components/*.cpp
                 LINK_LIBRARIES LoKiPhysLib LoKiProtoParticles LoKiArrayFunctorsLib)

gaudi_add_dictionary(LoKiArrayFunctors
                     dict/LoKiArrayFunctorsDict.h
                     dict/LoKiArrayFunctors.xml
                     LINK_LIBRARIES LoKiPhysLib LoKiProtoParticles LoKiArrayFunctorsLib
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
