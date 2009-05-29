
from Gaudi.Configuration import * 

from HltLine.HltLinesConfigurableUser import *

class Hlt2InclusivePhiLinesConf(HltLinesConfigurableUser) :

    def __apply_configuration__(self) :

         from Configurables import HltANNSvc
         ###
         #    ID 60000 - Inc Phi
         ##
         importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineInclusivePhi.py")
         HltANNSvc().Hlt2SelectionID.update( { "Hlt2LineInclusivePhiDecision" : 50015 } ) 

