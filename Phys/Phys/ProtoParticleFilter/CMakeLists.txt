################################################################################
# Package: ProtoParticleFilter
################################################################################
gaudi_subdir(ProtoParticleFilter v1r9)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         GaudiAlg
                         Phys/DaVinciInterfaces
                         Tr/TrackInterfaces)

gaudi_add_module(ProtoParticleFilter
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES CaloUtils GaudiAlgLib DaVinciInterfacesLib)

