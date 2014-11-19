################################################################################
# Package: MuonMoniSim
################################################################################
gaudi_subdir(MuonMoniSim v3r0)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(MuonMoniSim
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES MuonDetLib MCEvent GaudiAlgLib)

