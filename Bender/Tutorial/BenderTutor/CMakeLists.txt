################################################################################
# Package: BenderTutor
################################################################################
gaudi_subdir(BenderTutor v20r0)

gaudi_depends_on_subdirs(Ex/BenderExample
                         Phys/Bender)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
