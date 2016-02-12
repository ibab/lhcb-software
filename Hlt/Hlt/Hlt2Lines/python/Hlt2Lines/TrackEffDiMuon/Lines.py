#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Set of Hlt2-lines for tracking efficiency studies:
#
#  - Combination of long 'tag' track and partically reconstructed 'probe'
#  - Matching of probe with long tracks
#
#  @author Michael KOLPIN michael.kolpin@cern.ch
# =============================================================================
""" Set of Hlt2-lines for tracking efficiency studies:

 - Combination of long 'tag' track and partically reconstructed 'probe'
 - Matching of probe with long tracks
"""
# =============================================================================
__author__  = 'Michael Kolpin'
__date__    = '$Date: 2015-03-13$'
__version__ = '$Revision: 0.1 $'

from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class TrackEffDiMuonLines(Hlt2LinesConfigurableUser):
    __slots__ = {'Prescale' :  {},
                 'Common'   :  {'TisTosSpec'	: "Hlt1TrackMuonDecision%TOS",
			        'L0Filter'	: "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')",
                                'Hlt1Filter'    : None,
				'OverlapTT'	: 0.4,
				'OverlapIT'	: 0.4,
				'OverlapOT'	: 0.4,
				'OverlapMuon'	: 0.4,
				'OverlapVelo'	: 0.5},
		 'MuonTT'   :  {'TagMuonID'	: 2,
			        'TagP'		: 10 * GeV,
			        'TagPt'		: 1300 * MeV,
			        'TagTrChi2'	: 5,
			        'TagMinIP'	: 0 * mm,
			        'TagMinIPchi2'	: 0, #dummy
			        'ProbeP'	: 5 * GeV,
			        'ProbePt'	: 500 * MeV,
			        'ProbeTrChi2'	: 9999, #dummy
			        'JPsiPt'	: 1000 * MeV,
			        'JPsiDOCA'	: 9999 * mm, #dummy
			        'JPsiMaxIP'	: 0.8 * mm,
			        'JPsiMaxIPchi2'	: 9999, #dummy
			        'JPsiVtxChi2'	: 2,
			        'JPsiMassWin'	: 500 * MeV,
			        'StatHighPT'	: 2600 * MeV,
			        'StatLowETA'	: 2.7,
			        'StatHighETA'	: 4.2
                    },
		 'VeloMuon' :  {'TagMuonID'	: 1,
			        'TagP'		: 7 * GeV,
			        'TagPt'		: 0 * MeV,
			        'TagTrChi2'	: 3,
			        'TagMinIP'	: 0.2 * mm,
			        'TagMinIPchi2'	: 0, #dummy
			        'ProbeP'	: 5 * GeV,
			        'ProbePt'	: 500 * MeV,
			        'ProbeTrChi2'	: 5,
			        'JPsiPt'	: 500 * MeV,
			        'JPsiDOCA'	: 9999 * mm, #dummy
			        'JPsiMaxIP'	: 9999 * mm, #dummy
			        'JPsiMaxIPchi2'	: 9999, #dummy
			        'JPsiVtxChi2'	: 2,
			        'JPsiMassWin'	: 500 * MeV,
			        'StatHighPT'	: 2600 * MeV,
			        'StatLowETA'	: 2.7,
			        'StatHighETA'	: 4.2
                    },
		 'Downstream': {'TagMuonID'	: 2,
			        'TagP'		: 5 * GeV,
			        'TagPt'		: 700 * MeV,
			        'TagTrChi2'	: 10,
			        'TagMinIP'	: 0.5 * mm,
			        'TagMinIPchi2'	: 0, #dummy
			        'ProbeP'	: 5 * GeV,
			        'ProbePt'	: 500 * MeV,
			        'ProbeTrChi2'	: 10,
			        'JPsiPt'	: 0 * MeV,
			        'JPsiDOCA'	: 5 * mm,
			        'JPsiMaxIP'	: 9999 * mm, #dummy
			        'JPsiMaxIPchi2'	: 9999, #dummy
			        'JPsiVtxChi2'	: 5,
			        'JPsiMassWin'	: 200 * MeV,
			        'StatHighPT'	: 2600 * MeV,
			        'StatLowETA'	: 2.7,
			        'StatHighETA'	: 4.2
                    }
                }

    def stages(self, nickname = ""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages

        from Stages import JPsiCombiner, N3BodyCombiner
	#second arg gives mode = Plus/Minus refers to the charge of the Probe Muon
        self._stages = {'MuonTTPlusLowStatTagged'               : [JPsiCombiner(1, 'MuonTT', 'low')],
                        'MuonTTMinusLowStatTagged'              : [JPsiCombiner(0, 'MuonTT', 'low')],
                        'VeloMuonPlusLowStatTagged'             : [JPsiCombiner(1, 'VeloMuon', 'low')],
                        'VeloMuonMinusLowStatTagged'            : [JPsiCombiner(0, 'VeloMuon', 'low')],
                        'DownstreamPlusLowStatTagged'           : [JPsiCombiner(1, 'Downstream', 'low')],
                        'DownstreamMinusLowStatTagged'          : [JPsiCombiner(0, 'Downstream', 'low')],
                        'MuonTTPlusHighStatTagged'              : [JPsiCombiner(1, 'MuonTT', 'high')],
                        'MuonTTMinusHighStatTagged'             : [JPsiCombiner(0, 'MuonTT', 'high')],
                        'VeloMuonPlusHighStatTagged'            : [JPsiCombiner(1, 'VeloMuon', 'high')],
                        'VeloMuonMinusHighStatTagged'           : [JPsiCombiner(0, 'VeloMuon', 'high')],
                        'DownstreamPlusHighStatTagged'          : [JPsiCombiner(1, 'Downstream', 'high')],
                        'DownstreamMinusHighStatTagged'         : [JPsiCombiner(0, 'Downstream', 'high')],
                        'MuonTTPlusLowStatMatched'              : [JPsiCombiner(1, 'MuonTT', 'low'), N3BodyCombiner(1, 'MuonTT', 'low')],
                        'MuonTTMinusLowStatMatched'             : [JPsiCombiner(0, 'MuonTT', 'low'), N3BodyCombiner(0, 'MuonTT', 'low')],
                        'VeloMuonPlusLowStatMatched'            : [JPsiCombiner(1, 'VeloMuon', 'low'), N3BodyCombiner(1, 'VeloMuon', 'low')],
                        'VeloMuonMinusLowStatMatched'           : [JPsiCombiner(0, 'VeloMuon', 'low'), N3BodyCombiner(0, 'VeloMuon', 'low')],
                        'DownstreamPlusLowStatMatched'          : [JPsiCombiner(1, 'Downstream', 'low'), N3BodyCombiner(1, 'Downstream', 'low')],
                        'DownstreamMinusLowStatMatched'         : [JPsiCombiner(0, 'Downstream', 'low'), N3BodyCombiner(0, 'Downstream', 'low')],
                        'MuonTTPlusHighStatMatched'             : [JPsiCombiner(1, 'MuonTT', 'high'), N3BodyCombiner(1, 'MuonTT', 'high')],
                        'MuonTTMinusHighStatMatched'            : [JPsiCombiner(0, 'MuonTT', 'high'), N3BodyCombiner(0, 'MuonTT', 'high')],
                        'VeloMuonPlusHighStatMatched'           : [JPsiCombiner(1, 'VeloMuon', 'high'), N3BodyCombiner(1, 'VeloMuon', 'high')],
                        'VeloMuonMinusHighStatMatched'          : [JPsiCombiner(0, 'VeloMuon', 'high'), N3BodyCombiner(0, 'VeloMuon', 'high')],
                        'DownstreamPlusHighStatMatched'         : [JPsiCombiner(1, 'Downstream', 'high'), N3BodyCombiner(1, 'Downstream', 'high')],
                        'DownstreamMinusHighStatMatched'        : [JPsiCombiner(0, 'Downstream', 'high'), N3BodyCombiner(0, 'Downstream', 'high')]
                        }
        if nickname:
            return self._stages[nickname]
        else:
            return self._stages

    def __apply_configuration__(self):

        stages = self.stages()
        from HltLine.HltLine import Hlt2Line
        for (nickname, algos) in self.algorithms(stages):
            linename = 'TrackEffDiMuon' + nickname + 'TurboCalib'
            Hlt2Line(linename, prescale = self.prescale,
                     algos = algos, postscale = self.postscale,
                     L0DU = self.getProp('Common').get('L0Filter'),
                     HLT1 = self.getProp('Common').get('Hlt1Filter'),
                     Turbo = True)
        for (nickname, algos) in self.algorithms(stages):
            linename = 'TrackEffDiMuon' + nickname + 'Fullstream'
            Hlt2Line(linename, prescale = self.prescale,
                     algos = algos, postscale = self.postscale,
                     L0DU = self.getProp('Common').get('L0Filter'),
                     HLT1 = self.getProp('Common').get('Hlt1Filter'),
                     Turbo = False)
