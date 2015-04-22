################################################################################
# Package: SelPy
################################################################################
gaudi_subdir(SelPy v4r3)

find_package(pygraphics)
find_package(pytools)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
