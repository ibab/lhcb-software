################################################################################
# Package: MuonTools
################################################################################
gaudi_subdir(MuonTools v5r5p1)

gaudi_depends_on_subdirs(Det/MuonDet
                         Muon/MuonDAQ
                         Muon/MuonInterfaces
                         GaudiAlg)

gaudi_add_module(MuonTools
                 src/*.cpp
                 LINK_LIBRARIES MuonDetLib MuonInterfacesLib GaudiAlgLib)

