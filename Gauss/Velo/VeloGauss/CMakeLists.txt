################################################################################
# Package: VeloGauss
################################################################################
gaudi_subdir(VeloGauss v2r4)

gaudi_depends_on_subdirs(Det/VeloDet
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(VeloGauss
                 src/*.cpp
                 LINK_LIBRARIES VeloDetLib MCEvent GaudiAlgLib)

