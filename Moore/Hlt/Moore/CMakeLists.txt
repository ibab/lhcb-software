################################################################################
# Package: Moore
################################################################################
gaudi_subdir(Moore v23r7p2)

gaudi_depends_on_subdirs(Det/DDDB
                         GaudiConf
                         GaudiKernel
                         GaudiPython
                         GaudiSvc
                         Hlt/HltConf
                         L0/L0Interfaces)

find_package(HepMC)

gaudi_install_python_modules()

gaudi_alias(Moore
            gaudirun.py)


gaudi_add_test(QMTest QMTEST)
