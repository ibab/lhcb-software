################################################################################
# Package: MuonTools
################################################################################
gaudi_subdir(MuonTools v6r0)

gaudi_depends_on_subdirs(Det/MuonDet
                         Muon/MuonDAQ
                         Muon/MuonInterfaces
                         GaudiAlg)

gaudi_add_module(MuonTools
                 src/*.cpp
                 LINK_LIBRARIES MuonDetLib MuonInterfacesLib GaudiAlgLib)

