################################################################################
# Package: DetDescSvc
################################################################################
gaudi_subdir(DetDescSvc v3r8p2)

gaudi_depends_on_subdirs(Det/DetDesc
                         GaudiAlg
                         GaudiKernel)

find_package(Boost)

# Needed to be able to include IXmlParserSvc.h
gaudi_depends_on_subdirs(Tools/XmlTools)
find_package(XercesC)

gaudi_add_module(DetDescSvc
                 src/*.cpp
                 INCLUDE_DIRS Boost XercesC
                 LINK_LIBRARIES Boost XercesC DetDescLib GaudiAlgLib GaudiKernel)

