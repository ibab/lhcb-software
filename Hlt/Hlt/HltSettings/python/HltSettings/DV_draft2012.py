class DV_draft2012 :
    """
    Threshold settings for Hlt2 displaced vertex lines lines: draft for 2012
    
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
            "FilterVelo" : True ,
            "VeloIP" : 0.1 ,
            "UseVeloTracks": False,
            "FracE1Track": 0.9,
            "FracTrackBefV": 0.49,
            "MaxChi2Long": 5.,
            "MaxChi2Down": 10.,
            "MinNbTracks"  : { 'Hlt2RV2P' : 4
                               , 'Hlt2SingleLonglived' : 4
                               , 'Hlt2SingleDown' : 4
                               , 'Hlt2PreSelSingleDown' : 5
                               , 'Hlt2SingleLonglivedHighFD' :  6
                               , 'Hlt2SingleLonglivedHighMass' : 6
                               , 'Hlt2DoubleLonglived' : 4
                               , 'Hlt2SinglePSLonglived' : 4
                               , 'Hlt2DoublePSLonglived' : 4
                               , 'Hlt2SinglePSLonglivedHighMass' : 5
                               , 'Hlt2SinglePSLonglivedHighFD' : 5
                               , 'Hlt2SinglePSLonglivedMV' : 4
                               }
            ,  "RMin"         : {   'Hlt2RV2P' : 0.4
                                    , 'Hlt2SingleLonglived' : 1.7
                                    , 'Hlt2SingleDown' : 2. 
                                    , 'Hlt2PreSelSingleDown' : 0.4
                                    , 'Hlt2SingleLonglivedHighFD' : 4.
                                    , 'Hlt2SingleLonglivedHighMass' : 0.5
                                    , 'Hlt2DoubleLonglived' : 0.4 
                                    , 'Hlt2SinglePSLonglived' : 0.4
                                    , 'Hlt2DoublePSLonglived' : 0.4
                                    , 'Hlt2SinglePSLonglivedHighMass' : 0.5
                                    , 'Hlt2SinglePSLonglivedHighFD' : 3.0
                                    , 'Hlt2SinglePSLonglivedMV' : 0.4  
                                  }
            ,  "MinMass"      : {   'Hlt2RV2P' : 0.
                                    , 'Hlt2SingleLonglived' : 5500.
                                    , 'Hlt2SingleDown' : 3000
                                    , 'Hlt2PreSelSingleDown' : 2800.
                                    , 'Hlt2SingleLonglivedHighFD' : 3200.
                                    , 'Hlt2SingleLonglivedHighMass' : 8000.
                                    , 'Hlt2DoubleLonglived' : 2000.
                                    , 'Hlt2SinglePSLonglived' : 0.
                                    , 'Hlt2DoublePSLonglived' : 2000.
                                    , 'Hlt2SinglePSLonglivedHighMass' : 8000.
                                    , 'Hlt2SinglePSLonglivedHighFD' : 3200.
                                    , 'Hlt2SinglePSLonglivedMV' :0.

                                  }
            ,  "HighestMass"  : {   'Hlt2DoubleLonglived' : 2800.
                                    ,'Hlt2DoublePSLonglived' : 2000.
                                    }
            ,  "MinSumpt"     :{ 'Hlt2RV2P' : 0.
                                 , 'Hlt2SingleLonglived' : 1000.
                                 , 'Hlt2SingleDown' : 1000.
                                 , 'Hlt2PreSelSingleDown' : 1000.
                                 , 'Hlt2SingleLonglivedHighFD' : 1000.
                                 , 'Hlt2SingleLonglivedHighMass' : 1000
                                 , 'Hlt2DoubleLonglived' : 1000.
                                 , 'Hlt2SinglePSLonglived' : 0.
                                 , 'Hlt2DoublePSLonglived' : 0.
                                 , 'Hlt2SinglePSLonglivedHighMass' : 1000.
                                 , 'Hlt2SinglePSLonglivedHighFD' : 1000.
                                 , 'Hlt2SinglePSLonglivedMV' :0.
                                 }
            
            ,  "ComputeMatterVeto" : { 'Hlt2RV2P' : True
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
                                 }
            , "PostScale" : { 'Hlt2RV2P' : 1.
                              , 'Hlt2SingleLonglived' : 1.
                              , 'Hlt2SingleDown' : 1.
                              , 'Hlt2SingleLonglivedHighFD' : 1.
                              , 'Hlt2SingleLonglivedHighMass' : 1.
                              , 'Hlt2DoubleLonglived' : 1.
                              , 'Hlt2SinglePSLonglived' : 0.0005
                              , 'Hlt2DoublePSLonglived' : 0.01
                              , 'Hlt2SinglePSLonglivedHighMass' : 1.
                              , 'Hlt2SinglePSLonglivedHighFD' : 1.
                              , 'Hlt2SinglePSLonglivedMV' : 0.001
                              }  
            }}   
                  )

        return d
    
