class DV_draft2011 :
    """
    Threshold settings for Hlt2 displaced vertex lines lines: draft for 2011
    
    @author V. Coco
    @date 2011-02-12
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            # The topological
            'Hlt2DisplVerticesDouble', 
            'Hlt2DisplVerticesLowMassSingle',
            'Hlt2DisplVerticesHighMassSingle',
            'Hlt2DisplVerticesHighFDSingle',
            'Hlt2DisplVerticesSinglePostScaled',
            'Hlt2DisplVerticesSingleDown'
            ]

        return lines

    def Thresholds(self) : 
       
        d = { }
        ##########################################################################
        #
        #  For the Bandwidth division, variable for each line that might be tunned
        #       line Hlt2DisplVerticesLowMassSingle:  slot ["MinAngle"]['Hlt2SingleLonglivedLowMass'] can vary in range [ 0. ; 0.035 ] 
        #       line Hlt2DisplVerticesHighMassSingle: ["RMin"]['Hlt2SingleLonglivedHighMass'] can vary in range [ 0. ; 0.7 ] 
        #       line Hlt2DisplVerticesHighFDSingle:   ["MinFD"]['Hlt2SingleLonglivedHighFD'] can vary in range [ 15. ; 20. ] 
        #       line Hlt2DisplVerticesDouble:   ["MinMass"]['Hlt2DoubleLonglived'] can vary in range [ 2000. ; 3000. ]  
        #       line Hlt2DisplVerticesSinglePostScaled:   ["PostScale"]["Hlt2SinglePSLonglived"] can vary to the requiered value to have 1Hz
        #
        ##########################################################################
        

        from Hlt2Lines.Hlt2DisplVerticesLines  import Hlt2DisplVerticesLinesConf
        d.update( { Hlt2DisplVerticesLinesConf : {
             "MinNbTracks"  : { 'Hlt2RV2P' : 4
                               , 'Hlt2SingleLonglivedHighFD' : 5
                               , 'Hlt2SingleLonglivedLowMass' : 6
                               , 'Hlt2SingleLonglivedHighMass' : 6
                               , 'Hlt2DoubleLonglived' : 4
                               , 'Hlt2SinglePSLonglived' : 4
                               }
            ,  "RCutMethod"   : "FromUpstreamPV"
            ,  "RMin"         : { 'Hlt2RV2P' : 0.
                                  , 'Hlt2SingleLonglivedHighFD' : 0.7
                                  , 'Hlt2SingleLonglivedLowMass' : 0.3
                                  , 'Hlt2SingleLonglivedHighMass' : 0.3
                                  , 'Hlt2DoubleLonglived' : 0.3
                                  , 'Hlt2SinglePSLonglived' : 0.3
                                  }
            ,  "MinMass"      : {   'Hlt2RV2P' : 2000.
                                  , 'Hlt2SingleLonglivedHighFD' : 4500.
                                  , 'Hlt2SingleLonglivedLowMass' : 4500.
                                  , 'Hlt2SingleLonglivedHighMass' : 9000.
                                  , 'Hlt2DoubleLonglived' : 2000.
                                  , 'Hlt2SinglePSLonglived' : 2000.
                                  }
            ,  "MinSumpt"     :{ 'Hlt2RV2P' : 3000.
                                 , 'Hlt2SingleLonglivedHighFD' : 4500.
                                 , 'Hlt2SingleLonglivedLowMass' : 4500.
                                 , 'Hlt2SingleLonglivedHighMass' : 10000.
                                 , 'Hlt2DoubleLonglived' : 3000.
                                 , 'Hlt2SinglePSLonglived' : 3000.
                                 } 
            ,  "RemVtxFromDet" : { 'Hlt2RV2P' : 0
                                 , 'Hlt2SingleLonglivedHighFD' : 5
                                 , 'Hlt2SingleLonglivedLowMass' : 5
                                 , 'Hlt2SingleLonglivedHighMass' : 5
                                 , 'Hlt2DoubleLonglived' : 0
                                 , 'Hlt2SinglePSLonglived' : 0
                                   }
            ,  "MinAngle" : { 'Hlt2RV2P' : 0.
                                 , 'Hlt2SingleLonglivedHighFD' : 0.035
                                 , 'Hlt2SingleLonglivedLowMass' : 0.0
                                 , 'Hlt2SingleLonglivedHighMass' : 0.
                                 , 'Hlt2DoubleLonglived' : 0.0
                                 , 'Hlt2SinglePSLonglived' : 0.0
                                   }
            ,  "MinFD" : {  'Hlt2SingleLonglivedHighFD' : 15.
                                   }
            , "PostScale" : { 'Hlt2RV2P' : 1
                              , 'Hlt2SingleLonglivedHighFD' : 1
                              , 'Hlt2SingleLonglivedLowMass' : 1
                              , 'Hlt2SingleLonglivedHighMass' : 1
                              , 'Hlt2DoubleLonglived' : 1
                              , 'Hlt2SinglePSLonglived' : 0.0006
                              }

                
            # pre and post scales
            }})

        return d
    
