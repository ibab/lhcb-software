# For running without the SPD/PRS
from Gaudi.Configuration import appendPostConfigAction
def doMyChanges():
    from Configurables import CaloElectronAlg
    ceb = CaloElectronAlg('SingleElectronRec')
    ceb.SelectionTools = [  'CaloSelectCluster/ElectronCluster'
                            # , 'CaloSelectChargedClusterWithSpd/ChargedWithSpd' # disable for upgrade nospdps samples
                            # , 'CaloSelectClusterWithPrs/ClusterWithPrs'        # disable for upgrade nospdps samples
                            , 'CaloSelectorNOT/ChargedWithTracks'     ]
def doCaloBankChanges():
    from Configurables import GaudiSequencer
    GaudiSequencer("CaloBanksHandler").Members = [ GaudiSequencer("EcalBanksHandler"), GaudiSequencer("HcalBanksHandler") ]
    #from Configurables import CaloZSupAlg
    #GaudiSequencer("CaloDigits").Members = [ CaloZSupAlg("EcalZSup"), CaloZSupAlg("HcalZSup") ]

#CaloDigitsFromRaw/SpdFromRaw, CaloDigitsFromRaw/PrsFromRaw, CaloZSupAlg/EcalZSup, CaloZSupAlg/HcalZSup

appendPostConfigAction( doMyChanges )          
appendPostConfigAction( doCaloBankChanges )
