from GaudiKernel.SystemOfUnits import GeV, mm, MeV

class TrackEffDiMuon_25ns_Draft2016 :
    """
    Threshold settings for Hlt2 TrackEffDiMuon lines: draft for 2016 data taking

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author M. Kolpin
    @date 2016-02-12
    """

    __all__ = ( 'ActiveHlt2Lines' )


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2TrackEffDiMuonMuonTTPlusLowStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonMuonTTMinusLowStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonPlusLowStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonMinusLowStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamPlusLowStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamMinusLowStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonMuonTTPlusHighStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonMuonTTMinusHighStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonPlusHighStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonMinusHighStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamPlusHighStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamMinusHighStatTaggedTurboCalib',
            'Hlt2TrackEffDiMuonMuonTTPlusLowStatMatchedTurboCalib',
            'Hlt2TrackEffDiMuonMuonTTMinusLowStatMatchedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonPlusLowStatMatchedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonMinusLowStatMatchedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamPlusLowStatMatchedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamMinusLowStatMatchedTurboCalib',
            'Hlt2TrackEffDiMuonMuonTTPlusHighStatMatchedTurboCalib',
            'Hlt2TrackEffDiMuonMuonTTMinusHighStatMatchedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonPlusHighStatMatchedTurboCalib',
            'Hlt2TrackEffDiMuonVeloMuonMinusHighStatMatchedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamPlusHighStatMatchedTurboCalib',
            'Hlt2TrackEffDiMuonDownstreamMinusHighStatMatchedTurboCalib',
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
                                'ProbeP'        : 5 * GeV,
                                'ProbePt'       : 500 * MeV,
                                'ProbeTrChi2'   : 9999, #dummy
                                'JPsiPt'        : 1000 * MeV,
                                'JPsiDOCA'      : 9999 * mm, #dummy
                                'JPsiMaxIP'     : 0.8 * mm,
                                'JPsiMaxIPchi2' : 9999, #dummy
                                'JPsiVtxChi2'   : 2,
                                'JPsiMassWin'   : 500 * MeV,
                                'StatHighPT'	: 2600 * MeV,
                                'StatLowETA'	: 2.7,
                                'StatHighETA'	: 4.2
                                },
                 'VeloMuon' :  {'TagMuonID'     : 1,
                                'TagP'          : 7 * GeV,
                                'TagPt'         : 0 * MeV,
                                'TagTrChi2'     : 3,
                                'TagMinIP'      : 0.2 * mm,
                                'TagMinIPchi2'  : 0, #dummy
                                'ProbeP'        : 5 * GeV,
                                'ProbePt'       : 500 * MeV,
                                'ProbeTrChi2'   : 5,
                                'JPsiPt'        : 500 * MeV,
                                'JPsiDOCA'      : 9999 * mm, #dummy
                                'JPsiMaxIP'     : 9999 * mm, #dummy
                                'JPsiMaxIPchi2' : 9999, #dummy
                                'JPsiVtxChi2'   : 2,
                                'JPsiMassWin'   : 500 * MeV,
                                'StatHighPT'	: 2600 * MeV,
                                'StatLowETA'	: 2.7,
                                'StatHighETA'	: 4.2
                                },
                 'Downstream': {'TagMuonID'     : 2,
                                'TagP'          : 5 * GeV,
                                'TagPt'         : 700 * MeV,
                                'TagTrChi2'     : 10,
                                'TagMinIP'      : 0.5 * mm,
                                'TagMinIPchi2'  : 0, #dummy
                                'ProbeP'        : 5 * GeV,
                                'ProbePt'       : 500 * MeV,
                                'ProbeTrChi2'   : 10,
                                'JPsiPt'        : 0 * MeV,
                                'JPsiDOCA'      : 5 * mm,
                                'JPsiMaxIP'     : 9999 * mm, #dummy
                                'JPsiMaxIPchi2' : 9999, #dummy
                                'JPsiVtxChi2'   : 5,
                                'JPsiMassWin'   : 200 * MeV,
                                'StatHighPT'	: 2600 * MeV,
                                'StatLowETA'	: 2.7,
                                'StatHighETA'	: 4.2
                                }
               }
        })

        return d
