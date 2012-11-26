################################################################################
# Package: LoKiNumbers
################################################################################
gaudi_subdir(LoKiNumbers v10r3)

gaudi_depends_on_subdirs(GaudiPython
                         Phys/LoKiCore)

gaudi_add_dictionary(LoKiNumbers
                     dict/LoKiNumbersDict.h
                     dict/LoKiNumbers.xml
                     LINK_LIBRARIES LoKiCoreLib
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
