from Run1Lines import Run1TopoLines
from GaudiKernel.SystemOfUnits import MeV, picosecond, mm
Run1Topo = Run1TopoLines()
slots = Run1Topo.slots()
slots['Common'].update({
        'TOS'               : ('Hlt1(Two)?Track(MVA)?(Muon)?'
                               'Decision%TOS'),
        'MUTOS'             : 'Hlt1TrackMuonDecision%TOS',
        'HTOS'              : 'Hlt1(Two)?TrackMVADecision%TOS',
        'GEC_MAX'           : 500,
        'USE_KS'            : True,
        'USE_LAMBDA'        : True,
        'BDT_PREFILTER'     : 0.2,
        'BDT_2BODY_MIN'     : 0.997,
        'BDT_3BODY_MIN'     : 0.995,
        'BDT_4BODY_MIN'     : 0.993,
        'MU_BDT_2BODY_MIN'  : 0.993,
        'MU_BDT_3BODY_MIN'  : 0.99,
        'MU_BDT_4BODY_MIN'  : 0.99,
        'BDT_MIN'           : 0.99,
        'MU_BDT_MIN'        : 0.99,
        'BDT_VARMAP'        : {
            "n"      : "NINTREE((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0'))",
            "mcor"   : "BPVCORRM",
            "chi2"   : "VFASPF(VCHI2)",
            "sumpt"  : ("SUMTREE(PT,((ABSID=='K+')|(ID=='KS0')|"
                        "(ABSID=='Lambda0')), 0.0)/MeV"),
            "eta"    : "BPVETA",
            "fdchi2" : "BPVVDCHI2",
            "minpt"  : ("MINTREE(((ABSID=='K+')|(ID=='KS0')|"
                        "(ABSID=='Lambda0')),PT)/MeV"),
            "nlt16"  : ("NINTREE((ABSID == 'K+') & (BPVIPCHI2() < 16))"),
            "ipchi2" : "BPVIPCHI2()",
            "n1trk"  : ("NINTREE(((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0'))"
                        "& (PT > 1*GeV) & (BPVIPCHI2() > 16))")
        },
        'BDT_PARAMS'        : 'hlt2_topo_run2_v1.bbdt',
        'TRK_PT_MIN'        : 200 * MeV,
        'TRK_P_MIN'         : 3000 * MeV,
        'TRK_CHI2_MAX'      : 3,
        'TRK_IPCHI2_MIN'    : 4,
        'TRK_LT_MIN'        : 0 * picosecond,
        'CMB_TRK_NLT16_MAX' : 2,
        'CMB_PRT_PT_MIN'    : 2000 * MeV,
        'CMB_VRT_DIRA_MIN'  : 0,
        'CMB_VRT_VDCHI2_MIN': 16,
        'CMB_VRT_CHI2_MAX'  : 1000,
        'CMB_VRT_ETA_MIN'   : 2,
        'CMB_VRT_ETA_MAX'   : 5,
        'CMB_VRT_MCOR_MIN'  : 1000 * MeV,
        'CMB_VRT_MCOR_MAX'  : 10000 * MeV
        })

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import \
    Hlt2LinesConfigurableUser
class TopoLines(Hlt2LinesConfigurableUser):
    from copy import deepcopy
    __slots__ = deepcopy(slots)

    def stages(self, nickname = ''):
        if hasattr(self, '_stages') and self._stages:
            if nickname: return self._stages[nickname]
            else: return self._stages
        else: self._stages = {}

        # Create the Run 1 stages.
        props = self.getProps()['Common']
        self._stages.update(Run1Topo.stages(props))

        # Filter the particle input.
        from Inputs import (BiKalmanFittedKaonsWithMuonID, KsLLTF, KsDD,
                            LambdaLLTrackFitted, LambdaDDTrackFitted)
        from Stages import FilterParts, FilterParts4
        gec   = props['GEC_MAX'] >= 0
        parts = [FilterParts('Kaon', [BiKalmanFittedKaonsWithMuonID], gec)]
        if props['USE_KS']:
            parts.append(FilterParts('KsLL', [KsLLTF], gec))
            parts.append(FilterParts('KsDD', [KsDD], gec))
        if props['USE_LAMBDA']: 
            parts.append(FilterParts('LambdaLL', [LambdaLLTrackFitted], gec))
            parts.append(FilterParts('LambdaDD', [LambdaDDTrackFitted], gec))
        parts4 = [FilterParts4([BiKalmanFittedKaonsWithMuonID], gec)]

        # Create the stages.
        from Stages import CombineTos, Combine3, Combine4, FilterMVA
        combos2  = [CombineTos(parts)]
        combos2f = [FilterMVA(2,combos2,props,props['BDT_PREFILTER'])]
        combos3  = [Combine3(combos2f+parts)]
        combos3f = [FilterMVA(3,combos3,props,props['BDT_PREFILTER'])]
        combos4  = [Combine4(combos3f+parts4)]

        self._stages['Topo2BodyCombos'] = combos2
        self._stages['Topo3BodyCombos'] = combos3
        self._stages['Topo4BodyCombos'] = combos4
        for n in xrange(2, 5):
            self._stages['Topo%iBody' % n] = [
                FilterMVA(n, self._stages['Topo%iBodyCombos' % n], props)]
            self._stages['TopoMu%iBody' % n] = [
                FilterMVA(n, self._stages['Topo%iBodyCombos' % n], props,
                          props['MU_BDT_%iBODY_MIN' % n],True)]

        # Return the stages. 
        if nickname: return self._stages[nickname]
        else: return self._stages

    def __apply_configuration__(self):
        from HltLine.HltLine import Hlt2Line
        for (name, algos) in self.algorithms(self.stages()).iteritems():
            if 'Combos' in name: continue
            if 'Run1TopoE' in name:
                l0   = self.getProps()['Common']['RUN1_L0_ELECTRON_FILTER']
                hlt1 = self.getProps()['Common']['RUN1_HLT1_ELECTRON_FILT']
            else: l0 = None; hlt1 = None
            Hlt2Line(name, prescale = self.prescale, algos = algos,
                     L0DU = l0, HLT1 = hlt1, postscale = self.postscale)
