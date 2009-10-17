# =============================================================================
# $Id: HltCosmicLines.py,v 1.2 2009-10-17 16:57:30 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.2 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class HltCosmicLinesConf(HltLinesConfigurableUser):
 
    __slots__ = { 'Prescale' : { }
                }
    def __apply_configuration__(self):

        from HltLine.HltDecodeRaw import DecodeOT
        from HltLine.HltLine import Hlt1Line as Line
        from Configurables import HltCosmicsOT
        Line('CosmicOT'  
            , prescale = self.prescale
            , postscale = self.postscale
            , L0DU = "L0_ALL"
            , algos = [ DecodeOT, HltCosmicsOT('Hlt1CosmicOTDecision', OutputSelection = '%Decision') ]
            )
