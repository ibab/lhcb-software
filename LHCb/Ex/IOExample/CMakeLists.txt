################################################################################
# Package: IOExample
################################################################################
gaudi_subdir(IOExample v4r17)

gaudi_depends_on_subdirs(Event/EventPacker
                         GaudiConf
                         GaudiSvc
                         Kernel/LHCbAlgs
                         Kernel/LHCbKernel
                         Sim/SimComponents)

find_package(RELAX)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
