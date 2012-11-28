################################################################################
# Package: MuonTools
################################################################################
gaudi_subdir(MuonTools v5r4)

gaudi_depends_on_subdirs(Det/MuonDet
                         GaudiAlg)

gaudi_add_module(MuonTools
                 src/*.cpp
                 LINK_LIBRARIES MuonDetLib GaudiAlgLib)

gaudi_env(SET MUONTOOLSOPTS \${MUONTOOLSROOT}/options)

