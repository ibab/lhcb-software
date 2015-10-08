from GaudiKernel.SystemOfUnits import GeV, mm, MeV

class TrackEffDiMuon_25ns_August2015 :
    """
    Threshold settings for Hlt2 TrackEffDiMuon lines: proton-Argon run 2015

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author M. Kolpin
    @date 2015-10-08
    """

    __all__ = ( 'ActiveHlt2Lines' )


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2TrackEffDiMuonMuonTTPlusTaggedFullstream',
            'Hlt2TrackEffDiMuonMuonTTMinusTaggedFullstream',
            'Hlt2TrackEffDiMuonVeloMuonPlusTaggedFullstream',
            'Hlt2TrackEffDiMuonVeloMuonMinusTaggedFullstream',
            'Hlt2TrackEffDiMuonDownstreamPlusTaggedFullstream',
            'Hlt2TrackEffDiMuonDownstreamMinusTaggedFullstream',
            'Hlt2TrackEffDiMuonMuonTTPlusMatchedFullstream',
            'Hlt2TrackEffDiMuonMuonTTMinusMatchedFullstream',
            'Hlt2TrackEffDiMuonVeloMuonPlusMatchedFullstream',
            'Hlt2TrackEffDiMuonVeloMuonMinusMatchedFullstream',
            'Hlt2TrackEffDiMuonDownstreamPlusMatchedFullstream',
            'Hlt2TrackEffDiMuonDownstreamMinusMatchedFullstream',
            'Hlt2TrackEffDiMuonMuonTTPlusTaggedTurboCalib',
            'Hlt2TrackEffDiMuonMuonTTMinusTaggedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonPlusTaggedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonMinusTaggedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamPlusTaggedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamMinusTaggedTurboCalib',
            'Hlt2TrackEffDiMuonMuonTTPlusMatchedTurboCalib',
            'Hlt2TrackEffDiMuonMuonTTMinusMatchedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonPlusMatchedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonMinusMatchedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamPlusMatchedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamMinusMatchedTurboCalib',
        ]


        return lines

    def Thresholds(self) :

        d = {}

        from Hlt2Lines.TrackEffDiMuon.Lines     import TrackEffDiMuonLines
        d.update ({TrackEffDiMuonLines : { 
                 'Prescale' :  {},
                 'Common'   :  {'TisTosSpec'    : "Hlt1TrackMuonDecision%TOS",
                                'L0Filter'      : "L0_CHANNEL('Muon')",
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
                                'JPsiMaxIP'     : 0.8 * mm,
                                'JPsiMaxIPchi2' : 9999, #dummy
                                'JPsiVtxChi2'   : 2,
                                'JPsiMassWin'   : 500 * MeV},
                 'VeloMuon' :  {'TagMuonID'     : 1,
                                'TagP'          : 7 * GeV,
                                'TagPt'         : 0 * MeV,
                                'TagTrChi2'     : 3,
                                'TagMinIP'      : 0.2 * mm,
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
