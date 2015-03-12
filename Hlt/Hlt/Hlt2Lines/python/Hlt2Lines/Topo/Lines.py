from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import \
    Hlt2LinesConfigurableUser
class TopoLines(Hlt2LinesConfigurableUser) :
    from GaudiKernel.SystemOfUnits import MeV, picosecond, mm
    __slots__ = {
        'Common' : {
            # TISTOS cuts.
            'TisTosSpec': 'Hlt1Track.*Decision%TOS',
            # Particle cuts.
            'ALL_PT_MIN'        : 500.0 * MeV,
            'ALL_P_MIN'         : 5000.0 * MeV,
            'ALL_MIPCHI2DV_MIN' : 4.0,
            'MU_TRCHI2DOF_MAX'  : 4.0,
            'HAD_TRCHI2DOF_MAX' : 3.0,
            # V0 usage flags.
            'USE_KS'            : True,
            'USE_LAMBDA'        : True,
            # Combo cuts.
            'AMAXDOCA_MAX'      : 0.2 * mm,
            'BPVVDCHI2_MIN'     : 100.0,
            'MIN_TRCHI2DOF_MAX' : 2,
            'ONETRACK_PT_MIN'   : 1700.0 * MeV,
            'MU_ONETRACK_PT_MIN': 1200.0 * MeV,
            'ONETRACK_IPCHI2_MIN' : 36,
            'V2BODYCHI2_MAX'    : 10,
            'NV0_2Body_MAX'     : 2,
            'NV0_3Body_MAX'     : 2,
            'NV0_4Body_MAX'     : 2,
            'MIN_V0_LT'         : 10 * picosecond,
            # BDT response cuts.
            'BDT_2BODY_MIN'     : 0.4,
            'BDT_3BODY_MIN'     : 0.4,
            'BDT_4BODY_MIN'     : 0.3,
            # BDT param file versions.
            'BDT_2BODY_PARAMS'  : 'v1r0',
            'BDT_3BODY_PARAMS'  : 'v1r0',
            'BDT_4BODY_PARAMS'  : 'v1r0',
            }
        }

    def __apply_configuration__(self) :
        # The variable map for the BDT.
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

        # Create the stages.
        from Stages import (InPartFilter, AllNBody, FilterNforN, FilterSimple,
                            FilterBDT)
        from Inputs import BiKalmanFittedKaonsWithMuonID
        kaons     = InPartFilter('NewTopoInputKaons',
                                 [BiKalmanFittedKaonsWithMuonID])
        all2      = AllNBody('All', 2, [kaons])
        topo2_all = FilterNforN('All', 2, [all2])
        topo2_simple = FilterSimple('Simple', 2, [topo2_all], varmap)
        topo2_bdt    = FilterBDT('Bdt', 2, [topo2_all], varmap,
                                 self.getProps()['Common']['BDT_2BODY_PARAMS'],
                                 'BDT_2BODY_MIN')
        stages = {
            'NewTopo2BodySimple' : [topo2_simple],
            'NewTopo2BodyBBDT' :   [topo2_bdt]
            }

        # Register the lines.
        from HltLine.HltLine import Hlt2Line
        from Configurables import HltANNSvc
        for (name, algos) in self.algorithms(stages).iteritems():
            Hlt2Line(name, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
