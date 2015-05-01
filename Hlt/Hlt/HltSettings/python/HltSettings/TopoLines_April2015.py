from GaudiKernel.SystemOfUnits import MeV, picosecond, mm
class TopoLines_April2015 :
    """
    Threshold settings for Hlt2 topological lines.
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION!!!!!!!!
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """Returns a list of active lines."""

        lines = ['Hlt2Topo2Body',
                 'Hlt2Topo3Body',
                 'Hlt2Topo4Body',
                 'Hlt2TopoMu2Body',
                 'Hlt2TopoMu3Body',
                 'Hlt2TopoMu4Body'
                 ]
        
        return lines

    def Thresholds(self) :
        """Returns a dictionary of cuts."""
        d={}
        # NB: All vars are positive so negative = "no cut"
        from Hlt2Lines.Topo.Lines  import TopoLines
        d.update( { TopoLines : {'Common' : {
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
                "sumpt"  : ("SUMTREE(PT,"
                            "((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')), 0.0)/MeV"),
                "eta"    : "BPVETA",
                "fdchi2" : "BPVVDCHI2",
                "minpt"  : ("MINTREE(((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0')),"
                            "PT)/MeV"),
                "nlt16"  : ("NINTREE(((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0'))"
                            "& (BPVIPCHI2() < 16))"),
                "ipchi2" : "BPVIPCHI2()",
                "n1trk"  : ("NINTREE(((ABSID=='K+')|(ID=='KS0')|(ABSID=='Lambda0'))"
                            "& (PT > 1*GeV) & (BPVIPCHI2() > 16))")
            },
            'BDT_PARAMS'        : 'hlt2_topo_run2_v1.bbdt',
            'TRK_PT_MIN'        : 200 * MeV,
            'TRK_P_MIN'         : 3000 * MeV,
            'TRK_CHI2_MAX'      : 3,
            'TRK_IPCHI2_MIN'    : 4,
            'TRK_LT_MIN'        : 20 * picosecond,
            'CMB_TRK_NLT16_MAX' : 2,
            'CMB_PRT_PT_MIN'    : 2000 * MeV,
            'CMB_VRT_DIRA_MIN'  : 0,
            'CMB_VRT_VDCHI2_MIN': 16,
            'CMB_VRT_CHI2_MAX'  : 1000,
            'CMB_VRT_ETA_MIN'   : 2,
            'CMB_VRT_ETA_MAX'   : 5,
            'CMB_VRT_MCOR_MIN'  : 1000 * MeV,
            'CMB_VRT_MCOR_MAX'  : 10000 * MeV
        }}})

        return d

                    
