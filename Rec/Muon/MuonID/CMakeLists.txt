################################################################################
# Package: MuonID
################################################################################
gaudi_subdir(MuonID v9r4p1)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg)

find_package(ROOT COMPONENTS Hist Gpad Graf)

gaudi_add_module(MuonID
                 src/*.cpp
                 INCLUDE_DIRS ROOT
                 LINK_LIBRARIES ROOT MuonDetLib RecEvent TrackEvent GaudiAlgLib)

gaudi_install_python_modules()

