################################################################################
# Package: PyAna
################################################################################
gaudi_subdir(PyAna v1r3)

find_package(pytools)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
