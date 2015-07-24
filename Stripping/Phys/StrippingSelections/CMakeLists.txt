################################################################################
# Package: StrippingSelections
################################################################################
gaudi_subdir(StrippingSelections v4r4)

gaudi_depends_on_subdirs(GaudiConfUtils
                         Kernel/LHCbAlgs
                         Phys/DaVinciKernel
                         Phys/LoKiCore
                         Phys/StrippingConf
                         Phys/StrippingSettings
                         Phys/StrippingUtils
                         Phys/TisTosTobbing
                         PhysSel/PhysSelPython)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
