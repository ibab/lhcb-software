################################################################################
# Package: MuonBackground
################################################################################
gaudi_subdir(MuonBackground v2r13)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/GenEvent
                         Event/MCEvent
                         GaudiAlg)

find_package(AIDA)

gaudi_add_module(MuonBackground
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES MuonDetLib GenEvent MCEvent GaudiAlgLib)

