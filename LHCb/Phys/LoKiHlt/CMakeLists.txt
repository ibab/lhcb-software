################################################################################
# Package: LoKiHlt
################################################################################
gaudi_subdir(LoKiHlt v3r20p1)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/HltEvent
                         Event/L0Event
                         GaudiPython
                         Kernel/HltInterfaces
                         Phys/LoKiCore
                         Phys/LoKiNumbers)

gaudi_add_library(LoKiHltLib
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  LINK_LIBRARIES DAQEventLib HltEvent L0Event HltInterfaces LoKiCoreLib)

gaudi_add_module(LoKiHlt
                 src/Components/*.cpp
                 LINK_LIBRARIES DAQEventLib HltEvent L0Event HltInterfaces LoKiCoreLib LoKiHltLib)

gaudi_add_dictionary(LoKiHlt
                     dict/LoKiHltDict.h
                     dict/LoKiHlt.xml
                     LINK_LIBRARIES DAQEventLib HltEvent L0Event HltInterfaces LoKiCoreLib LoKiHltLib
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
