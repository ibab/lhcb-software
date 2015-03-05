__author__  = 'Michael Kolpin'
__date__    = '$Date: 2015-03-03$'
__version__ = '$Revision: 0.0 $'

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
			        'ProbeP'	: 0 * GeV,
			        'ProbePt'	: 0 * MeV,
			        'ProbeTrChi2'	: 9999, #dummy
			        'JPsiPt'	: 1000 * MeV,
			        'JPsiDOCA'	: 9999 * mm, #dummy
			        'JPsiMaxIP'	: 1 * mm,
			        'JPsiVtxChi2'	: 2,
			        'JPsiMassWin'	: 500 * MeV},
		 'VeloMuon' :  {'TagMuonID'	: 1,
			        'TagP'		: 7 * GeV,
			        'TagPt'		: 0 * MeV,
			        'TagTrChi2'	: 3,
			        'TagMinIP'	: 0 * mm,
			        'ProbeP'	: 0 * GeV,
			        'ProbePt'	: 0 * MeV,
			        'ProbeTrChi2'	: 5,
			        'JPsiPt'	: 500 * MeV,
			        'JPsiDOCA'	: 9999 * mm, #dummy
			        'JPsiMaxIP'	: 9999 * mm, #dummy
			        'JPsiVtxChi2'	: 2,
			        'JPsiMassWin'	: 500 * MeV},
		 'Downstream': {'TagMuonID'	: 2,
			        'TagP'		: 5 * GeV,
			        'TagPt'		: 700 * MeV,
			        'TagTrChi2'	: 10,
			        'TagMinIP'	: 0.5 * mm,
			        'ProbeP'	: 3 * GeV,
			        'ProbePt'	: 200 * MeV,
			        'ProbeTrChi2'	: 10,
			        'JPsiPt'	: 0 * MeV,
			        'JPsiDOCA'	: 5 * mm,
			        'JPsiMaxIP'	: 9999 * mm, #dummy
			        'JPsiVtxChi2'	: 5,
			        'JPsiMassWin'	: 200 * MeV}
                }
    
    def __apply_configuration__(self):
        from Stages import JPsiCombiner, N3BodyCombiner
	#second arg gives mode = Plus/Minus refers to the charge of the Probe Muon, third arg decides if matched/unmatched
        stages ={'MuonTTPlusTagged'              : [JPsiCombiner('JPsi', 1, 'MuonTT')],
                 'MuonTTMinusTagged'             : [JPsiCombiner('JPsi', 0, 'MuonTT')],
                 'VeloMuonPlusTagged'            : [JPsiCombiner('JPsi', 1, 'VeloMuon')],
                 'VeloMuonMinusTagged'           : [JPsiCombiner('JPsi', 0, 'VeloMuon')],
                 'DownstreamPlusTagged'          : [JPsiCombiner('JPsi', 1, 'Downstream')],
                 'DownstreamMinusTagged'         : [JPsiCombiner('JPsi', 0, 'Downstream')],
                 'MuonTTPlusTaggedTurbo'         : [JPsiCombiner('JPsi', 1, 'MuonTT')],
                 'MuonTTMinusTaggedTurbo'        : [JPsiCombiner('JPsi', 0, 'MuonTT')],
                 'VeloMuonPlusTaggedTurbo'       : [JPsiCombiner('JPsi', 1, 'VeloMuon')],
                 'VeloMuonMinusTaggedTurbo'      : [JPsiCombiner('JPsi', 0, 'VeloMuon')],
                 'DownstreamPlusTaggedTurbo'     : [JPsiCombiner('JPsi', 1, 'Downstream')],
                 'DownstreamMinusTaggedTurbo'    : [JPsiCombiner('JPsi', 0, 'Downstream')],
                 'MuonTTPlusMatchedTurbo'        : [JPsiCombiner('JPsi', 1, 'MuonTT'), N3BodyCombiner('N3Body', 1, 'MuonTT')],
                 'MuonTTMinusMatchedTurbo'       : [JPsiCombiner('JPsi', 0, 'MuonTT'), N3BodyCombiner('N3Body', 0, 'MuonTT')],
                 'VeloMuonPlusMatchedTurbo'      : [JPsiCombiner('JPsi', 1, 'VeloMuon'), N3BodyCombiner('N3Body', 1, 'VeloMuon')],
                 'VeloMuonMinusMatchedTurbo'     : [JPsiCombiner('JPsi', 0, 'VeloMuon'), N3BodyCombiner('N3Body', 0, 'VeloMuon')],
                 'DownstreamPlusMatchedTurbo'    : [JPsiCombiner('JPsi', 1, 'Downstream'), N3BodyCombiner('N3Body', 1, 'Downstream')],
                 'DownstreamMinusMatchedTurbo'   : [JPsiCombiner('JPsi', 0, 'Downstream'), N3BodyCombiner('N3Body', 0, 'Downstream')]
        	}
        
        from HltLine.HltLine import Hlt2Line
        for (nickname, algos) in self.algorithms(stages).iteritems():
            linename = 'TrackEff' + nickname
            turbo = True if 'Turbo' in nickname else False
            Hlt2Line(linename, prescale = self.prescale,
                     algos = algos, postscale = self.postscale,
                     L0DU = self.getProp('Common').get('L0Filter'), Turbo = turbo)
