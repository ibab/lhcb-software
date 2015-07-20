################################################################################
# Package: SimComponents
################################################################################
gaudi_subdir(SimComponents v2r25)

gaudi_depends_on_subdirs(Det/CaloDet
                         Event/GenEvent
                         Event/MCEvent
                         GaudiAlg
                         Kernel/MCInterfaces)

find_package(AIDA)
find_package(Boost)

gaudi_add_module(SimComponents
                 src/*.cpp
                 INCLUDE_DIRS Boost AIDA Kernel/MCInterfaces
                 LINK_LIBRARIES Boost CaloDetLib GenEvent MCEvent GaudiAlgLib)

