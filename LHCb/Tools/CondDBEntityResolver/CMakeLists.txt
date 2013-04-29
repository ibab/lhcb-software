################################################################################
# Package: CondDBEntityResolver
################################################################################
gaudi_subdir(CondDBEntityResolver v5r5)

gaudi_depends_on_subdirs(Det/DetCond
                         Tools/XmlTools)

gaudi_add_module(CondDBEntityResolver
                 src/component/*.cpp
                 LINK_LIBRARIES DetCondLib XmlToolsLib)

