################################################################################
# Package: DetDescExample
################################################################################
gaudi_subdir(DetDescExample v7r8p1)

gaudi_depends_on_subdirs(Det/CaloDet
                         Det/CaloDetXmlCnv
                         Det/DDDB
                         Det/DetDescCnv
                         Det/DetDescSvc
                         GaudiAlg
                         GaudiConf
                         GaudiSvc)

gaudi_add_executable(DetDescExample
                     ${CMAKE_SOURCE_DIR}/GaudiConf/src/GaudiMain.cpp
                     src/*.cpp
                     LINK_LIBRARIES CaloDetLib DetDescCnvLib GaudiAlgLib)

