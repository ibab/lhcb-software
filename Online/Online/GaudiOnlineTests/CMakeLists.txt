################################################################################
# Package: GaudiOnlineTests
################################################################################
gaudi_subdir(GaudiOnlineTests v1r7)

gaudi_depends_on_subdirs(GaudiAud
                         GaudiSvc
                         GaudiAlg
                         DAQ/MDF
                         Event/DAQEvent
                         Online/DIM
                         Online/RootCnv
                         Online/OnlineKernel
                         Online/RawBankSizes
                         Online/GaudiOnline
                         Online/GaudiCheckpointing
                         GaudiExamples)

find_package(PythonLibs)
find_package(AIDA)

gaudi_add_library(GaudiOnlineTests src/*.cpp
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} GaudiKernel)

gaudi_add_test(QMTest QMTEST)

gaudi_env(SET GaudiOnlineExe   "GaudiOnlineExe.exe")
gaudi_env(SET GAUDIONLINE_OPTS "\${GAUDIONLINEROOT}/options")
gaudi_install_scripts()
