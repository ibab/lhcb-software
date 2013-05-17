class TopoLines :
    """
    Threshold settings for Hlt2 topological lines.
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION!!!!!!!!
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """Returns a list of active lines."""

        lines = ['Hlt2Topo2BodySimple',
                 'Hlt2Topo3BodySimple',
                 'Hlt2Topo4BodySimple',
                 'Hlt2Topo2BodyBBDT',
                 'Hlt2Topo3BodyBBDT',
                 'Hlt2Topo4BodyBBDT',
                 'Hlt2TopoMu2BodyBBDT',
                 'Hlt2TopoMu3BodyBBDT',
                 'Hlt2TopoMu4BodyBBDT',
                 'Hlt2TopoE2BodyBBDT',
                 'Hlt2TopoE3BodyBBDT',
                 'Hlt2TopoE4BodyBBDT',
                 'Hlt2TopoRad2BodyBBDT',
                 'Hlt2TopoRad2plus1BodyBBDT'
                 ]
        
        return lines

   
    
    def Thresholds(self) :
        """Returns a dictionary of cuts."""

        # keep pass through thresholds
        d = { }

        # simple vars
        ptSum = "PTSUM"
        doca = "DOCA"
        ipChi2 = "CANDIPCHI2"
        mCor = "MCOR"
        ptMin = "PTMIN"
        fdChi2 = "FDCHI2"

        # NB: All vars are positive so negative = "no cut"

        simple_cuts2 = [{'M':(2500,7000),doca:(-1,0.2),fdChi2:(1000,-1),
                         ptSum:(7000,-1)}]
        simple_cuts3 = [{'M':(3000,7000),doca:(-1,0.2),fdChi2:(1000,-1),
                         ptSum:(8000,-1)}]
        simple_cuts4 = [{'M':(3500,7000),doca:(-1,0.2),fdChi2:(1000,-1),
                         ptSum:(9000,-1)}]

        from Hlt2Lines.Hlt2TopologicalLines  import Hlt2TopologicalLinesConf
        d.update( { Hlt2TopologicalLinesConf : {
            # single track cuts
            'ALL_PT_MIN'        : 500.0,  # MeV
            'ALL_P_MIN'         : 5000.0, # MeV
            'ALL_MIPCHI2DV_MIN' : 4.0,    # unitless
            'MU_TRCHI2DOF_MAX'  : 2.5,    # unitless
            'E_TRCHI2DOF_MAX'   : 2.5,    # unitless
            'HAD_TRCHI2DOF_MAX' : 2.5,    # unitless
            # V0's
            'USE_KS'            : True,
            'USE_LAMBDA'        : True,
            # upfront combo cuts
            'AMAXDOCA_MAX'      : 0.2,    # mm        
            'BPVVDCHI2_MIN'     : 100.0,  # unitless
            'MIN_TRCHI2DOF_MAX' : 2.0,      # unitless
            'ONETRACK_PT_MIN'   : 1700.0, # MeV
            'ONETRACK_IPCHI2_MIN' : 16,   # unitless   
            'V2BODYCHI2_MAX'    : 10,     # unitless
            'NV0_2Body_MAX'     : 2,
            'NV0_3Body_MAX'     : 2,
            'NV0_4Body_MAX'     : 1,
            # bdt cuts
            'BDT_2BODY_MIN'     : 0.4,
            'BDT_3BODY_MIN'     : 0.4,
            'BDT_4BODY_MIN'     : 0.3,
            'BDT_2BODYMU_MIN'   : 0.1,
            'BDT_3BODYMU_MIN'   : 0.1,
            'BDT_4BODYMU_MIN'   : 0.1,
            'BDT_2BODYE_MIN'    : 0.1,
            'BDT_3BODYE_MIN'    : 0.1,
            'BDT_4BODYE_MIN'    : 0.1,
            'BDT_RAD2_MIN'      : 0.1,
            'BDT_RAD2p1_MIN'    : 0.1,
            # bdt param file versions
            'BDT_2BODY_PARAMS'  : 'v1r0',
            'BDT_3BODY_PARAMS'  : 'v1r0',
            'BDT_4BODY_PARAMS'  : 'v1r0',
            # electron filters & cuts
            'PIDE_MIN'          : -2.0,
            'L0_ELECTRON_FILTER': "L0_CHANNEL('Electron')",
            'HLT1_ELECTRON_FILT': "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')",
            # radiative filters
            'L0_RAD_FILTER'     : "L0_CHANNEL('Electron') | L0_CHANNEL('Photon')",
            'RAD_TOS'           : True,
            # global event cuts
            'USE_GEC'           : True,
            'GEC_MAX'           : 500,
            # "simple" cuts            
            'SIMPLE_2BODY_CUTS' : simple_cuts2,
            'SIMPLE_3BODY_CUTS' : simple_cuts3,
            'SIMPLE_4BODY_CUTS' : simple_cuts4,            
            # pre and post scales
            'Prescale'   : {'Hlt2TopoRad2BodyBBDT' : 0.0, 'Hlt2TopoRad2plus1BodyBBDT' : 0.0}, 
            'Postscale'  : {}
            }})

        return d
