################################################################################
# Package: DaVinciTests
################################################################################
gaudi_subdir(DaVinciTests v3r1)

gaudi_depends_on_subdirs(GaudiConf)

gaudi_add_test(QMTest QMTEST)

gaudi_install_python_modules()
