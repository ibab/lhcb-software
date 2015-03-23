from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import \
    Hlt2LinesConfigurableUser
class TopoLines(Hlt2LinesConfigurableUser):
    from GaudiKernel.SystemOfUnits import MeV, picosecond, mm
    __slots__ = {
        'Common' : {
            # TISTOS cuts.
            'TisTosSpec'          : 'Hlt1Track.*Decision%TOS',
            # Electron and radiative filters.
            'L0_ELECTRON_FILTER'  : "L0_CHANNEL('Electron')",
            'HLT1_ELECTRON_FILT'  : ("HLT_PASS_RE('Hlt1(Track|.*Electron)."
                                     "*Decision')"),
            'L0_RAD_FILTER'       : ("L0_CHANNEL('Electron') | L0_CHANNEL"
                                     "('Photon')"),
            'RAD_TOS'             : True,
           # Particle cuts.
            'ALL_PT_MIN'          : 500.0 * MeV,
            'ALL_P_MIN'           : 5000.0 * MeV,
            'ALL_MIPCHI2DV_MIN'   : 4.0,
            'E_TRCHI2DOF_MAX'     : 2.5,
            'MU_TRCHI2DOF_MAX'    : 2.5,
            'HAD_TRCHI2DOF_MAX'   : 2.5,
            'PIDE_MIN'            : -2.0,
            # V0 usage flags (needs OR logic still).
            'USE_KS'              : False,
            'USE_LAMBDA'          : False,
            # Combo cuts.
            'AMAXDOCA_MAX'        : 0.2 * mm,
            'BPVVDCHI2_MIN'       : 100.0,
            'MIN_TRCHI2DOF_MAX'   : 2,
            'ONETRACK_PT_MIN'     : 1500.0 * MeV,
            'MU_ONETRACK_PT_MIN'  : 1000.0 * MeV,
            'ONETRACK_IPCHI2_MIN' : 16,
            'V2BODYCHI2_MAX'      : 10,
            'NV0_2Body_MAX'       : 2,
            'NV0_3Body_MAX'       : 1,
            'NV0_4Body_MAX'       : 0,
            'MIN_V0_LT'           : 20 * picosecond,
            # BDT response cuts.
            'BDT_2BODY_MIN'       : 0.4,
            'BDT_3BODY_MIN'       : 0.4,
            'BDT_4BODY_MIN'       : 0.3,
            'BDT_2BODYMU_MIN'     : 0.1,
            'BDT_3BODYMU_MIN'     : 0.1,
            'BDT_4BODYMU_MIN'     : 0.1,
            'BDT_2BODYE_MIN'      : 0.1,
            'BDT_3BODYE_MIN'      : 0.1,
            'BDT_4BODYE_MIN'      : 0.1,
            'BDT_2BODYRAD_MIN'    : 0.1,
            'BDT_3BODYRAD_MIN'    : 0.1,
            'BDT_4BODYRAD_MIN'    : 0.1,
            # BDT parameter file versions.
            'BDT_2BODY_PARAMS'    : 'v1r0',
            'BDT_3BODY_PARAMS'    : 'v1r0',
            'BDT_4BODY_PARAMS'    : 'v1r0',
            # GEC.
            'USE_GEC'             : True,
            'GEC_MAX'             : 500,
            # Simple cuts.
            'SIMPLE_2BODY_CUTS'   : [
                {'PTSUM': (7000, -1), 'FDCHI2': (1000, -1), 
                 'DOCA': (-1, 0.2), 'M': (2500, 7000)}],
            'SIMPLE_3BODY_CUTS'   : [
                {'PTSUM': (8000, -1), 'FDCHI2': (1000, -1), 
                 'DOCA': (-1, 0.2), 'M': (3000, 7000)}],
            'SIMPLE_4BODY_CUTS'   : [
                {'PTSUM': (9000, -1), 'FDCHI2': (1000, -1), 
                 'DOCA': (-1, 0.2), 'M': (3500, 7000)}]
            }
        }

    def __apply_configuration__(self):
        # The variable map for the BBDT.
        varmap = {
               "M"          :  "MM/MeV",
               "DOCA"       :  "DOCAMAX_('', False)/mm",
               "CANDIPCHI2" :  "BPVIPCHI2()",
               "MCOR"       :  "BPVCORRM",
               "FDCHI2"     :  "BPVVDCHI2",
               "PT"         :  "PT",
               "PTMIN"      :  "MINTREE(ISBASIC, PT)/MeV",
               "PTSUM"      :  "SUMTREE(PT, ISBASIC, 0.0)/MeV",
               }
        props = self.getProps()['Common']

        # Filter the particle input.
        from Inputs import (BiKalmanFittedKaonsWithMuonID, 
                            BiKalmanFittedKaonsWithEID, KsLLTF, KsDD,
                            LambdaLLTrackFitted, LambdaDDTrackFitted)
        from Stages import FilterParts
        mparts = [FilterParts('Kaon', 'MU', [BiKalmanFittedKaonsWithMuonID],
                              props['USE_GEC'])]
        eparts = [FilterParts('Kaon', 'E', [BiKalmanFittedKaonsWithEID],
                              props['USE_GEC'])]
        if props['USE_KS']:
            mparts.append(FilterParts('KsLL', 'MU', [KsLLTF], props['USE_GEC']))
            mparts.append(FilterParts('KsDD', 'MU', [KsDD], props['USE_GEC']))
        if props['USE_LAMBDA']:
            mparts.append(FilterParts('LambdaLL', 'MU', [LambdaLLTrackFitted],
                                      props['USE_GEC']))
            mparts.append(FilterParts('LambdaDD', 'MU', [LambdaDDTrackFitted],
                                      props['USE_GEC']))

        # Build and register the primary lines.
        from Stages import (FilterMforN, FilterNforN, 
                            FilterSimple, FilterBDT, CombineN)
        minputs = []; mcombos = []; einputs = []; ecombos = []
        for n in range(2, 5):
            mcombos.append(CombineN('All', n, mparts if n == 2 else 
                                    mparts + [minputs[n - 3]]))
            ecombos.append(CombineN('E', n, eparts if n == 2 else 
                                    eparts + [einputs[n - 3]]))
            mfiltered = FilterNforN('All', n, [mcombos[-1]])
            efiltered = FilterNforN('E', n, [ecombos[-1]])
            if n < 4:
                minputs.append(FilterMforN('All', n, n + 1, [mcombos[-1]]))
                einputs.append(FilterMforN('E', n, n + 1, [ecombos[-1]]))
            stages = {}
            stages['NewTopo%iBodySimple' % n] = [
                FilterSimple(n, [mfiltered], varmap, props)]
            stages['NewTopo%iBodyBBDT' % n] = [
                FilterBDT('', n, [mfiltered], varmap, props)]
            stages['NewTopoMu%iBodyBBDT' % n] = [
                FilterBDT('MU', n, [mfiltered], varmap, props)]
            stages['NewTopoE%iBodyBBDT' % n] = [
                FilterBDT('E', n, [efiltered], varmap, props)]
            from HltLine.HltLine import Hlt2Line
            for (name, algos) in self.algorithms(stages).iteritems():
                if 'TopoE' in name:
                    l0  = props['L0_ELECTRON_FILTER']
                    hlt = props['HLT1_ELECTRON_FILT']
                else: l0 = None; hlt = None
                Hlt2Line(name, prescale = self.prescale, algos = algos,
                         L0DU = l0, HLT = hlt, postscale = self.postscale)

        # Build and register the radiative lines.
        from Inputs import BiKalmanFittedPhotonsFromL0
        stages = {}
        combos = CombineN('RAD', 3, [minputs[0], BiKalmanFittedPhotonsFromL0])
        stages['NewTopoRad2BodyBBDT'] = [
            FilterBDT('RAD', 2, [mcombos[0]], varmap, props)]
        stages['NewTopoRad2plus1BodyBBDT'] = [
            FilterBDT('RAD', 3, [combos], varmap, props)]
        from HltLine.HltLine import Hlt2Line
        for (name, algos) in self.algorithms(stages).iteritems():
            Hlt2Line(name, prescale = self.prescale, algos = algos,
                     L0DU = props['L0_RAD_FILTER'], postscale = self.postscale)
