## --------------------------------------------------------------------------------
## Lines for decays with out a decay vertex
## Eg, B+ -> K+ pi0, Lambda_b -> Lambda0 Gamma, B0 -> K0 pi0
##
## Author: Jason Andrews, jea@umd.edu
## Shameless copy of Albert Puig's Radiative lines
## --------------------------------------------------------------------------------
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV

## Dict for each stage, plus 'Common'
cuts = {'Common'        : {'L0FILTER'    : 'Photon,Electron',
                           'TrackTisTos' : 'Hlt1Track.*Decision%TOS',
                           'PhotonTisTos': 'L0(Photon|Electron).*Decision%TOS',
                           'NTRACK_MAX'  : 300}, ## 300 in Rad vs 500 in topo

        ## let's leave the track alone if we can, PID excepted
        'FilteredKaons'  : {'TRACK_TRCHI2DOF_MAX': 3.0,
                            'TRACK_PT_MIN': 1200 * MeV, ## All=1250, Photon=1200
                            'TRACK_P_MIN' : 3000 * MeV, ## All=6000, Photon=6000
                            'TRACK_IPCHI2_MIN':13,      ## All=13, Photon=13
                            'TRACK_PIDK_MIN'  :-0.5},   ## -0.5 is safe
        
        'SharedPhotons2' : {'PHOTON_PT_MIN': 2000 * MeV,
                           'PHOTON_P_MIN' : 5000 * MeV},
        
        'B2Kpi0'        : {'MASS_MIN':4000, ## units (MeV) are in cut string
                           'MASS_MAX':7000, ## units (MeV) are in cut string
                           # 'MOTHER_COMB':"(in_range(4000*MeV, AM, 7000*MeV))",
                           'PT_MIN'  :2000 * MeV,
                           'MTDOCAChi2_MAX':25} ## could go as low as 13, nbd
        }



class VertexlessLines(Hlt2LinesConfigurableUser):
    __slots__ = cuts
    def __apply_configuration__(self) :
        from Stages import TrackGEC, KaonFilter, Hb2XGammaCombiner
        from Inputs import Hlt2LooseKaons

        filteredKaons = KaonFilter('FilteredKaons', [Hlt2LooseKaons])
        B2Kpi0 = Hb2XGammaCombiner('B2Kpi0', '[B+ -> K+ gamma]cc', filteredKaons)
        
        from HltTracking.HltPVs import PV3D
        stages = {'B2Kpi0' : [TrackGEC(), PV3D('Hlt2'), B2Kpi0]}
        from HltLine.HltLine import Hlt2Line
        for (linename, algos) in self.algorithms(stages).iteritems():
            Hlt2Line(linename,
                     algos=algos,
                     prescale=self.prescale,
                     postscale=self.postscale)
