
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusiveMuonLinesConf(HltLinesConfigurableUser) :

    def __apply_configuration__(self) :
        from Configurables import HltANNSvc
        ###
        #    ID 50440 -  selection for a large PT single muon
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineSingleHighPTMuon.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleHighPTMuonDecision" : 50440 } )
        ###
        #    ID 50400 -  selection for inclusive decays with mu + track
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineIncMuTrack.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrackDecision" : 50400 } )
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineIncMuTrackNoPT.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrackNoPTDecision" : 50401 } )
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineIncMuTrack4JPsi.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrack4JpsiDecision" : 50402 } )
        ###
        #    ID 50190 - single muon
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineSingleMuon.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonDecision" : 50190 } )
