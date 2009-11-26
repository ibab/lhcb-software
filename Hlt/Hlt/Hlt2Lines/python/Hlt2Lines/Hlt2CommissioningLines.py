# =============================================================================
# $Id: Hlt2CommissioningLines.py,v 1.13 2009-11-26 10:49:12 panmanj Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.13 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class Hlt2CommissioningLinesConf(HltLinesConfigurableUser):

   __slots__ = { 'Prescale' : { 'Hlt2PassThrough'  : 0.0001 
                              , 'Hlt2Forward'      : 0. # first needs a check that /Hlt/Track/Long actually exists..
                              , 'Hlt2DebugEvent'   : 0.0001
                              }
               }
   def __apply_configuration__(self):
        from HltLine.HltLine import Hlt2Line   as Line
        Line('PassThrough' ,  HLT = "HLT_PASS_RE('^Hlt1(?!Lumi).*Decision$')"
            , prescale = self.prescale
            , postscale = self.postscale
            , PV = False
            )
        Line('Transparent' ,  HLT = "HLT_PASS_RE('^Hlt1(ODIN.*|L0.*|Lumi.*|Bias.*|Velo.*|Align.*|Incident|Tell1Error)Decision$')"
            , prescale = self.prescale
            , postscale = self.postscale
            , PV = False
            )

        from Configurables import HltCopySelection_LHCb__Track_ as HltCopyTrackSelection
        from Configurables import Hlt2PID
        tracks = Hlt2PID().hlt2Tracking()
        Line('Forward', prescale = self.prescale, postscale = self.postscale
            , algos = [ tracks,
                        HltCopyTrackSelection( 'Hlt2ForwardDecision' 
                                             , InputSelection = 'TES:/Hlt/Track/Long' # careful! needs info from HltLine.HltReco!!
                                             )
                      ]
            , PV = False
            )
        from Configurables import HltIncidentGenerator, HltSelReportsMaker
        HltSelReportsMaker().DebugIncident = 'RequestDebugEvent'
        Line('DebugEvent', prescale = self.prescale, postscale = self.postscale
            , HLT = "HLT_PASS_RE('^Hlt1(?!Lumi).*Decision$')" # do not want debug events on lumi-exclusive Hlt1 events...
            , algos = [ HltIncidentGenerator('Hlt2DebugEventDecision'
                                            , Incident = HltSelReportsMaker().DebugIncident
                                            )
                      ]
            , PV = False
            )

        from Configurables import HltANNSvc
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2PassThroughDecision"     : 50011
                                            , "Hlt2TransparentDecision"     : 50012
                                            , "Hlt2ForwardDecision"         : 50013
                                            , "Hlt2DebugEventDecision"      : 50014 } )

