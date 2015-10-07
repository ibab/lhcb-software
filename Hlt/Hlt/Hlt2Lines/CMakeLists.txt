################################################################################
# Package: Hlt2Lines
################################################################################
gaudi_subdir(Hlt2Lines v10r4)

gaudi_depends_on_subdirs(Hlt/Hlt2SharedParticles
                         Hlt/HltLine
                         Hlt/HltTracking)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
