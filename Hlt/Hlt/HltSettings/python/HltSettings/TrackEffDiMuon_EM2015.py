from GaudiKernel.SystemOfUnits import GeV, mm, MeV

class TrackEffDiMuon_EM2015 :
    """
    Threshold settings for Hlt2 TrackEffDiMuon lines: EM 2015

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author C. Linn
    @date 2015-05-19
    """

    __all__ = ( 'ActiveHlt2Lines' )


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2TrackEffDiMuonMuonTTPlusTaggedTurbo',
            'Hlt2TrackEffDiMuonMuonTTMinusTaggedTurbo',
            'Hlt2TrackEffDiMuonVeloMuonPlusTaggedTurbo',
            'Hlt2TrackEffDiMuonVeloMuonMinusTaggedTurbo',
            'Hlt2TrackEffDiMuonDownstreamPlusTaggedTurbo',
            'Hlt2TrackEffDiMuonDownstreamMinusTaggedTurbo',
            'Hlt2TrackEffDiMuonMuonTTPlusMatchedTurbo',
            'Hlt2TrackEffDiMuonMuonTTMinusMatchedTurbo',
            'Hlt2TrackEffDiMuonVeloMuonPlusMatchedTurbo',
            'Hlt2TrackEffDiMuonVeloMuonMinusMatchedTurbo',
            'Hlt2TrackEffDiMuonDownstreamPlusMatchedTurbo',
            'Hlt2TrackEffDiMuonDownstreamMinusMatchedTurbo',
        ]


        return lines

    def Thresholds(self) :

        d = {}

        from Hlt2Lines.TrackEffDiMuon.Lines     import TrackEffDiMuonLines
        d.update ({TrackEffDiMuonLines : { 
                 'Prescale' :  {},
                 'Common'   :  {'TisTosSpec'    : "Hlt1TrackMuonDecision%TOS",
                                'L0Filter'      : "L0_CHANNEL('Muon') | L0_CHANNEL('DiMuon')",
                                'OverlapTT'     : 0.4,
                                'OverlapIT'     : 0.4,
                                'OverlapOT'     : 0.4,
                                'OverlapMuon'   : 0.4,
                                'OverlapVelo'   : 0.5},
                 'MuonTT'   :  {'TagMuonID'     : 2,
                                'TagP'          : 10 * GeV,
                                'TagPt'         : 1300 * MeV,
                                'TagTrChi2'     : 5,
                                'TagMinIP'      : 0 * mm,
                                'TagMinIPchi2'  : 0, #dummy
                                'ProbeP'        : 0 * GeV,
                                'ProbePt'       : 0 * MeV,
                                'ProbeTrChi2'   : 9999, #dummy
                                'JPsiPt'        : 1000 * MeV,
                                'JPsiDOCA'      : 9999 * mm, #dummy
                                'JPsiMaxIP'     : 1.0 * mm,
                                'JPsiMaxIPchi2' : 9999, #dummy
                                'JPsiVtxChi2'   : 2,
                                'JPsiMassWin'   : 500 * MeV},
                 'VeloMuon' :  {'TagMuonID'     : 1,
                                'TagP'          : 7 * GeV,
                                'TagPt'         : 0 * MeV,
                                'TagTrChi2'     : 3,
                                'TagMinIP'      : 0 * mm,
                                'TagMinIPchi2'  : 0, #dummy
                                'ProbeP'        : 0 * GeV,
                                'ProbePt'       : 0 * MeV,
                                'ProbeTrChi2'   : 5,
                                'JPsiPt'        : 500 * MeV,
                                'JPsiDOCA'      : 9999 * mm, #dummy
                                'JPsiMaxIP'     : 9999 * mm, #dummy
                                'JPsiMaxIPchi2' : 9999, #dummy
                                'JPsiVtxChi2'   : 2,
                                'JPsiMassWin'   : 500 * MeV},
                 'Downstream': {'TagMuonID'     : 2,
                                'TagP'          : 5 * GeV,
                                'TagPt'         : 700 * MeV,
                                'TagTrChi2'     : 10,
                                'TagMinIP'      : 0.5 * mm,
                                'TagMinIPchi2'  : 0, #dummy
                                'ProbeP'        : 3 * GeV,
                                'ProbePt'       : 200 * MeV,
                                'ProbeTrChi2'   : 10,
                                'JPsiPt'        : 0 * MeV,
                                'JPsiDOCA'      : 5 * mm,
                                'JPsiMaxIP'     : 9999 * mm, #dummy
                                'JPsiMaxIPchi2' : 9999, #dummy
                                'JPsiVtxChi2'   : 5,
                                'JPsiMassWin'   : 200 * MeV}
               }
        })

        return d
