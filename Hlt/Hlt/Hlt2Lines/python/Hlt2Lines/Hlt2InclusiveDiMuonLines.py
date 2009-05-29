


from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusiveDiMuonLinesConf(HltLinesConfigurableUser) :

    def __apply_configuration__(self) :
        from Configurables import HltANNSvc
        ###
        #    ID 50200 - heavy dimuon
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineUnbiasedDiMuon.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedDiMuonDecision" : 50200 } )
        ###
        #    ID 50450 - lifetime unbiased JPsi
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineUnbiasedJPsi.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2UnbiasedJPsiDecision" : 50450 } )
        ###
        #    ID 50040 - biased dimuon 
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineBiasedDiMuon.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonDecision" : 50040 } )
