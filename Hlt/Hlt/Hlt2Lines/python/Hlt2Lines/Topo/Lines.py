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
            # V0 usage flags.
            'USE_KS'              : True,
            'USE_LAMBDA'          : True,
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
                 'DOCA': (-1, 0.2), 'M': (3500, 7000)}],
            # Optimized parameters.
            'OPT_TOS'               : 'Hlt1Track.*Decision%TOS',
            'OPT_GEC_MAX'           : 500,
            'OPT_BDT_MIN'           : 0.98,
            'OPT_BDT_PARAMS'        : 'hlt2_border_base.mx',
            'OPT_TRK_PT_MIN'        : 500 * MeV,
            'OPT_TRK_P_MIN'         : 5000 * MeV,
            'OPT_TRK_CHI2_MAX'      : 3,
            'OPT_TRK_IPCHI2_MIN'    : 4,
            'OPT_CMB_TRK_NLT16_MAX' : 2,
            'OPT_CMB_PRT_PT_MIN'    : 2000 * MeV,
            'OPT_CMB_VRT_DIRA_MIN'  : 0,
            'OPT_CMB_VRT_VDCHI2_MIN': 0,
            'OPT_CMB_VRT_CHI2_MAX'  : 10,
            'OPT_CMB_VRT_ETA_MIN'   : 2,
            'OPT_CMB_VRT_ETA_MAX'   : 5,
            'OPT_CMB_VRT_MCOR_MIN'  : 1000 * MeV,
            'OPT_CMB_VRT_MCOR_MAX'  : 15000 * MeV
            },
        }

    def __apply_configuration__(self):
        self.__apply_run1_configuration()
        self.__apply_run2_configuration()

    def __apply_run2_configuration(self):

        # The variable map for the BBDT.
        varmap = {
            "chi2"   : "VFASPF(VCHI2)",
            "sumpt"  : "SUMTREE(PT, ISBASIC, 0.0)",
            "eta"    : "BPVETA",
            "fdchi2" : "BPVVDCHI2",
            "minpt"  : "MINTREE(ISBASIC, PT)/MeV",
            "nlt16"  : "NINTREE(ISBASIC & (BPVIPCHI2() < 16))",
            "ipchi2" : "BPVIPCHI2()",
            "n1trk"  : "NINTREE(ISBASIC & (PT > 1*GeV) & (BPVIPCHI2() < 16))"
            }
        props = self.getProps()['Common']

        # Filter the particle input.
        from Inputs import BiKalmanFittedKaonsWithMuonID
        from Stages import FilterParts, CombineTos
        parts  = FilterParts('Kaon', [BiKalmanFittedKaonsWithMuonID],
                             props['OPT_GEC_MAX'] >= 0)
        combos = CombineTos([parts])

        # Build the lines.
        from Stages import FilterCombos, CombineN
        stages = {}
        for n in xrange(2, 5):
            if n == 2:
                stages['OptTopo%iBody' % n] = [
                    FilterCombos(n, [combos], varmap, props)]
                continue
            stages['OptTopo%iBody' % n] = [
                FilterCombos(n, [CombineN(n, [combos, parts])], varmap, props)]
        from HltLine.HltLine import Hlt2Line
        for (name, algos) in self.algorithms(stages).iteritems():
            Hlt2Line(name, prescale = self.prescale, algos = algos,
                     postscale = self.postscale)

    def __apply_run1_configuration(self):
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
        from Stages import Run1FilterParts
        mparts = [Run1FilterParts('Kaon', 'MU', [BiKalmanFittedKaonsWithMuonID],
                                  props['USE_GEC'])]
        eparts = [Run1FilterParts('Kaon', 'E', [BiKalmanFittedKaonsWithEID],
                                  props['USE_GEC'])]
        if props['USE_KS']:
            mparts.append(Run1FilterParts('KsLL', 'MU', [KsLLTF], 
                                          props['USE_GEC']))
            mparts.append(Run1FilterParts('KsDD', 'MU', [KsDD], 
                                          props['USE_GEC']))
        if props['USE_LAMBDA']:
            mparts.append(Run1FilterParts('LambdaLL', 'MU', 
                                          [LambdaLLTrackFitted],
                                          props['USE_GEC']))
            mparts.append(Run1FilterParts('LambdaDD', 'MU', 
                                          [LambdaDDTrackFitted],
                                          props['USE_GEC']))

        # Build and register the primary lines.
        from Stages import (Run1FilterMforN, Run1FilterNforN, 
                            Run1FilterSimple, Run1FilterBDT, Run1CombineN)
        minputs = []; mcombos = []; einputs = []; ecombos = []
        for n in xrange(2, 5):
            mcombos.append(Run1CombineN('All', n, mparts if n == 2 else 
                                        mparts + [minputs[n - 3]]))
            ecombos.append(Run1CombineN('E', n, eparts if n == 2 else 
                                        eparts + [einputs[n - 3]]))
            mfiltered = Run1FilterNforN('All', n, [mcombos[-1]])
            efiltered = Run1FilterNforN('E', n, [ecombos[-1]])
            if n < 4:
                minputs.append(Run1FilterMforN('All', n, n + 1, [mcombos[-1]]))
                einputs.append(Run1FilterMforN('E', n, n + 1, [ecombos[-1]]))
            stages = {}
            stages['Topo%iBodySimple' % n] = [
                Run1FilterSimple(n, [mfiltered], varmap, props)]
            stages['Topo%iBodyBBDT' % n] = [
                Run1FilterBDT('', n, [mfiltered], varmap, props)]
            stages['TopoMu%iBodyBBDT' % n] = [
                Run1FilterBDT('MU', n, [mfiltered], varmap, props)]
            stages['TopoE%iBodyBBDT' % n] = [
                Run1FilterBDT('E', n, [efiltered], varmap, props)]
            from HltLine.HltLine import Hlt2Line
            for (name, algos) in self.algorithms(stages).iteritems():
                if 'TopoE' in name:
                    l0   = props['L0_ELECTRON_FILTER']
                    hlt1 = props['HLT1_ELECTRON_FILT']
                else: l0 = None; hlt1 = None
                Hlt2Line(name, prescale = self.prescale, algos = algos,
                         L0DU = l0, HLT1 = hlt1, postscale = self.postscale)

        # Build and register the radiative lines.
        from Inputs import BiKalmanFittedPhotonsFromL0
        stages = {}
        combos = Run1CombineN('RAD', 3, 
                              [minputs[0], BiKalmanFittedPhotonsFromL0])
        stages['TopoRad2BodyBBDT'] = [
            Run1FilterBDT('RAD', 2, [mcombos[0]], varmap, props)]
        stages['TopoRad2plus1BodyBBDT'] = [
            Run1FilterBDT('RAD', 3, [combos], varmap, props)]
        from HltLine.HltLine import Hlt2Line
        for (name, algos) in self.algorithms(stages).iteritems():
            Hlt2Line(name, prescale = self.prescale, algos = algos,
                     L0DU = props['L0_RAD_FILTER'], postscale = self.postscale)
