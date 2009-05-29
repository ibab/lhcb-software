

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2DXLinesConf(HltLinesConfigurableUser) :

    def __apply_configuration__(self) :
        from Configurables import HltANNSvc
        ###
        #   DX inclusive lines
        ## 
        ## TODO: Hlt2LinesDX.py should disappear, and its contents should appear _here_
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineDX.py" )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DX3BodyCopKsDDDecision" : 62000 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DX4BodyCopKsDDDecision" : 62010 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DX3BodyCopDecision" : 62020 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DX4BodyCopDecision" : 62030 } )

