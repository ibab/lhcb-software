################################################################################
# Package: StrippingUtils
################################################################################
gaudi_subdir(StrippingUtils v1r7)

gaudi_depends_on_subdirs(Phys/StrippingConf
                         PhysSel/PhysSelPython)

find_package(pytools)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
