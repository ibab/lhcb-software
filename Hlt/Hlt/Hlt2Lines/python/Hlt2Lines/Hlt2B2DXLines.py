

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2DXLinesConf(HltLinesConfigurableUser) :

    def __apply_configuration__(self) :
        from Configurables import HltANNSvc
        ###
        #   DX inclusive lines
        ## 
        ## TODO: DX lines should appear here.  The are temporarily implemented
        ##       in Hlt2TopologicalLines.py because they share some
        ##       combinatorics.
