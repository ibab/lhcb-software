################################################################################
# Package: GaudiConfUtils
################################################################################
gaudi_subdir(GaudiConfUtils v1r0)

gaudi_depends_on_subdirs(GaudiKernel)

find_package(pytools)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
