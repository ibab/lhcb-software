################################################################################
# Package: DetCondExample
################################################################################
gaudi_subdir(DetCondExample v11r15)

gaudi_depends_on_subdirs(Det/DetCond
                         Det/DetDesc
                         Det/DetDescCnv
                         Det/DetDescSvc
                         GaudiAlg
                         GaudiConf
                         GaudiKernel
                         GaudiSvc
                         Tools/CondDBEntityResolver
                         Tools/XmlTools)

find_package(MySQL)
find_package(Oracle)
find_package(SQLite)

gaudi_add_executable(DetCondExample
                     src/*.cpp
                     ${CMAKE_SOURCE_DIR}/GaudiConf/src/GaudiMain.cpp
                     LINK_LIBRARIES DetCondLib DetDescLib DetDescCnvLib GaudiAlgLib GaudiKernel XmlToolsLib)

gaudi_env(SET CONDDBCONNECTIONSTRING sqlite_file:CONDDB.db/CONDDB
          SET CONDDBCONNECTIONSTRING1 sqlite_file:CONDDB1.db/CONDDB1
          SET CONDDBCONNECTIONSTRING2 sqlite_file:CONDDB2.db/CONDDB2
          SET CONDITIONS_PATH conddb:/Conditions/MainCatalog.xml
          SET DAQCONNECTIONSTRING sqlite_file:DAQ.db/DAQ
          SET DAQFOLDERNAME DAQ
          SET DETCONDEXAMPLE_XMLDDDB \${DETCONDEXAMPLEROOT}/XMLDDDB)

