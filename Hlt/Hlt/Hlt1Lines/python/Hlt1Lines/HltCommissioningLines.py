# =============================================================================
# $Id: HltCommissioningLines.py,v 1.3 2009-06-01 20:35:08 graven Exp $
# =============================================================================
## @file
#  Configuration of Hlt Lines for commissioning
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-12-02
# =============================================================================
"""
 script to configure Hlt lines for commissioning
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.3 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt1Line   as Line
from HltLine.HltLine import Hlt1Member as Member


class HltCommissioningLinesConf(HltLinesConfigurableUser):

   __slots__ = { 'Prescale' : { 'Hlt1Physics'  : 1
                              , 'Hlt1Random'   : 0.000001 # @OnlineEnv.AcceptRate
                              , 'Hlt1L0Forced' : 0 
                              , 'Hlt1RawBankConversion' : 0.000001
                              }
               }
   def __apply_configuration__(self):
        Line('Physics' ,  ODIN = 'ODIN_TRGTYP != LHCb.ODIN.RandomTrigger'
            , prescale = self.prescale
            , postscale = self.postscale
            )
        Line('Random' ,  ODIN = 'ODIN_TRGTYP == LHCb.ODIN.RandomTrigger'
            , prescale = self.prescale
            , postscale = self.postscale
            )
        #Line('L0Forced', L0DU = 'L0_FORCEBIT'
        #    , prescale = self.prescale
        #    , postscale = self.postscale
        #    )
        from Configurables import HltTriggerRawConversion
        Line('RawBankConversion'
            , ODIN = 'ODIN_TRGTYP != LHCb.ODIN.RandomTrigger'
            , algos = [ HltTriggerRawConversion() ]
            , prescale = self.prescale
            , postscale = self.postscale
            )
        from Configurables import HltIncidentFilter
        Line('Incident'
            , ODIN = 'ODIN_ALL'
            , algos = [ HltIncidentFilter() ]
            , prescale = self.prescale
            , postscale = self.postscale
            )
