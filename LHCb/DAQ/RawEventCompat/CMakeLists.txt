################################################################################
# Package: RawEventCompat
################################################################################
gaudi_subdir(RawEventCompat v1r6p2)

gaudi_depends_on_subdirs(GaudiKernel
                         DAQ/DAQUtils)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
