################################################################################
# Package: HltConf
################################################################################
gaudi_subdir(HltConf v13r2)

gaudi_depends_on_subdirs(Hlt/Hlt1Lines
                         Hlt/Hlt2Lines
                         Hlt/Hlt2SharedParticles
                         Hlt/HltLine
                         Hlt/HltLuminosity
                         Hlt/HltSettings
                         Hlt/HltTracking)

set_property(DIRECTORY PROPERTY CONFIGURABLE_USER_MODULES HltConf.Hlt1 HltConf.Hlt2 HltConf.Configuration)
gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
