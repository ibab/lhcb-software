################################################################################
# Package: VeloSimulation
################################################################################
gaudi_subdir(VeloSimulation v3r15p1)

gaudi_depends_on_subdirs(Det/VeloDet
                         Event/MCEvent
                         GaudiAlg)

find_package(AIDA)

gaudi_add_module(VeloSimulation
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES VeloDetLib MCEvent GaudiAlgLib)

