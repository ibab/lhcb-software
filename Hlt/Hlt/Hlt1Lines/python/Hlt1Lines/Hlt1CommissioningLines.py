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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.13 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class Hlt1CommissioningLinesConf(HltLinesConfigurableUser):

   __slots__ = { 'Prescale' : { 'Hlt1ODINPhysics'    : 0.000001
                              , 'Hlt1ODINTechnical'  : 0.000001 # @OnlineEnv.AcceptRate
                              , 'Hlt1Tell1Error'     : 0
                              , 'Hlt1VeloClosingMicroBias' : 1 
                              }
               , 'Postscale' : { 'Hlt1Incident'     : 'RATE(1)' 
                               , 'Hlt1ErrorEvent'   : 'RATE(1)' 
                               , 'Hlt1NZSVelo'      : 'RATE(1)'
                               , 'Hlt1GEC.*'        : 'RATE(1)'
                               , 'Hlt1VeloClosingMicroBias' : 'RATE(500)'
                               }
               , 'ODINPhysics'   : '( ODIN_TRGTYP == LHCb.ODIN.PhysicsTrigger )'
               , 'ODINTechnical' : '( ODIN_TRGTYP == LHCb.ODIN.TechnicalTrigger )'
               , 'ODINVeloClosing' : 'scale( jbit( ODIN_EVTTYP,0 ), RATE(10000) )'
               }
   def __apply_configuration__(self):
        from HltLine.HltLine import Hlt1Line   as Line
        #from Hlt1GECs import Hlt1_GEC
        #for i in [ 'VELO','IT','OT' ] :
        #    # WARNING: these imply we always decode Velo & IT 
        #    Line('GECPassThrough%s'  % i
        #        , L0DU = 'L0_DECISION_PHYSICS'
        #        , prescale = self.prescale
        #        , postscale = self.postscale
        #        , algos = [Hlt1_GEC(i,reject=False)]
        #        )
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
            , algos = [ FilterByBankType('Hlt1Tell1ErrorDecision' 
                                        , PassSelectedEvents = True
                                        , BankNames = [ ".*Error" ] 
                                        )
                      ]
            , prescale = self.prescale
            , postscale = self.postscale
            )
        Line('NZSVelo'
            , algos = [ FilterByBankType('Hlt1NZSVeloDecision' 
                                        , PassSelectedEvents = True
                                        , BankNames = [ "VeloFull" ] 
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
        from HltTracking.HltReco import MinimalVelo
        from HltLine.HltLine import Hlt1Member   as Member
        Line ( 'VeloClosingMicroBias'
                    , prescale = self.prescale
                    , ODIN = self.getProp('ODINVeloClosing')
                    , algos = [ MinimalVelo
                              , Member( 'Hlt::TrackFilter','All'
                                      , Code = [ 'TrALL' ]
                                      , InputSelection = 'TES:%s' % MinimalVelo.outputSelection()
                                      , OutputSelection = '%Decision'
                                      ) 
                              ]
                    , postscale = self.postscale
                    ) 
