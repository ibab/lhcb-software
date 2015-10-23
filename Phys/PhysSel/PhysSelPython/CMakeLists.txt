################################################################################
# Package: PhysSelPython
################################################################################
gaudi_subdir(PhysSelPython v9r8)

gaudi_depends_on_subdirs(GaudiConf
                         GaudiConfUtils
                         GaudiPython
                         Phys/SelPy)

find_package(pytools)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
