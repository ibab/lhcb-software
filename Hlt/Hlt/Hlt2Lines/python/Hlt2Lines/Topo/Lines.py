from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import \
    Hlt2LinesConfigurableUser
class TopoLines(Hlt2LinesConfigurableUser):
    from GaudiKernel.SystemOfUnits import MeV, picosecond, mm
    __slots__ = {
        'Common' : {
            # TISTOS cuts.
            'TisTosSpec': 'Hlt1Track.*Decision%TOS',
            # Particle cuts.
            'ALL_PT_MIN'          : 500.0 * MeV,
            'ALL_P_MIN'           : 5000.0 * MeV,
            'ALL_MIPCHI2DV_MIN'   : 4.0,
            'E_TRCHI2DOF_MAX'     : 5.0,
            'MU_TRCHI2DOF_MAX'    : 4.0,
            'HAD_TRCHI2DOF_MAX'   : 3.0,
            'PIDE_MIN'            : -2.0,
            # V0 usage flags (currently ignored).
            'USE_KS'              : True,
            'USE_LAMBDA'          : True,
            # Combo cuts.
            'AMAXDOCA_MAX'        : 0.2 * mm,
            'BPVVDCHI2_MIN'       : 100.0,
            'MIN_TRCHI2DOF_MAX'   : 2,
            'ONETRACK_PT_MIN'     : 1700.0 * MeV,
            'MU_ONETRACK_PT_MIN'  : 1200.0 * MeV,
            'ONETRACK_IPCHI2_MIN' : 36,
            'V2BODYCHI2_MAX'      : 10,
            'NV0_2Body_MAX'       : 2,
            'NV0_3Body_MAX'       : 2,
            'NV0_4Body_MAX'       : 2,
            'MIN_V0_LT'           : 10 * picosecond,
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

        # Filter the inputs.
        from Inputs import (BiKalmanFittedKaonsWithMuonID, 
                            BiKalmanFittedKaonsWithEID)
        from Stages import FilterParts
        mkaons  = FilterParts('Kaons', 'MU', [BiKalmanFittedKaonsWithMuonID])
        ekaons  = FilterParts('Kaons', 'E', [BiKalmanFittedKaonsWithEID])

        # Build and register the primary lines.
        from Stages import (FilterMforN, FilterNforN, 
                            FilterSimple, FilterBDT, CombineN)
        minputs = []; mcombos = []; einputs = []; ecombos = []
        for n in range(2, 5):
            mcombos.append(CombineN('All', n, [mkaons] if n == 2 else 
                                    [mkaons, minputs[n - 3]]))
            ecombos.append(CombineN('E', n, [ekaons] if n == 2 else 
                                    [ekaons, einputs[n - 3]]))
            mfiltered = FilterNforN('All', n, [mcombos[-1]])
            efiltered = FilterNforN('E', n, [ecombos[-1]])
            if n < 4:
                minputs.append(FilterMforN('All', n, n + 1, [mcombos[-1]]))
                einputs.append(FilterMforN('E', n, n + 1, [ecombos[-1]]))
            params = self.getProps()['Common']['BDT_%iBODY_PARAMS' % n]
            stages  = {}
            stages['NewTopo%iBodySimple' % n] = [
                FilterSimple(n, [mfiltered], varmap)]
            stages['NewTopo%iBodyBBDT' % n] = [
                FilterBDT('', n, [mfiltered], varmap, params)]
            stages['NewTopoMu%iBodyBBDT' % n] = [
                FilterBDT('MU', n, [mfiltered], varmap, params)]
            stages['NewTopoE%iBodyBBDT' % n] = [
                FilterBDT('E', n, [efiltered], varmap, params)]
            from HltLine.HltLine import Hlt2Line
            for (name, algos) in self.algorithms(stages).iteritems():
                Hlt2Line(name, prescale = self.prescale,
                         algos = algos, postscale = self.postscale)

        # Build and register the radiative lines.
        from Inputs import BiKalmanFittedPhotonsFromL0
        stages = {}
        params = self.getProps()['Common']['BDT_2BODY_PARAMS']
        combos = CombineN('RAD', 3, [minputs[0], BiKalmanFittedPhotonsFromL0])
        stages['NewTopoRad2BodyBBDT'] = [
            FilterBDT('RAD', 2, [mcombos[0]], varmap, params)]
        stages['NewTopoRad2plus1BodyBBDT'] = [
            FilterBDT('RAD', 3, [combos], varmap, params)]
        from HltLine.HltLine import Hlt2Line
        for (name, algos) in self.algorithms(stages).iteritems():
            Hlt2Line(name, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
