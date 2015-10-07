## --------------------------------------------------------------------------------
## Lines for modes without a reconstructible decay vertex
## Ie, B+ -> K+ pi0 and B0 -> K0 pi0
## Author: Jason Andrews, jea@umd.edu
## --------------------------------------------------------------------------------
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, mm

## Dict for each stage, plus 'Common'
cuts = {'Common'       : {'NTRACK_MAX':300,
                          'TRACK_TRCHI2DOF_MAX'  :3.0,
                          'TRACK_TRGHOSTPROB_MAX':0.5,
                          'L0FILTER'  :"L0_CHANNEL('Photon')|L0_CHANNEL('Electron')",
                          'PI0_TISTOS':'L0(Photon|Electron).*Decision%TOS'},

        'FilteredPi0s' : {'PI0_PT_MIN':3500 * MeV,
                          'PI0_P_MIN' :5000 * MeV },

        'FilteredKaons': {'TRACK_TISTOS'    :'Hlt1TrackMVADecision%TOS',
                          'TRACK_PT_MIN'    :1200 * MeV,
                          'TRACK_P_MIN'     :12000 * MeV,
                          'TRACK_IPCHI2_MIN':50,
                          'TRACK_PIDK_MIN'  :-0.5 },

        'FilteredKSs'  : {'KS0_TISTOS':None,
                          'KS0_PT_MIN':500 * MeV,
                          'KS0_P_MIN' :8000 * MeV,
                          'KS0_ADMASS':15 * MeV,
                          'KS0_VCHI2PDOF_MAX':15,
                          'KS0_IPCHI2_MIN'   :10},

        'B2Kpi0'       : {'HLT1FILTER' :"HLT_PASS('Hlt1TrackMVADecision')",
                          'MASS_MIN'   :4000, ## units (MeV) are in cut string
                          'MASS_MAX'   :6200, ## units (MeV) are in cut string
                          'ASUM_PT_MIN':6500 * MeV,
                          'PT_MIN'     :5000 * MeV,
                          'MTDOCACHI2_MAX':8.0 },

        'B2K0pi0'      : {'HLT1FILTER' :"HLT_PASS_RE('Hlt1(Two)?TrackMVADecision')",
                          'MASS_MIN'   :4000, ## units (MeV) are in cut string
                          'MASS_MAX'   :6200, ## units (MeV) are in cut string
                          'ASUM_PT_MIN':5000 * MeV,
                          'PT_MIN'     :4000 * MeV,
                          'MTDOCACHI2_MAX':10.0 },
        }

class B2Kpi0Lines(Hlt2LinesConfigurableUser):
    __slots__ = cuts
    def __apply_configuration__(self) :
        from Stages import ( TrackGEC, KaonFilter, KS0Filter,
                             Pi0TOSFilter, Pi0Filter, Hb2XNeutralCombiner )
        from Inputs import Hlt2LooseKaons, KsLL

        filteredPi0s  = Pi0Filter('FilteredPi0s', [Pi0TOSFilter()])
        filteredKaons = KaonFilter('FilteredKaons', [Hlt2LooseKaons],
                                   self.getProp('FilteredKaons').get('TRACK_TISTOS'))
        filteredKSs   = KS0Filter('FilteredKSs', [KsLL],
                                  self.getProp('FilteredKSs').get('KS0_TISTOS'))

        B2Kpi0  = Hb2XNeutralCombiner('B2Kpi0', '[B+ -> K+ pi0]cc',
                                      [filteredKaons, filteredPi0s])
        B2K0pi0 = Hb2XNeutralCombiner('B2K0pi0','[B0 -> KS0 pi0]cc',
                                      [filteredKSs, filteredPi0s])

        from HltTracking.HltPVs import PV3D
        stages = {'B2Kpi0' : [TrackGEC(), PV3D('Hlt2'), B2Kpi0],
                  'B2K0pi0': [TrackGEC(), PV3D('Hlt2'), B2K0pi0],
                  }
        from HltLine.HltLine import Hlt2Line
        for (linename, algos) in self.algorithms(stages):
            Hlt2Line(linename,
                     algos=algos,
                     prescale=self.prescale,
                     postscale=self.postscale,
                     L0DU=self.getProp('Common').get('L0FILTER'),
                     HLT1=self.getProp(linename).get('HLT1FILTER'))
