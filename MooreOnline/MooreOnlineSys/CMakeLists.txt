################################################################################
# Package: MooreOnlineSys
################################################################################
gaudi_subdir(MooreOnlineSys v24r0)

gaudi_depends_on_subdirs(MooreScripts
                         MooreOnlineConf
                         Hlt/IndependenceTests
                         Hlt/HltMonitoring
						 Online/ZeroMQ
						 Online/Hlt2Monitoring)

gaudi_add_test(QMTest QMTEST)
