################################################################################
# Package: Tesla
################################################################################
gaudi_subdir(Tesla v2r0)

gaudi_depends_on_subdirs(Event/HltEvent
                         Event/MicroDst
                         Det/DDDB
                         GaudiConfUtils
                         Hlt/HltDAQ
                         Phys/TeslaTools
                         Phys/DaVinciKernel)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
