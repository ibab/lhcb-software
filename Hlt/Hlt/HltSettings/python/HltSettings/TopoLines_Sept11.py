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

        # 2-body cuts
        base_cuts = {'M':(-1,7000),doca:(-1,0.2),fdChi2:(100,-1),
                     ptSum:(3000,-1),mCor:(4000,7000)}
        cuts2 = []
        cut = base_cuts.copy()
        cut[ptMin] = (3000,-1)
        cuts2.append(cut)
        cut = base_cuts.copy()
        cut[ptSum] = (10000,-1)
        cuts2.append(cut)
        base_cuts['M'] = (2000,7000)
        cut = base_cuts.copy()
        cut[fdChi2] = (1000,-1)
        cut[ptSum] = (5000,-1)
        cut[ptMin] = (750,-1)
        cuts2.append(cut)
        cut = base_cuts.copy()
        cut[fdChi2] = (300,-1)
        cut[ptSum] = (5500,-1)
        cut[ptMin] = (1000,-1)
        cuts2.append(cut)

        # 3-body cuts
        base_cuts = {'M':(-1,7000),doca:(-1,0.1),fdChi2:(100,-1),
                     ptSum:(4000,-1),mCor:(4000,7000)}
        cuts3 = []
        cut = base_cuts.copy()
        cut[ptMin] = (3000,-1)
        cuts3.append(cut)
        cut = base_cuts.copy()
        cut[ptSum] = (15000,-1)
        cuts3.append(cut)
        base_cuts['M'] = (3000,7000)
        base_cuts[mCor] = (4500,6500)
        base_cuts[fdChi2] = (300,-1)
        cut = base_cuts.copy()
        cut[ptMin] = (2000,-1)
        cuts3.append(cut)
        cut = base_cuts.copy()
        cut[ptSum] = (6000,-1)
        cut[ptMin] = (1000,-1)
        cuts3.append(cut)
        cut = base_cuts.copy()
        cut[ptSum] = (7500,-1)
        cut[ptMin] = (750,-1)
        cuts3.append(cut)
        base_cuts[fdChi2] = (1000,-1)
        cut = base_cuts.copy()
        cut[ptMin] = (1500,-1)
        cuts3.append(cut)
        cut = base_cuts.copy()
        cut[ptSum] = (7000,-1)
        cut[ptMin] = (600,-1)
        cuts3.append(cut)

        # 4-body cuts
        base_cuts = {'M':(-1,7000),doca:(-1,0.1),fdChi2:(100,-1),
                     ptSum:(4000,-1),mCor:(4000,7000)}
        cuts4 = []
        cut = base_cuts.copy()
        cut[ptMin] = (2000,-1)
        cuts4.append(cut)
        cut = base_cuts.copy()
        cut[ptSum] = (15000,-1)
        cuts4.append(cut)
        base_cuts['M'] = (3500,7000)
        base_cuts[fdChi2] = (300,-1)
        cut = base_cuts.copy()
        cut[ptMin] = (1750,-1)
        cuts4.append(cut)
        cut = base_cuts.copy()
        cut[ptSum] = (8000,-1)
        cut[ptMin] = (600,-1)
        cuts4.append(cut)
        base_cuts[fdChi2] = (1000,-1)
        cut = base_cuts.copy()
        cut[ptMin] = (1250,-1)
        cuts4.append(cut)
        cut = base_cuts.copy()
        cut[ptSum] = (7500,-1)
        cut[ptMin] = (550,-1)
        cuts4.append(cut)

        from Hlt2Lines.Hlt2TopologicalLines  import Hlt2TopologicalLinesConf
        d.update( { Hlt2TopologicalLinesConf : {
            # single track cuts
            'ALL_PT_MIN'        : 500.0,  # MeV
            'ALL_P_MIN'         : 5000.0, # MeV
            'ALL_MIPCHI2DV_MIN' : 4.0,    # unitless
            'MU_TRCHI2DOF_MAX'  : 4.0,    # unitless
            'E_TRCHI2DOF_MAX'   : 5.0,    # unitless
            'HAD_TRCHI2DOF_MAX' : 3.0,    # unitless
            # V0 cuts
            'V0DT_MIPCHI2DV_MIN': 16.0,    # unitless
            'KS_MASS_WINDOW'    : 30.0,   # MeV
            'KS_BPVVDCHI2_MIN'  : 1000.0,  # unitless 
            'USE_KS'            : True,
            'L_MASS_WINDOW'     : 20.0,   # MeV
            'L_BPVVDCHI2_MIN'   : 1000.0,  # unitless 
            'USE_LAMBDA'        : True,
            # upfront combo cuts
            'AMAXDOCA_MAX'      : 0.2,    # mm        
            'BPVVDCHI2_MIN'     : 100.0,  # unitless
            'MIN_TRCHI2DOF_MAX' : 2.4,    # unitless
            'V2BODYCHI2_MAX'    : 10,     # unitless
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
            'USE_GEC'           : False,
            'GEC_MAX'           : 350,
            # "simple" cuts            
            'SIMPLE_2BODY_CUTS' : cuts2,
            'SIMPLE_3BODY_CUTS' : cuts3,
            'SIMPLE_4BODY_CUTS' : cuts4,            
            # pre and post scales
            'Prescale'   : {}, 
            'Postscale'  : {}
            }})

        return d
