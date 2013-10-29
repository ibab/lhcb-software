################################################################################
# Package: DaVinciTests
################################################################################
gaudi_subdir(DaVinciTests v2r20)

gaudi_depends_on_subdirs(GaudiConf)

gaudi_add_test(QMTest QMTEST)

gaudi_install_python_modules()
