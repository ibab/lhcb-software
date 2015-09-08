################################################################################
# Package: StrippingConf
################################################################################
gaudi_subdir(StrippingConf v4r19)

gaudi_depends_on_subdirs(GaudiAlg
                         Kernel/SelectionLine
                         Phys/DaVinciKernel
                         Phys/LoKiCore
                         Phys/SelPy
                         Phys/StrippingAlgs)

find_package(AIDA)
find_package(Boost)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
