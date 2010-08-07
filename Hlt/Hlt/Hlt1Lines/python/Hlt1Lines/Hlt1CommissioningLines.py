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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.11 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class Hlt1CommissioningLinesConf(HltLinesConfigurableUser):

   __slots__ = { 'Prescale' : { 'Hlt1ODINPhysics'    : 0.000001
                              , 'Hlt1ODINTechnical'  : 0.000001 # @OnlineEnv.AcceptRate
                              , 'Hlt1Tell1Error'     : 0
                              }
               , 'Postscale' : { 'Hlt1Incident'     : 'RATE(1)' 
                               , 'Hlt1ErrorEvent'   : 'RATE(1)' 
                               , 'Hlt1VeloGECPassThrough' : 0.01
                               , 'Hlt1ITGECPassThrough'   : 0.01}
               , 'ODINPhysics'   : '( ODIN_TRGTYP == LHCb.ODIN.PhysicsTrigger )'
               , 'ODINTechnical' : '( ODIN_TRGTYP == LHCb.ODIN.TechnicalTrigger )'
               }
   def __apply_configuration__(self):
        from HltLine.HltLine import Hlt1Line   as Line
        from Hlt1GECs import Hlt1_IT_GEC, Hlt1_OT_GEC,Hlt1_Velo_GEC

        Line('VeloGECPassThrough' 
            , prescale = self.prescale
            , postscale = self.postscale
            , algos = [Hlt1_Velo_GEC(">")]
            )
        Line('ITGECPassThrough' 
            , prescale = self.prescale
            , postscale = self.postscale
            , algos = [Hlt1_IT_GEC(">")]
            )
        Line('OTGECPassThrough' 
            , prescale = self.prescale
            , postscale = self.postscale
            , algos = [Hlt1_OT_GEC(">")]
            )
        Line('ODINPhysics',   ODIN = self.getProp('ODINPhysics')
            , prescale = self.prescale
            , postscale = self.postscale
            )
        Line('ODINTechnical', ODIN = self.getProp('ODINTechnical')
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
            , algos = [ HltIncidentFilter('Hlt1IncidentDecision') ]
            , prescale = self.prescale
            , postscale = self.postscale
            , priority = 254
            )
        Line('ErrorEvent',prescale = self.prescale, postscale = self.postscale
            , HLT = "HLT_COUNT_ERRORBITS_RE('^Hlt1.*',0xffff) > 0" # TODO: just want presence, so want HLT_ERRORBITS(0xffff) would be nice to have...
            , priority = 254
            )
