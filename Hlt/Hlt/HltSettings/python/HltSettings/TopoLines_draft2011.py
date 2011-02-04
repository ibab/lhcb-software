class TopoLines :
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
                 'Hlt2TopoMu4Body',
                 ]
        
        return lines

   
    
    def Thresholds(self) :
        """Returns a dictionary of cuts."""

        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Hlt2TopologicalLines  import Hlt2TopologicalLinesConf
        d.update( { Hlt2TopologicalLinesConf : {
            # single track cuts
            'ALL_PT_MIN'        : 500.0,  # MeV
            'ALL_P_MIN'         : 5000.0, # MeV
            'ALL_MIPCHI2DV_MIN' : 4.0,    # unitless
            'ALL_TRCHI2DOF_MAX' : 4.0,    # unitless
            # upfront combo cuts
            'AMAXDOCA_MAX'      : 0.2,    # mm        
            'BPVVDCHI2_MIN'     : 100.0,  # unitless
            'MIN_TRCHI2DOF_MAX' : 2.4,    # unitless
            'V2BODYCHI2_MAX'    : 10,     # unitless
            # bdt cuts
            'BDT_2BODY_MIN'     : 0.65,
            'BDT_3BODY_MIN'     : 0.7,
            'BDT_4BODY_MIN'     : 0.42,
            'BDT_2BODYMU_MIN'   : 0.3,
            'BDT_3BODYMU_MIN'   : 0.33,
            'BDT_4BODYMU_MIN'   : 0.2,
            # bdt param file versions
            'BDT_2BODY_PARAMS'  : 'v1r0',
            'BDT_3BODY_PARAMS'  : 'v1r0',
            'BDT_4BODY_PARAMS'  : 'v1r0',
            # global event cuts
            'USE_GEC'           : False,
            'GEC_MAX'           : 350,
            'HLT1FILTER'        : "",
            # pre and post scales
            'Prescale'   : {}, 
            'Postscale'  : {}
            }})

        return d
