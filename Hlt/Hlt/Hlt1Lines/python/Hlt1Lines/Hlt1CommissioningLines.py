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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.4 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class Hlt1CommissioningLinesConf(HltLinesConfigurableUser):

   __slots__ = { 'Prescale' : { 'Hlt1ODINPhysics'    : 0.000001
                              , 'Hlt1ODINTechnical'  : 0.000001 # @OnlineEnv.AcceptRate
                              , 'Hlt1Tell1Error'     : 0
                              }
               , 'TechnicalTrigger' : 'TechnicalTrigger'
               }
   def __apply_configuration__(self):
        from HltLine.HltLine import Hlt1Line   as Line
        Line('ODINPhysics' ,  ODIN = 'ODIN_TRGTYP == LHCb.ODIN.PhysicsTrigger'
            , prescale = self.prescale
            , postscale = self.postscale
            )
        Line('ODINTechnical' ,  ODIN = 'ODIN_TRGTYP == LHCb.ODIN.%s' % self.getProp('TechnicalTrigger')
            , prescale = self.prescale
            , postscale = self.postscale
            )
        from Configurables import FilterByBankType
        Line('Tell1Error'
            , ODIN='ODIN_ALL'
            , algos = [ FilterByBankType('Hlt1Tell1ErrorDecision' 
                                        , PassSelectedEvents = True
                                        , BankNames = [ ".*Error" ] 
                                        )
                      ]
            , prescale = self.prescale
            , postscale = self.postscale
            )
        from Configurables import HltIncidentFilter
        Line('Incident'
            , ODIN = 'ODIN_ALL'
            , algos = [ HltIncidentFilter('Hlt1IncidentDecision') ]
            , prescale = self.prescale
            , postscale = self.postscale
            , priority = 254
            )
