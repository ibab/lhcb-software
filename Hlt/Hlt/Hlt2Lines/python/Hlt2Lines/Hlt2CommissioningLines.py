# $Id: Hlt2CommissioningLines.py,v 1.27 2010-08-18 14:57:39 graven Exp $
# =============================================================================
## @file
#  Configuration of Hlt Lines for commissioning 
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2009-08-11
# =============================================================================
"""
 script to configure Hlt2 lines for commissioning
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.27 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class Hlt2CommissioningLinesConf(HltLinesConfigurableUser):

   __slots__ = { 'Prescale' : { 'Hlt2PassThrough'  : 0.0001 
                              , 'Hlt2Forward'      : 0.0001
                              , 'Hlt2DebugEvent'   : 0.0001
                              }
               , 'Postscale' : { 'Hlt2ErrorEvent' : 'RATE(1)' }
               }
   def __apply_configuration__(self):
        from HltLine.HltLine import Hlt2Line   as Line
        Line('PassThrough' ,  HLT1 = "HLT_PASS_RE('^Hlt1(?!Lumi).*Decision$')"
            , VoidFilter = '' # make sure we DO NOT get a filter thrown on top of us!!!
            , prescale = self.prescale
            , postscale = self.postscale
            )
        Line('Transparent' ,  HLT1 = "HLT_PASS_RE('^Hlt1(ODIN.*|L0.*|MB.*|BeamGas.*|Velo.*|NZS.*|Incident|Tell1Error|ErrorEvent)Decision$')"
            , VoidFilter = '' # make sure we DO NOT get a filter thrown on top of us!!!
            , prescale = self.prescale
            , postscale = self.postscale
            )
        Line('Lumi' ,  HLT1 = "HLT_PASS_SUBSTR('Hlt1Lumi')"
            , VoidFilter = '' # make sure we DO NOT get a filter thrown on top of us!!!
            , prescale = self.prescale
            , postscale = self.postscale
            )

        from Configurables import HltCopySelection_LHCb__Track_ as HltCopyTrackSelection
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()
        Line('Forward', prescale = self.prescale, postscale = self.postscale
            , algos = [ tracks,
                        HltCopyTrackSelection( 'Hlt2ForwardDecision' 
                                             , InputSelection = 'TES:'+tracks.outputSelection() # careful! needs info from Hlt2Tracking!!
                                             )
                      ]
            )
        from Configurables import HltIncidentGenerator, HltSelReportsMaker
        HltSelReportsMaker().DebugIncident = 'RequestDebugEvent'
        Line('DebugEvent', prescale = self.prescale, postscale = self.postscale
            , HLT1 = "HLT_PASS_RE('^Hlt1(?!Lumi).*Decision$')" # do not want debug events on lumi-exclusive Hlt1 events...
            , algos = [ HltIncidentGenerator('Hlt2DebugEventDecision'
                                            , Incident = HltSelReportsMaker().DebugIncident
                                            )
                      ]
            )

        Line('ErrorEvent',prescale = self.prescale, postscale = self.postscale
            , HLT1 = "HLT_COUNT_ERRORBITS_RE('^Hlt2.*',0xffff) > 0" # HLT_ERRORBITS(0xffff) would be nice... don't want to count...
            , VoidFilter = '' 
            , priority = 254
            )

        from Configurables import HltANNSvc
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2PassThroughDecision"     : 50011
                                            , "Hlt2TransparentDecision"     : 50012
                                            , "Hlt2ForwardDecision"         : 50013
                                            , "Hlt2DebugEventDecision"      : 50014 
                                            , "Hlt2ErrorEventDecision"       : 50015
                                            } )

