################################################################################
# Package: HltL0Conf
################################################################################
gaudi_subdir(HltL0Conf v4r3)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Det/CaloDet
                         Det/MuonDet
                         Event/L0Event
                         Event/LinkerInstances
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiKernel
                         Hlt/HltBase
                         Tf/TsaKernel)

gaudi_add_module(HltL0Conf
                 src/*.cpp
                 LINK_LIBRARIES CaloUtils CaloDetLib MuonDetLib L0Event RecEvent TrackEvent GaudiKernel HltBase TsaKernel)

gaudi_env(SET HLTLOCONFOPTS \${HLTL0CONFROOT}/options)


gaudi_add_test(QMTest QMTEST)
