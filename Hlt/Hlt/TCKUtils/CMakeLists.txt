################################################################################
# Package: TCKUtils
################################################################################
gaudi_subdir(TCKUtils v1r17)

gaudi_depends_on_subdirs(Hlt/HltCommon)

find_package(pytools)

gaudi_install_python_modules()
gaudi_install_scripts()


gaudi_add_test(QMTest QMTEST)
