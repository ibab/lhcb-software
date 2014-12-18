################################################################################
# Package: BenderTutor
################################################################################
gaudi_subdir(BenderTutor v25r3)

gaudi_depends_on_subdirs(Ex/BenderExample
                         Phys/Bender)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
