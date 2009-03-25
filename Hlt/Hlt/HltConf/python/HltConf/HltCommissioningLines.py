# =============================================================================
# $Id: HltCommissioningLines.py,v 1.5 2009-03-25 08:38:54 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.5 $"
# =============================================================================

from HltConf.HltLinesConfigurableUser import *
from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import Hlt1Member as Member
from Configurables   import HltIncidentFilter


class HltCommissioningLinesConf(HltLinesConfigurableUser):

   __slots__ = { 'Prescale' : { 'Physics'  : 1 # update default prescales for these lines
                              , 'Random'   : 1 # @OnlineEnv.AcceptRate
                              , 'L0Forced' : 0 
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
        Line('RawBankConversion'
            , ODIN = 'ODIN_TRGTYP != LHCb.ODIN.RandomTrigger'
            , algos = [ HltTriggerRawConversion() ]
            , prescale = self.prescale
            , postscale = self.postscale
            )
        Line('Incident'
            , ODIN = 'ODIN_ALL'
            , algos = [ HltIncidentFilter() ]
            , prescale = self.prescale
            , postscale = self.postscale
            )
