# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Implementation of Hlt2-lines devoted to charmless 2-body B decays.
#  Two kind of lines are present:
#    1) An inclusive line of all the modes done under the pi+pi- final 
#       state hypothesis. This line use cuts that introduce an acceptance in
#       the decay-time.
#    2) A set of exclusive lines for all the modes. This stage
#       don't use cuts that may introduce an acceptance in the decay-time.
#       In order to achieve this the usage of PID variables is necessary.
#
#
#  @author Stefano Perazzini Stefano.Perazzini@cern.ch
#  @date   2015-03-13
#  
#                    $Revision:$
#  Last modification $Date:$
#  by                $Author:$
# =============================================================================
""" Implementation of Hlt2-lines devoted to charmless 2-body B decays.
    Two kind of lines are present:
      1) An inclusive line of all the modes done under the pi+pi- final 
         state hypothesis. This line use cuts that introduce an acceptance in
         the decay-time.
      2) A set of exclusive lines for all the modes. This stage
         don't use cuts that may introduce an acceptance in the decay-time.
         In order to achieve this the usage of PID variables is necessary.
"""
__version__ = "$Revision: $"
__author__  = "Stefano Perazzini Stefano.Perazzini@cern.ch"
__date__    = "2015-03-13"
__all__     = ( 'B2HHLines'  )

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class B2HHLines(Hlt2LinesConfigurableUser):
    __slots__ = {'Prescale' : {},
                 'Common'   : { 'TrChi2'         : 3,    
                                'PionPT'         : 1000 * MeV, 
                                'SumPT'          : 4500 * MeV,  
                                'DOCACHI2'       : 9,   
                                'BIPCHI2'        : 9,  
                                'BDIRA'          : 0.99,  
                                'BPT'            : 1200 * MeV },

                 'B2HH'     : { 'TisTosSpec'     : "Hlt1.*Track.*Decision%TOS",
                                'BMassWinLow'    : 4700 * MeV,
                                'BMassWinHigh'   : 6200 * MeV,
                                'PionIPCHI2'     : 16,
                                'BFDCHI2'        : 100, },

                 'B2PiPi'   : { 'TisTosSpec'     : "Hlt1B2HH_LTUNB_PiPiDecision%TOS",
                                'BMassWinLow'    : 4900 * MeV,
                                'BMassWinHigh'   : 5900 * MeV,
                                'PidCutPions'    : -5,
                                'PidCutKaons'    : 2,
                                'PidCutProtons'  : 5,
                                'BLT'            : 0.3 * picosecond  },

                 'B2KPi'    : { 'TisTosSpec'     : "Hlt1B2HH_LTUNB_KPiDecision%TOS",
                                'BMassWinLow'    : 4900 * MeV,
                                'BMassWinHigh'   : 5900 * MeV,
                                'PidCutPions'    : -5,
                                'PidCutKaons'    : 2,  
                                'PidCutProtons'  : 5,
                                'BLT'            : 0.3 * picosecond },

                 'B2KK'     : { 'TisTosSpec'     : "Hlt1B2HH_LTUNB_KKDecision%TOS",
                                'BMassWinLow'    : 4900 * MeV,
                                'BMassWinHigh'   : 5900 * MeV,
                                'PidCutPions'    : -5,
                                'PidCutKaons'    : 2,
                                'PidCutProtons'  : 5, 
                                'BLT'            : 0.3 * picosecond },

                 'Lb2PK'    : { 'BMassWinLow'    : 5100 * MeV,
                                'BMassWinHigh'   : 6200 * MeV,
                                'PidCutPions'    : -5,
                                'PidCutKaons'    : 2,
                                'PidCutProtons'  : 5,
                                'BLT'            : 0.3 * picosecond },

                 'Lb2PPi'   : { 'BMassWinLow'    : 5100 * MeV,
                                'BMassWinHigh'   : 6200 * MeV,
                                'PidCutPions'    : -5,
                                'PidCutKaons'    : 2,
                                'PidCutProtons'  : 5,
                                'BLT'            : 0.3 * picosecond }
                }
    
    def __apply_configuration__(self):
        from Stages import SelB2HH, SelB2PiPi, SelB2KPi, SelB2KK, SelLb2PK, SelLb2PPi
        stages = { 'B2HH'   : [ SelB2HH   ],
                   'B2PiPi' : [ SelB2PiPi ],
                   'B2KPi'  : [ SelB2KPi  ],
                   'B2KK'   : [ SelB2KK   ],
                   'Lb2PK'  : [ SelLb2PK  ],
                   'Lb2PPi' : [ SelLb2PPi ] }

        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms(stages).iteritems():
            Hlt2Line(nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
