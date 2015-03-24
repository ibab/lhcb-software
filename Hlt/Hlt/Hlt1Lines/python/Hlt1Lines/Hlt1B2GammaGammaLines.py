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
    __slots__ = {'GAMMA_PT_MIN'             : 3000,     # MeV
                 'B_SUMPT_MIN'              : 6000,     # MeV
                 'B_PT_MIN'                 : 3000,     # MeV
                 'B_MASS_MIN'               : 3500,     # MeV
                 'B_MASS_MAX'               : 7500,     # MeV
                }

    def hlt1B2GammaGammaLine_Preambulo(self, props):
        from HltTracking.Hlt1Tracking import L0CaloCandidates

        preambulo = [L0CaloCandidates('Photon'),
                     "from LoKiArrayFunctors.decorators import ASUM, AM",
                     "from LoKiPhys.decorators import PT, ALL",
                     "from LoKiArrayFunctors.decorators import APT, ADAMASS, ACUTDOCA",
                     "CombinationConf = LoKi.Hlt1.Hlt1CombinerConf( 'B_s0 -> gamma gamma' \
                                         , (APT>%(B_PT_MIN)s*MeV) & (in_range(%(B_MASS_MIN)s*MeV, AM, %(B_MASS_MAX)s*MeV)) & (ASUM(PT) > %(B_SUMPT_MIN)s*MeV)\
                                         , ALL, 'MomentumCombiner')" % props]
        return preambulo

    def hlt1B2GammaGammaLine_Streamer(self, name, props) :
        from Configurables import LoKi__HltUnit as HltUnit
        props['name'] = name

        PhotonUnitLineCode = """
        L0CaloCandidates
        >>  tee  ( monitor( TC_SIZE > 0, '# pass CaloCandidates', LoKi.Monitoring.ContextSvc ) )
        >>  tee  ( monitor( TC_SIZE    , 'nCaloCandidates' , LoKi.Monitoring.ContextSvc ) )
        >>  TC_FROMCALOTOPARTICLES( 'gamma',  '', (PT>%(GAMMA_PT_MIN)s*MeV) )
        >>  tee ( monitor( TC_SIZE > 0, '# pass ToPhotons', LoKi.Monitoring.ContextSvc ) )
        >>  tee ( monitor( TC_SIZE    , 'nPhotons',         LoKi.Monitoring.ContextSvc ) )
        >>  SINK ('Hlt1B2GammaGammaPhotons')
        >>  ~TC_EMPTY
        """ % props

        BUnitLineCode = """
        TC_HLT1COMBINER('', 
                        CombinationConf,
                        'Hlt1B2GammaGammaPhotons',
                        ALL)
        >>  tee ( monitor( TC_SIZE > 0, '# pass SV', LoKi.Monitoring.ContextSvc ) )
        >>  tee ( monitor( TC_SIZE    , 'nSV',       LoKi.Monitoring.ContextSvc ) )
        >>  SINK ('Hlt1B2GammaGammaDecision')
        >>  ~TC_EMPTY
        """ % props

        from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
        from Configurables import LoKi__HltUnit as HltUnit
        from HltTracking.HltPVs import PV3D

        hlt1B2GammaGammaLine_PhotonUnit = HltUnit('Hlt1%sPhotonUnit' % name,
                                                  #OutputLevel=1,
                                                  Monitor=True,
                                                  Preambulo=self.hlt1B2GammaGammaLine_Preambulo(props),
                                                  Code=PhotonUnitLineCode)

        hlt1B2GammaGammaLine_BUnit = HltUnit('Hlt1%sUnit' % name,
                                             PVSelection="PV3D",
                                             #OutputLevel=1,
                                             Monitor=True,
                                             Preambulo=self.hlt1B2GammaGammaLine_Preambulo(props),
                                             Code=BUnitLineCode)

        return [Hlt1GECUnit('Loose'),
                PV3D('Hlt1'),
                hlt1B2GammaGammaLine_PhotonUnit,
                hlt1B2GammaGammaLine_BUnit]

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt1Line

        Hlt1Line('B2GammaGamma',
                 prescale=self.prescale,
                 postscale=self.postscale,
                 L0DU="L0_CHANNEL('Photon') | L0_CHANNEL('Electron')",
                 algos=self.hlt1B2GammaGammaLine_Streamer("B2GammaGamma", self.getProps()))

# EOF

