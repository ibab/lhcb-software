################################################################################
# Package: LoKiPhys
################################################################################
gaudi_subdir(LoKiPhys v11r6)

gaudi_depends_on_subdirs(Event/PhysEvent
                         Phys/DaVinciInterfaces
                         Phys/KalmanFilter
                         Phys/LoKiCore
                         Phys/LoKiNumbers
                         Phys/LoKiUtils)

find_package(VDT)

gaudi_add_library(LoKiPhysLib
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  INCLUDE_DIRS VDT
                  LINK_LIBRARIES VDT PhysEvent DaVinciInterfacesLib KalmanFilter LoKiCoreLib LoKiUtils)

gaudi_add_module(LoKiPhys
                 src/Components/*.cpp
                 INCLUDE_DIRS VDT
                 LINK_LIBRARIES VDT PhysEvent DaVinciInterfacesLib KalmanFilter LoKiCoreLib LoKiUtils LoKiPhysLib)

gaudi_add_dictionary(LoKiPhys
                     dict/LoKiPhysDict.h
                     dict/LoKiPhys.xml
                     LINK_LIBRARIES PhysEvent DaVinciInterfacesLib KalmanFilter LoKiCoreLib LoKiUtils LoKiPhysLib
                     OPTIONS " -U__MINGW32__ ")

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(DecayGrammarTest
                       src/tests/DecayGrammarTest.cpp
                       LINK_LIBRARIES PhysEvent DaVinciInterfacesLib KalmanFilter LoKiCoreLib LoKiUtils LoKiPhysLib)
endif()

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
