################################################################################
# Package: GaudiMTTools
################################################################################
gaudi_subdir(GaudiMTTools v1r2p2)

gaudi_depends_on_subdirs(GaudiAlg)

find_package(Boost COMPONENTS thread system)

gaudi_add_module(GaudiMTTools
                 src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost GaudiAlgLib)

