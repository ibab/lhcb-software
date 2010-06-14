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
                        ## Cut values for robust stages.
                          'ComRobAllTrkPtLL'        : 300.0      # in MeV
                        , 'ComRobAllTrkPLL'         : 2000.0     # in MeV
                        , 'ComRobAllTrkPVIPLL'      : 0.025      # in mm
                        , 'ComRobPairMinDocaUL'     : 0.20       # in mm
                        , 'ComRobPairMaxDocaUL'     : 1.0        # in mm
                        , 'ComRobTrkMaxPtLL'        : 1500.0     # in MeV
                        , 'ComRobVtxPVDispLL'       : 2.0        # in mm
                        , 'ComRobVtxPVRDispLL'      : 0.2        # in mm
                        , 'RobustPointingUL'        : 0.28       # unitless
                        , 'ComRobUseGEC'            : True       # do or do not 
                        , 'ComRobGEC'               : 120        # max N trk
                        ## Cut values for post-track fit stages
                        , 'ComTFAllTrkPtLL'         : 300.0      # in MeV
                        , 'ComTFAllTrkPLL'          : 2000.0     # in MeV
                        , 'ComTFAllTrkPVIPChi2LL'   : 9.0        # unitless
                        , 'ComTFAllTrkChi2UL'       : 10.0       # unitless
                        , 'ComTFPairMinDocaUL'      : 0.20       # in mm
                        , 'ComTFPairMaxDocaUL'      : 1.0        # in mm
                        , 'ComTFTrkMaxPtLL'         : 1500.0     # in MeV
                        , 'ComTFVtxPVDispChi2LL'    : 64.0       # unitless
                        , 'TFPointUL'               : 0.28       # unitless
                        ## Cut values for one stage track-fit (OSTF) lines.
                        , 'OSTFAllTrkPtLL'          : 300.0      # in MeV
                        , 'OSTFAllTrkPLL'           : 2000.0     # in MeV
                        , 'OSTFAllTrkPVIPChi2LL'    : 9.0        # unitless
                        , 'OSTFAllTrkChi2UL'        : 10.0       # unitless
                        , 'OSTFPairMinDocaUL'       : 0.20       # in mm
                        , 'OSTFPairMaxDocaUL'       : 1.0        # in mm
                        , 'OSTFTrkMaxPtLL'          : 1500.0     # in MeV
                        , 'OSTFVtxPVDispChi2LL'     : 64.0       # unitless
                        , 'OSTFPointUL'             : 0.28       # unitless
                        , 'Prescale'   : { 'Hlt2TopoTF2BodySA' : 1.00
                                         , 'Hlt2TopoTF3BodySA' : 1.00
                                         , 'Hlt2TopoTF4BodySA' : 1.00
                                         }
                        , 'Postscale'  : { 'Hlt2Topo2BodySA' : 1.00
                                         , 'Hlt2Topo3BodySA' : 0.10
                                         , 'Hlt2Topo4BodySA' : 0.10
                                         , 'Hlt2TopoTF2BodySA' : 1.00
                                         , 'Hlt2TopoTF3BodySA' : 0.10
                                         , 'Hlt2TopoTF4BodySA' : 0.10
                                         }
                    }
                  } )

        return d
