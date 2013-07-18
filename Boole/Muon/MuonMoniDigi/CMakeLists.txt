################################################################################
# Package: MuonMoniDigi
################################################################################
gaudi_subdir(MuonMoniDigi v2r8)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/DigiEvent
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(MuonMoniDigi
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent
                 LINK_LIBRARIES MuonDetLib MCEvent GaudiAlgLib)

