from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import \
    Hlt2LinesConfigurableUser
class Run1TopoLines():
    def slots(self):
        from GaudiKernel.SystemOfUnits import MeV, picosecond, mm
        return {
        'Common' : {
            # TISTOS cuts.
            'RUN1_TisTosSpec'          : 'Hlt1Track(?!MVA).*Decision%TOS',
            # Electron and radiative filters.
            'RUN1_L0_ELECTRON_FILTER'  : "L0_CHANNEL('Electron')",
            'RUN1_HLT1_ELECTRON_FILT'  : ("HLT_PASS_RE('Hlt1(Track|.*Electron)."
                                          "*Decision')"),
            'RUN1_L0_RAD_FILTER'       : ("L0_CHANNEL('Electron') | L0_CHANNEL"
                                          "('Photon')"),
            'RUN1_RAD_TOS'             : True,
            # Particle cuts.
            'RUN1_ALL_PT_MIN'          : 500.0 * MeV,
            'RUN1_ALL_P_MIN'           : 5000.0 * MeV,
            'RUN1_ALL_MIPCHI2DV_MIN'   : 4.0,
            'RUN1_E_TRCHI2DOF_MAX'     : 2.5,
            'RUN1_MU_TRCHI2DOF_MAX'    : 2.5,
            'RUN1_HAD_TRCHI2DOF_MAX'   : 2.5,
            'RUN1_PIDE_MIN'            : -2.0,
            # V0 usage flags.
            'RUN1_USE_KS'              : True,
            'RUN1_USE_LAMBDA'          : True,
            # Combo cuts.
            'RUN1_AMAXDOCA_MAX'        : 0.2 * mm,
            'RUN1_BPVVDCHI2_MIN'       : 100.0,
            'RUN1_MIN_TRCHI2DOF_MAX'   : 2,
            'RUN1_ONETRACK_PT_MIN'     : 1500.0 * MeV,
            'RUN1_MU_ONETRACK_PT_MIN'  : 1000.0 * MeV,
            'RUN1_ONETRACK_IPCHI2_MIN' : 16,
            'RUN1_V2BODYCHI2_MAX'      : 10,
            'RUN1_NV0_2Body_MAX'       : 2,
            'RUN1_NV0_3Body_MAX'       : 1,
            'RUN1_NV0_4Body_MAX'       : 0,
            'RUN1_MIN_V0_LT'           : 20 * picosecond,
            # BDT response cuts.
            'RUN1_BDT_2BODY_MIN'       : 0.4,
            'RUN1_BDT_3BODY_MIN'       : 0.4,
            'RUN1_BDT_4BODY_MIN'       : 0.3,
            'RUN1_BDT_2BODYMU_MIN'     : 0.1,
            'RUN1_BDT_3BODYMU_MIN'     : 0.1,
            'RUN1_BDT_4BODYMU_MIN'     : 0.1,
            'RUN1_BDT_2BODYE_MIN'      : 0.1,
            'RUN1_BDT_3BODYE_MIN'      : 0.1,
            'RUN1_BDT_4BODYE_MIN'      : 0.1,
            'RUN1_BDT_2BODYRAD_MIN'    : 0.1,
            'RUN1_BDT_3BODYRAD_MIN'    : 0.1,
            'RUN1_BDT_4BODYRAD_MIN'    : 0.1,
            # BDT parameter file versions.
            'RUN1_BDT_2BODY_PARAMS'    : 'v1r0',
            'RUN1_BDT_3BODY_PARAMS'    : 'v1r0',
            'RUN1_BDT_4BODY_PARAMS'    : 'v1r0',
            'RUN1_VARMAP'              : {
                "M"          :  "MM/MeV",
                "DOCA"       :  "DOCAMAX_('', False)/mm",
                "CANDIPCHI2" :  "BPVIPCHI2()",
                "MCOR"       :  "BPVCORRM",
                "FDCHI2"     :  "BPVVDCHI2",
                "PT"         :  "PT",
                "PTMIN"      :  "MINTREE(ISBASIC, PT)/MeV",
                "PTSUM"      :  "SUMTREE(PT, ISBASIC, 0.0)/MeV"},
            # GEC.
            'RUN1_USE_GEC'             : True,
            'RUN1_GEC_MAX'             : 500,
            # Simple cuts.
            'RUN1_SIMPLE_2BODY_CUTS'   : [
                {'PTSUM': (7000, -1), 'FDCHI2': (1000, -1), 
                 'DOCA': (-1, 0.2), 'M': (2500, 7000)}],
            'RUN1_SIMPLE_3BODY_CUTS'   : [
                {'PTSUM': (8000, -1), 'FDCHI2': (1000, -1), 
                 'DOCA': (-1, 0.2), 'M': (3000, 7000)}],
            'RUN1_SIMPLE_4BODY_CUTS'   : [
                {'PTSUM': (9000, -1), 'FDCHI2': (1000, -1), 
                 'DOCA': (-1, 0.2), 'M': (3500, 7000)}]
            }
        }

    def stages(self, props, nickname = ''):
        if hasattr(self, '_stages') and self._stages:
            if nickname: return self._stages[nickname]
            else: return self._stages
        else: self._stages = {}

        # Filter the particle input.
        varmap = props['RUN1_VARMAP']
        from Inputs import (BiKalmanFittedKaonsWithMuonID, 
                            BiKalmanFittedKaonsWithEID, KsLLTF, KsDD,
                            LambdaLLTrackFitted, LambdaDDTrackFitted)
        from Run1Stages import FilterParts
        mparts = [FilterParts('Kaon', 'MU', [BiKalmanFittedKaonsWithMuonID],
                              props['RUN1_USE_GEC'])]
        eparts = [FilterParts('Kaon', 'E', [BiKalmanFittedKaonsWithEID],
                              props['RUN1_USE_GEC'])]
        if props['RUN1_USE_KS']:
            mparts.append(FilterParts('KsLL', 'MU', [KsLLTF], 
                                      props['RUN1_USE_GEC']))
            mparts.append(FilterParts('KsDD', 'MU', [KsDD], 
                                      props['RUN1_USE_GEC']))
        if props['RUN1_USE_LAMBDA']:
            mparts.append(FilterParts('LambdaLL', 'MU', [LambdaLLTrackFitted],
                                      props['RUN1_USE_GEC']))
            mparts.append(FilterParts('LambdaDD', 'MU', [LambdaDDTrackFitted],
                                      props['RUN1_USE_GEC']))

        # Build the primary stages.
        from Run1Stages import (FilterMforN, FilterNforN, 
                            FilterSimple, FilterBDT, CombineN)
        minputs = []; mcombos = []; einputs = []; ecombos = []
        for n in xrange(2, 5):
            mcombos.append(CombineN('All', n, mparts if n == 2 else 
                                        mparts + [minputs[n - 3]]))
            ecombos.append(CombineN('E', n, eparts if n == 2 else 
                                        eparts + [einputs[n - 3]]))
            mfiltered = FilterNforN('All', n, [mcombos[-1]])
            efiltered = FilterNforN('E', n, [ecombos[-1]])
            if n < 4:
                minputs.append(FilterMforN('All', n, n + 1, [mcombos[-1]]))
                einputs.append(FilterMforN('E', n, n + 1, [ecombos[-1]]))
            self._stages['Run1Topo%iBodySimple' % n] = [
                FilterSimple(n, [mfiltered], varmap, props)]
            self._stages['Run1Topo%iBodyBBDT' % n] = [
                FilterBDT('', n, [mfiltered], varmap, props)]
            self._stages['Run1TopoMu%iBodyBBDT' % n] = [
                FilterBDT('MU', n, [mfiltered], varmap, props)]
            self._stages['Run1TopoE%iBodyBBDT' % n] = [
                FilterBDT('E', n, [efiltered], varmap, props)]

        # Build the radiative stages.
        from Inputs import BiKalmanFittedPhotonsFromL0
        combos = CombineN('RAD', 3, [minputs[0], BiKalmanFittedPhotonsFromL0])
        self._stages['Run1TopoRad2BodyBBDT'] = [
            FilterBDT('RAD', 2, [mcombos[0]], varmap, props)]
        self._stages['Run1TopoRad2plus1BodyBBDT'] = [
            FilterBDT('RAD', 3, [combos], varmap, props)]

        # Return the stages.
        if nickname: return self._stages[nickname]
        else: return self._stages
