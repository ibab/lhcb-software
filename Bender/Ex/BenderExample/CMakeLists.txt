################################################################################
# Package: BenderExample
################################################################################
gaudi_subdir(BenderExample v20r0)

gaudi_depends_on_subdirs(Phys/Bender
                         Phys/BenderTools)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
