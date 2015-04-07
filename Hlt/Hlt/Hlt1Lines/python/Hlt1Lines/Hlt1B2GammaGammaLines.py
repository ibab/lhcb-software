# =============================================================================
## @@file
#  Configuration of B -> gamma gamma
#  Get two photons and cut on their mass
#  @@author Albert Puig
#  @@date 2015-03-17
# =============================================================================
"""Script to configure the B->gamma gamma line"""


from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt1B2GammaGammaLinesConf(HltLinesConfigurableUser) :
    __slots__ = {'GAMMA_PT_MIN'             : 3500,     # MeV
                 'B_SUMPT_MIN'              : 8000,     # MeV
                 'B_PT_MIN'                 : 2000,     # MeV
                 'B_MASS_MIN'               : 3500,     # MeV
                 'B_MASS_MAX'               : 6000,     # MeV
                }

    def hlt1B2GammaGammaLine_Preambulo(self, props):
        from HltTracking.Hlt1Tracking import L0CaloCandidates

        preambulo = [L0CaloCandidates('DiPhoton')]
        return preambulo

    def hlt1B2GammaGammaLine_Streamer(self, name, props) :
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name

        DiPhotonUnitLineCode = """
        L0CaloCandidates
        >>  tee  ( monitor( TC_SIZE > 0, '# pass CaloCandidates', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nCaloCandidates' , LoKi.Monitoring.ContextSvc ) )
        >>  TC_HLT1DIPHOTONMAKER( 'B_s0',  '', %(GAMMA_PT_MIN)s, %(B_SUMPT_MIN)s, %(B_MASS_MIN)s, %(B_MASS_MAX)s, %(B_PT_MIN)s )
        >>  tee ( monitor( TC_SIZE > 0, '# pass DiPhoton', LoKi.Monitoring.ContextSvc ) )
        >>  tee ( monitor( TC_SIZE    , 'nDiPhotons',         LoKi.Monitoring.ContextSvc ) )
        >>  SINK ('Hlt1B2GammaGammaDecision')
        >>  ~TC_EMPTY
        """ % props

        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.HltPVs import PV3D

        hlt1B2GammaGammaLine_DiPhotonUnit = HltUnit('Hlt1%sDiPhotonUnit' % name,
                                                    #OutputLevel=1,
                                                    Monitor=True,
                                                    Preambulo=self.hlt1B2GammaGammaLine_Preambulo(props),
                                                    Code=DiPhotonUnitLineCode)


        return [Hlt1GECUnit('Loose'),
                PV3D('Hlt1'),
                hlt1B2GammaGammaLine_DiPhotonUnit]

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt1Line

        Hlt1Line('B2GammaGamma',
                 prescale=self.prescale,
                 postscale=self.postscale,
                 L0DU="L0_CHANNEL('Photon') | L0_CHANNEL('Electron')",
                 algos=self.hlt1B2GammaGammaLine_Streamer("B2GammaGamma", self.getProps()))

# EOF

