################################################################################
# Package: BooleSys
################################################################################
gaudi_subdir(BooleSys v29r11)

gaudi_depends_on_subdirs(Digi/Boole
                         GaudiConf
)

gaudi_add_test(QMTest QMTEST)
