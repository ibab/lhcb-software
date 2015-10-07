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
			        'ProbeP'	: 0 * GeV,
			        'ProbePt'	: 0 * MeV,
			        'ProbeTrChi2'	: 9999, #dummy
			        'JPsiPt'	: 1000 * MeV,
			        'JPsiDOCA'	: 9999 * mm, #dummy
			        'JPsiMaxIP'	: 0.8 * mm,
			        'JPsiMaxIPchi2'	: 9999, #dummy
			        'JPsiVtxChi2'	: 2,
			        'JPsiMassWin'	: 500 * MeV},
		 'VeloMuon' :  {'TagMuonID'	: 1,
			        'TagP'		: 7 * GeV,
			        'TagPt'		: 0 * MeV,
			        'TagTrChi2'	: 3,
			        'TagMinIP'	: 0.2 * mm,
			        'TagMinIPchi2'	: 0, #dummy
			        'ProbeP'	: 0 * GeV,
			        'ProbePt'	: 0 * MeV,
			        'ProbeTrChi2'	: 5,
			        'JPsiPt'	: 500 * MeV,
			        'JPsiDOCA'	: 9999 * mm, #dummy
			        'JPsiMaxIP'	: 9999 * mm, #dummy
			        'JPsiMaxIPchi2'	: 9999, #dummy
			        'JPsiVtxChi2'	: 2,
			        'JPsiMassWin'	: 500 * MeV},
		 'Downstream': {'TagMuonID'	: 2,
			        'TagP'		: 5 * GeV,
			        'TagPt'		: 700 * MeV,
			        'TagTrChi2'	: 10,
			        'TagMinIP'	: 0.5 * mm,
			        'TagMinIPchi2'	: 0, #dummy
			        'ProbeP'	: 3 * GeV,
			        'ProbePt'	: 200 * MeV,
			        'ProbeTrChi2'	: 10,
			        'JPsiPt'	: 0 * MeV,
			        'JPsiDOCA'	: 5 * mm,
			        'JPsiMaxIP'	: 9999 * mm, #dummy
			        'JPsiMaxIPchi2'	: 9999, #dummy
			        'JPsiVtxChi2'	: 5,
			        'JPsiMassWin'	: 200 * MeV}
                }

    def stages(self, nickname = ""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages

        from Stages import JPsiCombiner, N3BodyCombiner
	#second arg gives mode = Plus/Minus refers to the charge of the Probe Muon
        self._stages = {'MuonTTPlusTagged'              : [JPsiCombiner(1, 'MuonTT')],
                        'MuonTTMinusTagged'             : [JPsiCombiner(0, 'MuonTT')],
                        'VeloMuonPlusTagged'            : [JPsiCombiner(1, 'VeloMuon')],
                        'VeloMuonMinusTagged'           : [JPsiCombiner(0, 'VeloMuon')],
                        'DownstreamPlusTagged'          : [JPsiCombiner(1, 'Downstream')],
                        'DownstreamMinusTagged'         : [JPsiCombiner(0, 'Downstream')],
                        'MuonTTPlusMatched'             : [JPsiCombiner(1, 'MuonTT'), N3BodyCombiner(1, 'MuonTT')],
                        'MuonTTMinusMatched'            : [JPsiCombiner(0, 'MuonTT'), N3BodyCombiner(0, 'MuonTT')],
                        'VeloMuonPlusMatched'           : [JPsiCombiner(1, 'VeloMuon'), N3BodyCombiner(1, 'VeloMuon')],
                        'VeloMuonMinusMatched'          : [JPsiCombiner(0, 'VeloMuon'), N3BodyCombiner(0, 'VeloMuon')],
                        'DownstreamPlusMatched'         : [JPsiCombiner(1, 'Downstream'), N3BodyCombiner(1, 'Downstream')],
                        'DownstreamMinusMatched'        : [JPsiCombiner(0, 'Downstream'), N3BodyCombiner(0, 'Downstream')]
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
                     L0DU = self.getProp('Common').get('L0Filter'), Turbo = True)
