"""
Hlt2 lines for studying hadroproudction of Charmonium -> DiHadrons.
Including the following lines:
Hlt2DiProton
Hlt2DiProtonLowMult
Hlt2DiPhi
"""
__author__  = "Jibo He, Jibo.He@cern.ch"

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CcDiHadronLines(Hlt2LinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'DiProton' : { 'Prescale': 1.0,
                                'L0Req'   : "(L0_DATA('Spd(Mult)') < 300 )",
                                'Hlt1Req' : "HLT_PASS('Hlt1DiProtonDecision')",
                                'DauCuts' : """
                                            (PT > 1.9*GeV)
                                            & (TRCHI2DOF < 3)
                                            & (PIDp > 20)
                                            & ((PIDp-PIDK) > 10)
                                            """,
                                'ComCuts' : """
                                            (in_range( 2.75*GeV, AM, 4.1*GeV))
                                            & (APT> 6.3 *GeV)
                                            """,
                                'MomCuts' : """
                                            (VFASPF(VCHI2PDOF) < 9)
                                            & (in_range( 2.8*GeV, MM, 4.0*GeV))
                                            & (PT> 6.5 *GeV)
                                            """
                                },
                 'DiProtonLowMult' : { 'Prescale': 1.0,
                                'L0Req'   : "(L0_DATA('Spd(Mult)') < 10 )",
                                'Hlt1Req' : "HLT_PASS('Hlt1DiProtonLowMultDecision')",
                                'DauCuts' : """
                                            (PT > 0.5*GeV)
                                            & (TRCHI2DOF < 3)
                                            & (PIDp > 10)
                                            & ((PIDp-PIDK) > 5)
                                            """,
                                'ComCuts' : """
                                            (AM > 2.75*GeV )
                                            """,
                                'MomCuts' : """
                                            (VFASPF(VCHI2PDOF) < 9)
                                            & (MM > 2.8*GeV )
                                            """
                                },
                 'DiPhi' :    { 'Prescale': 1.0,
                                'L0Req'   : "(L0_DATA('Spd(Mult)') < 600 )",
                                'Hlt1Req' : None,
                                'DauCuts' : """
                                            (INTREE( ('K+'==ID) & (PT > 0.65*GeV)
                                            & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK > 0.)))
                                            & (INTREE( ('K-'==ID) & (PT > 0.65*GeV)
                                            & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK > 0.)))
                                            & (PT > 0.8*GeV)
                                            & (VFASPF(VCHI2PDOF) < 9)
                                            & (ADMASS('phi(1020)') < 20.*MeV)
                                             """,
                                'ComCuts' : """
                                            (in_range( 2.75*GeV, AM, 4.85*GeV))
                                            & (APT> 1.9 *GeV)
                                            """,
                                'MomCuts' : """
                                            (VFASPF(VCHI2PDOF) < 9)
                                            & (in_range( 2.8*GeV, MM, 4.8*GeV))
                                            & (PT> 2.0 *GeV)
                                            """
                                }
                 }


    def stages(self, nickname = ""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages

        from Stages import DiProtonCombiner, DiPhiCombiner
        self._stages = {'DiProton'          : [DiProtonCombiner('DiProton')],
                        'DiProtonLowMult'   : [DiProtonCombiner('DiProtonLowMult')],
                        'DiPhi'             : [DiPhiCombiner('DiPhi')]
                        }
        if nickname:
            return self._stages[nickname]
        else:
            return self._stages


    def __apply_configuration__(self):
        from HltLine.HltLine import Hlt2Line

        stages = self.stages()
        cuts   = self.getProps()
        for nickname, algos in self.algorithms(stages):
            Hlt2Line(nickname,
                     prescale = cuts[nickname].get('Prescale', 1.0),
                     L0DU     = cuts[nickname].get('L0Req',    None),
                     HLT1     = cuts[nickname].get('Hlt1Req',  None),
                     algos = algos, postscale = self.postscale)
