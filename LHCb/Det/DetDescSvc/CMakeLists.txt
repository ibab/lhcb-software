################################################################################
# Package: DetDescSvc
################################################################################
gaudi_subdir(DetDescSvc v3r6)

gaudi_depends_on_subdirs(Det/DetDesc
                         GaudiAlg
                         GaudiKernel)

find_package(Boost)

gaudi_add_module(DetDescSvc
                 src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost DetDescLib GaudiAlgLib GaudiKernel)

