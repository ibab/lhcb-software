################################################################################
# Package: HltBeamGasAlley
################################################################################
gaudi_subdir(HltBeamGasAlley v3r1p1)

gaudi_depends_on_subdirs(Det/VeloDet
                         GaudiAlg
                         Hlt/HltBase)

find_package(AIDA)

gaudi_add_module(HltBeamGasAlley
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES VeloDetLib GaudiAlgLib HltBase)


gaudi_add_test(QMTest QMTEST)
