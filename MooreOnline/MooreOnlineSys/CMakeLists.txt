################################################################################
# Package: MooreOnlineSys
################################################################################
gaudi_subdir(MooreOnlineSys v25r0)

gaudi_depends_on_subdirs(MooreScripts
                         MooreOnlineConf
                         Hlt/IndependenceTests
                         Hlt/HltMonitoring
                         )

gaudi_add_test(QMTest QMTEST)
