################################################################################
# Package: LbHidValley
################################################################################
gaudi_subdir(LbHidValley v4r0p1)

gaudi_depends_on_subdirs(Gen/GENSER
                         Gen/LbPythia
                         Phys/LoKiGen)

find_package(Boost COMPONENTS program_options)

include(GENSER_headers)

string(REPLACE "-pedantic" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wall" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wextra" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Werror=return-type" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
gaudi_add_library(LbHidValleyLib
                  src/Lib/*.cpp
                  src/Lib/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES pythia6forgauss LbPythiaLib LoKiGenLib)

gaudi_add_module(LbHidValley
                 src/Components/*.cpp
                 LINK_LIBRARIES pythia6forgauss LbPythiaLib LoKiGenLib LbHidValleyLib)

gaudi_add_executable(HidValley
                     src/Main/*.cpp
                     src/Main/*.F
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost pythia6forgauss LbPythiaLib LoKiGenLib LbHidValleyLib)

gaudi_env(SET LBHIDVALLEYOPTS \${LBHIDVALLEYROOT}/options)

