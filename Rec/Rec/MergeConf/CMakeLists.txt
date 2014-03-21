################################################################################
# Package: MergeConf
################################################################################
gaudi_subdir(L0App v2r1)

gaudi_depends_on_subdirs(Det/DDDB
                         GaudiConf
                         GaudiKernel
                         Kernel/LHCbAlgs
                         Rec/LumiAlgs)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
