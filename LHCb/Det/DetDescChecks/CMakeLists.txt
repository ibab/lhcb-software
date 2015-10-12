################################################################################
# Package: DetDescChecks
################################################################################
gaudi_subdir(DetDescChecks v5r12p1)

gaudi_depends_on_subdirs(Det/DDDB
                         Det/DetDesc
                         Det/DetDescCnv
                         Det/DetDescSvc
                         Det/Magnet
                         GaudiAlg
                         GaudiConf
                         GaudiPython
                         GaudiSvc
                         RootHistCnv)

find_package(AIDA)
find_package(Boost)

gaudi_add_module(DetDescChecks
                 src/*.cpp
                 INCLUDE_DIRS Boost AIDA
                 LINK_LIBRARIES Boost DetDescLib DetDescCnvLib GaudiAlgLib)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
