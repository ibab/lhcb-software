from GaudiKernel.SystemOfUnits import GeV, mm, MeV, picosecond

class B2HH_25ns_August2015 :
    """
    Threshold settings for Hlt2 B2HHlines: August 2015
    """
    __all__ = ( 'ActiveHlt2Lines' )

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        lines = [
                'Hlt2B2HH',
                'Hlt2B2PiPi',
                'Hlt2B2KPi',
                'Hlt2B2KK',
                'Hlt2Lb2PK',
                'Hlt2Lb2PPi'
                ]

        return lines


    def Thresholds(self) :

        d = {}
        from Hlt2Lines.B2HH.Lines import B2HHLines
        d.update ({B2HHLines : {
                    'Prescale' : {},
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
                                           })

        return d
