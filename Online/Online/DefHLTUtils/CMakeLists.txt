################################################################################
# Package: DefHLTUtils
################################################################################
gaudi_subdir(DefHLTUtils v1r14)

gaudi_depends_on_subdirs(Online/Gaucho
                         Online/OnlineKernel
                         Online/ROMon)

find_package(ROOT COMPONENTS Core Hist Rint Gpad)

gaudi_add_library(DefUtil src/HLTPerfFormatter.cpp src/FileTestSvc.cpp
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS ROOT
                  LINK_LIBRARIES ROMon OnlineKernel GaudiKernel dim ROOT)

macro(add_exe name)
  gaudi_add_executable(${name} src/${name} LINK_LIBRARIES DefUtil)
endmacro()

foreach(app DiracMon FarmStatus HLTFileEqualizer HLTDefBridge DefHltPlot FileDeleter)
  add_exe(${app})
endforeach()

target_link_libraries(DefHltPlot GauchoLib)
