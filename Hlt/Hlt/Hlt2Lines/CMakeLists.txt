################################################################################
# Package: Hlt2Lines
################################################################################
gaudi_subdir(Hlt2Lines v10r7)

gaudi_depends_on_subdirs(Hlt/Hlt2SharedParticles
                         Hlt/HltJets
                         Hlt/HltLine
                         )

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
