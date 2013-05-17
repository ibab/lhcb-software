# =============================================================================
## @file
#  Cosmics 
#  @author Stephan Nies & Jesko Merkel 
#  @date 2009-09-02
# =============================================================================
"""
 script to configure Hlt lines for commissioning
"""
# =============================================================================
__author__  = "Stephan Nies & Jesko Merkel " 
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.5 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class Hlt1CosmicLinesConf(HltLinesConfigurableUser):
 
    __slots__ = { 'Prescale' : { }
                }
    def __apply_configuration__(self):

        from HltLine.HltDecodeRaw import DecodeOT
        from HltLine.HltLine import Hlt1Line as Line
        from Configurables import HltCosmicsOT
        Line('CosmicOT'  
            , prescale = self.prescale
            , postscale = self.postscale
            , algos = [ DecodeOT, HltCosmicsOT('Hlt1CosmicOTDecision', OutputSelection = 'Hlt1CosmicOTDecision') ]
            )
