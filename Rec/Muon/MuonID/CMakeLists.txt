################################################################################
# Package: MuonID
################################################################################
gaudi_subdir(MuonID v9r7)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/RecEvent
                         Event/TrackEvent
                         Tr/TrackInterfaces
                         GaudiAlg)

find_package(ROOT COMPONENTS Hist Gpad Graf)

gaudi_add_module(MuonID
                 src/*.cpp
                 INCLUDE_DIRS ROOT Tr/TrackInterfaces
                 LINK_LIBRARIES ROOT MuonDetLib RecEvent TrackEvent GaudiAlgLib)

gaudi_install_python_modules()

