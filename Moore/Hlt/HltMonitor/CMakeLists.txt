################################################################################
# Package: HltMonitor
################################################################################
gaudi_subdir(HltMonitor v1r4)

gaudi_depends_on_subdirs(GaudiKernel)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
