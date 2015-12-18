################################################################################
# Package: MooreOnlineSys
################################################################################
gaudi_subdir(MooreOnlineSys v24r4)

gaudi_depends_on_subdirs(MooreScripts
                         MooreOnlineConf
                         Hlt/IndependenceTests
                         Hlt/HltMonitoring
                         Online/ZeroMQ)

gaudi_add_test(QMTest QMTEST)
