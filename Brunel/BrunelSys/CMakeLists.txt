################################################################################
# Package: BrunelSys
################################################################################
gaudi_subdir(BrunelSys v44r2)

gaudi_depends_on_subdirs(Rec/Brunel
                         GaudiConf)

gaudi_add_test(QMTest QMTEST)
