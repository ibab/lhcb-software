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
            'Hlt2DisplVerticesSingle',
            'Hlt2DisplVerticesDouble', 
            'Hlt2DisplVerticesHighMassSingle',
            'Hlt2DisplVerticesHighFDSingle',
            'Hlt2DisplVerticesSinglePostScaled',
            'Hlt2DisplVerticesSingleDown',
            'Hlt2DisplVerticesDoublePostScaled',
            'Hlt2DisplVerticesSingleHighMassPostScaled',
            'Hlt2DisplVerticesSingleHighFDPostScaled',
            'Hlt2DisplVerticesSingleMVPostScaled'
            ]

        return lines

    def Thresholds(self) : 
       
        d = { }
        ##########################################################################
        #
        # For tuning:
        # Prescale only the PS lines:
        # Hlt2SinglePSLonglived
        # Hlt2DoublePSLonglived
        # Hlt2SinglePSLonglivedHighMass(PS of 0.02 give 0.1 Hz)
        # Hlt2SinglePSLonglivedHighFD (PS of 0.02 give 0.1 Hz)
        # Hlt2SinglePSLonglivedMV
        ##########################################################################
        
        from Hlt2Lines.Hlt2DisplVerticesLines  import Hlt2DisplVerticesLinesConf
        d.update( { Hlt2DisplVerticesLinesConf : {
            "MinNbTracks"  : { 'Hlt2RV2P' : 4
                               , 'Hlt2SingleLonglived' : 5
                               , 'Hlt2SingleDown' : 4
                               , 'Hlt2SingleLonglivedHighFD' :  5
                               , 'Hlt2SingleLonglivedHighMass' : 6
                               , 'Hlt2DoubleLonglived' : 4
                               , 'Hlt2SinglePSLonglived' : 4
                               , 'Hlt2DoublePSLonglived' : 4
                               , 'Hlt2SinglePSLonglivedHighMass' : 4
                               , 'Hlt2SinglePSLonglivedHighFD' : 5
                               , 'Hlt2SinglePSLonglivedMV' : 4
                               }
            ,  "RCutMethod"   : "FromBeamSpot"
            ,  "RMin"         : {   'Hlt2RV2P' : 0.4
                                    , 'Hlt2SingleLonglived' : 2.5
                                    , 'Hlt2SingleDown' : 2.0
                                    , 'Hlt2SingleLonglivedHighFD' : 4.0
                                    , 'Hlt2SingleLonglivedHighMass' : 0.4
                                    , 'Hlt2DoubleLonglived' : 0.4 
                                    , 'Hlt2SinglePSLonglived' : 0.4
                                    , 'Hlt2DoublePSLonglived' : 0.4
                                    , 'Hlt2SinglePSLonglivedHighMass' : 0.4
                                    , 'Hlt2SinglePSLonglivedHighFD' : 2.0
                                    , 'Hlt2SinglePSLonglivedMV' : 0.4  
                                  }
            ,  "MinMass"      : {   'Hlt2RV2P' : 3000.
                                    , 'Hlt2SingleLonglived' : 7000.
                                    , 'Hlt2SingleDown' : 3000
                                    , 'Hlt2SingleLonglivedHighFD' : 4500.
                                    , 'Hlt2SingleLonglivedHighMass' : 10000.
                                    , 'Hlt2DoubleLonglived' : 3000.
                                    , 'Hlt2SinglePSLonglived' : 3000.
                                    , 'Hlt2DoublePSLonglived' : 3000.
                                    , 'Hlt2SinglePSLonglivedHighMass' : 10000.
                                    , 'Hlt2SinglePSLonglivedHighFD' : 4500.
                                    , 'Hlt2SinglePSLonglivedMV' :3000.
                                  }
            ,  "HighestMass"  : {   'Hlt2DoubleLonglived' : 4500.
                                    ,'Hlt2DoublePSLonglived' : 3000.
                                    }
            ,  "MinSumpt"     :{ 'Hlt2RV2P' : 3000.
                                 , 'Hlt2SingleLonglived' : 3000.
                                 , 'Hlt2SingleDown' : 3000.
                                 , 'Hlt2SingleLonglivedHighFD' : 3000.
                                 , 'Hlt2SingleLonglivedHighMass' : 3000
                                 , 'Hlt2DoubleLonglived' : 3000.
                                 , 'Hlt2SinglePSLonglived' : 3000.
                                 , 'Hlt2DoublePSLonglived' : 3000.
                                 , 'Hlt2SinglePSLonglivedHighMass' : 3000.
                                 , 'Hlt2SinglePSLonglivedHighFD' : 3000.
                                 , 'Hlt2SinglePSLonglivedMV' :3000.
                                 } 
            ,  "RemVtxFromDet" : { 'Hlt2RV2P' : 0
                                 , 'Hlt2SingleLonglived' : 5
                                 , 'Hlt2SingleLonglivedHighFD' : 5
                                 , 'Hlt2SingleLonglivedHighMass' : 5
                                 , 'Hlt2DoubleLonglived' : 5
                                 , 'Hlt2SingleDown' : 0
                                 , 'Hlt2SinglePSLonglived' : 0
                                 , 'Hlt2DoublePSLonglived' : 0
                                 , 'Hlt2SinglePSLonglivedHighMass' : 5
                                 , 'Hlt2SinglePSLonglivedHighFD' : 5
                                 , 'Hlt2SinglePSLonglivedMV' : 5
                                   }
            ,  "AllOutDet"  : {  'Hlt2DoubleLonglived' : False
                                 ,'Hlt2DoublePSLonglived' : False
                                 }
            , "PostScale" : { 'Hlt2RV2P' : 1.
                              , 'Hlt2SingleLonglived' : 1.
                              , 'Hlt2SingleDown' : 1.
                              , 'Hlt2SingleLonglivedHighFD' : 1.
                              , 'Hlt2SingleLonglivedHighMass' : 1.
                              , 'Hlt2DoubleLonglived' : 1.
                              , 'Hlt2SinglePSLonglived' : 0.0001
                              , 'Hlt2DoublePSLonglived' : 0.01
                              , 'Hlt2SinglePSLonglivedHighMass' : 1.
                              , 'Hlt2SinglePSLonglivedHighFD' : 1.
                              , 'Hlt2SinglePSLonglivedMV' : 0.0006

                              }             
            # pre and post scales
            }})

        return d
    
