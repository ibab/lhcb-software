"""
Hlt2 lines for studying Bottomonium -> DiHadrons
Including the following lines:
Hlt2BottomoniumDiKstar
Hlt2BottomoniumDiPhi
"""
__author__  = "Simone Stracka, simone.stracka@unipi.it"

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class BottomoniumLines(Hlt2LinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'DiKstar': { 'Prescale': 1.0,
                              'L0Req'   : "(L0_DATA('Spd(Mult)') < 600 )",
                              'Hlt1Req' : "HLT_PASS_RE('^Hlt1Bottomonium2KstarKstarDecision$')", 
                              'DauCuts' : """
                                            (INTREE( ('K+'==ABSID) & (PT > 1.*GeV)
                                            & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK > 0.)))
                                            & (INTREE( ('pi+'==ABSID) & (PT > 1.*GeV)
                                            & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK < 0.)))
                                            & (PT > 2.*GeV)
                                            & (VFASPF(VCHI2PDOF) < 9)
                                            & (ADMASS('K*(892)0') < 75.*MeV)
                                             """, 
                              'ComCuts' : """
                                            (in_range( 8.8*GeV, AM, 10.1*GeV))
                                            & (APT> 1.9 *GeV) 
                                            """, 
                              'MomCuts' : """
                                            (VFASPF(VCHI2PDOF) < 9)
                                            & (in_range( 8.9*GeV, MM, 10.0*GeV))
                                            & (PT> 2.0 *GeV) 
                                            """                          
                              },
                 'DiPhi' :  { 'Prescale': 1.0,
                              'L0Req'   : "(L0_DATA('Spd(Mult)') < 600 )",
                              'Hlt1Req' : "HLT_PASS_RE('^Hlt1Bottomonium2PhiPhiDecision$')", 
                              'DauCuts' : """
                                            (INTREE( ('K+'==ID) & (PT > 0.65*GeV)
                                            & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK > 0.)))
                                            & (INTREE( ('K-'==ID) & (PT > 0.65*GeV)
                                            & (P > 3.*GeV) & (TRCHI2DOF < 3.) & (PIDK > 0.)))
                                            & (PT > 1.2*GeV)
                                            & (VFASPF(VCHI2PDOF) < 9)
                                            & (ADMASS('phi(1020)') < 30.*MeV)
                                            """,
                              'ComCuts' : """
                                            (in_range( 8.8*GeV, AM, 10.7*GeV))
                                            & (APT> 0.9 *GeV) 
                                            """,
                              'MomCuts' : """
                                            (VFASPF(VCHI2PDOF) < 9)
                                            & (in_range( 8.9*GeV, MM, 10.6*GeV))
                                            & (PT> 1.0 *GeV) 
                                            """  
                              }                 
                 }


    def stages(self, nickname = ""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages

        from Stages import DiKstarCombiner, DiPhiWideCombiner
        self._stages = {                                               
            'DiKstar'         : [DiKstarCombiner('DiKstar')],
            'DiPhi'           : [DiPhiWideCombiner('DiPhi')]
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
            Hlt2Line('Bottomonium' + nickname,
                     prescale = cuts[nickname].get('Prescale', 1.0), 
                     L0DU     = cuts[nickname].get('L0Req',    None),
                     HLT1     = cuts[nickname].get('Hlt1Req',  None),
                     algos = algos, postscale = self.postscale)

        for nickname, algos in self.algorithms(stages): 
            Hlt2Line('Bottomonium' + nickname + 'Turbo',
                     prescale = cuts[nickname].get('Prescale', 1.0), 
                     L0DU     = cuts[nickname].get('L0Req',    None),
                     HLT1     = cuts[nickname].get('Hlt1Req',  None),
                     algos = algos,
                     postscale = self.postscale,
                     Turbo = True )



