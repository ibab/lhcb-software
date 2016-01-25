################################################################################
# Package: GenTune
################################################################################
gaudi_subdir(GenTune v3r0)

gaudi_depends_on_subdirs(Event/GenEvent
                         GaudiAlg
                         Gen/GENSER
                         Gen/Generators)

find_package(Boost)
find_package(FastJet)
find_package(HepMC)
find_package(ROOT)
find_package(Rivet)

gaudi_add_module(GenTune
                 src/*.cpp
                 INCLUDE_DIRS AIDA Rivet
                 LINK_LIBRARIES Rivet GenEvent GaudiAlgLib pythia6forgauss GeneratorsLib)


gaudi_add_test(QMTest QMTEST)
