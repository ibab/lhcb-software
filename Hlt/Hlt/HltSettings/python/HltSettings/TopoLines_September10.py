class TopoLines :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of exclusive)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-05
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = ['Hlt2Topo2BodySA'
                 , 'Hlt2Topo4BodySA'
                 , 'Hlt2Topo3BodySA'
                 , 'Hlt2TopoTF4BodySA'
                 , 'Hlt2TopoTF3BodySA'
                 , 'Hlt2TopoTF2BodySA'
                 , 'Hlt2TopoTF4BodyReq4Yes'
                 , 'Hlt2TopoTF4BodyReq3Yes'
                 , 'Hlt2TopoTF4BodyReq2Yes'
                 , 'Hlt2TopoTF3BodyReq4Yes'
                 , 'Hlt2TopoTF3BodyReq3Yes'
                 , 'Hlt2TopoTF3BodyReq2Yes'
                 , 'Hlt2TopoTF2BodyReq4Yes'
                 , 'Hlt2TopoTF2BodyReq3Yes'
                 , 'Hlt2TopoTF2BodyReq2Yes'
                 , 'Hlt2TopoOSTF4Body'
                 , 'Hlt2TopoOSTF3Body'
                 , 'Hlt2TopoOSTF2Body'
                 , 'Hlt2TopoRobTF2Body'
                 , 'Hlt2TopoRobTF3Body'
                 , 'Hlt2TopoRobTF4Body'
                 ]
        
        return lines

   
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """

        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Hlt2TopologicalLines  import Hlt2TopologicalLinesConf
        d.update( { Hlt2TopologicalLinesConf : {
            # global cuts
            'MCOR_MAX'          : 7000.0, # MeV
            'MCOR_MIN'          : 4000.0, # MeV
            'SUM_PT_MIN'        : 4000.0, # MeV
            'MAX_PT_MIN'        : 1500.0, # MeV
            'ALL_PT_MIN'        : 500.0,  # MeV
            'ALL_P_MIN'         : 5000.0, # MeV
            'AMAXDOCA_MAX'      : 0.12,   # mm
            'AMINDOCA_MAX'      : 0.12,   # mm 
            'USE_GEC'           : False,
            'GEC_MAX'           : 200,
            'HLT1FILTER'        : "",
            'M_CHARM_VETO'      : 2500,   # MeV
            'SUM_IPCHI2_MIN'    : 100,   
            # fit cuts
            'ALL_MIPCHI2DV_MIN' : 16.0,    # unitless
            'ALL_TRCHI2DOF_MAX' : 5.0,   # unitless
            'BPVVDCHI2_MIN'     : 64.0,   # unitless
            'MIN_TRCHI2DOF_MAX' : 3,
            # robust
            'ALL_MIPDV_MIN'     : 0.025,  # mm
            'BPVVD_MIN'         : 2.0,    # mm
            'BPVVDR_MIN'        : 0.2,    # mm
            # pre and post scales
            'Prescale'   : {'Hlt2TopoTF2BodySA' : 0.01,
                            'Hlt2TopoTF3BodySA' : 0.01, 
                            'Hlt2TopoTF4BodySA' : 0.001,
                            'Hlt2Topo2BodySA' : 0.01,
                            'Hlt2Topo3BodySA' : 0.01,
                            'Hlt2Topo4BodySA' : 0.001,
                            'Hlt2TopoRobTF2Body' : 0.01,
                            'Hlt2TopoRobTF3Body' : 0.01,
                            'Hlt2TopoRobTF4Body' : 0.001,
                            'Hlt2TopoTF2BodyReq2YesDecision' : 0.001,
                            'Hlt2TopoTF2BodyReq3YesDecision' : 0.001, 
                            'Hlt2TopoTF2BodyReq4YesDecision' : 0.001,
                            'Hlt2TopoTF3BodyReq2YesDecision' : 0.001,
                            'Hlt2TopoTF3BodyReq3YesDecision' : 0.001,
                            'Hlt2TopoTF3BodyReq4YesDecision' : 0.001,
                            'Hlt2TopoTF4BodyReq2YesDecision' : 0.001,
                            'Hlt2TopoTF4BodyReq3YesDecision' : 0.001,
                            'Hlt2TopoTF4BodyReq4YesDecision' : 0.001}, 
            'Postscale'  : {}
            }})

        return d
