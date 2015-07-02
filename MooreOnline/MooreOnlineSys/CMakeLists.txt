################################################################################
# Package: MooreOnlineSys
################################################################################
gaudi_subdir(MooreOnlineSys v23r7p3)

gaudi_depends_on_subdirs(MooreScripts
                         MooreOnlineConf)


gaudi_add_test(QMTest QMTEST)
