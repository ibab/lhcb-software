################################################################################
# Package: MuonPIDChecker
################################################################################
gaudi_subdir(MuonPIDChecker v5r5p1)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg)

gaudi_add_module(MuonPIDChecker
                 src/*.cpp
                 LINK_LIBRARIES MuonDetLib LinkerEvent MCEvent RecEvent TrackEvent GaudiAlgLib)

gaudi_install_python_modules()

