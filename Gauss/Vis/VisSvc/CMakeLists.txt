################################################################################
# Package: VisSvc
################################################################################
gaudi_subdir(VisSvc v5r8)

gaudi_depends_on_subdirs(Det/DetDesc
                         Tools/XmlTools)

find_package(Boost COMPONENTS regex)

gaudi_add_module(VisSvc
                 src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost DetDescLib XmlToolsLib)

gaudi_install_headers(VisSvc)

