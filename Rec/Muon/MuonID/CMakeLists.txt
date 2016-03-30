################################################################################
# Package: MuonID
################################################################################
gaudi_subdir(MuonID v10r1)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/RecEvent
                         Event/TrackEvent
                         Muon/MuonInterfaces
                         Muon/MuonTrackRec
                         Tr/TrackInterfaces
                         Tr/TrackFitEvent
                         GaudiAlg)

find_package(ROOT COMPONENTS Hist Gpad Graf Matrix)
find_package(VDT)

gaudi_add_library(MuonIDLib
                 src/lib/*.cpp
                 PUBLIC_HEADERS MuonID
                 INCLUDE_DIRS ROOT Tr/TrackInterfaces VDT
                 LINK_LIBRARIES ROOT MuonDetLib RecEvent TrackEvent)

gaudi_add_module(MuonID
                 src/component/*.cpp
                 INCLUDE_DIRS ROOT Tr/TrackInterfaces VDT
                 LINK_LIBRARIES ROOT MuonDetLib RecEvent TrackEvent MuonInterfacesLib TrackFitEvent GaudiAlgLib MuonIDLib)

#gaudi_add_dictionary(MuonID
#                     dict/MuonIDDict.h
#                     dict/MuonID.xml
#                     INCLUDE_DIRS ROOT AIDA Boost RELAX ROOT PythonLibs
#                     LINK_LIBRARIES ROOT Boost RELAX ROOT PythonLibs GaudiAlgLib LHCbKernel LHCbMathLib PartPropLib LoKiCoreLib
#                     OPTIONS " -U__MINGW32__ ")


gaudi_install_python_modules()
