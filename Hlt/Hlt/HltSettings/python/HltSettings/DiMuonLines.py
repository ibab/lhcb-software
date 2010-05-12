class DiMuonLines :
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

        lines = [
            'Hlt2SingleMuonLow',
            'Hlt2IncMuTrack',
            'Hlt2IncMuTrack4Jpsi', 
            'Hlt2IncMuTrackLow', 
            'Hlt2IncMuTrackLowMass', 
            'Hlt2IncMuTrackLowMassLow', 
            'Hlt2IncMuTrackLowMassMid', 
            'Hlt2IncMuTrackMid', 
            'Hlt2DiMuonUnbiasedJPsi',
            'Hlt2DiMuonUnbiasedPsi2S',
            'Hlt2DiMuonUnbiasedBmm',
            'Hlt2DiMuonUnbiasedZmm',
            'Hlt2DiMuonDY1',
            'Hlt2DiMuonDY2',
            'Hlt2DiMuonDY3',
            'Hlt2DiMuonDY4',
            'Hlt2BiasedDiMuonSimple',
            'Hlt2BiasedDiMuonRefined',
            'Hlt2BiasedDiMuonMass',
            'Hlt2BiasedDiMuonIP',
#            'Hlt2DiMuonUnbiasedJPsiLow',
            'Hlt2UnbiasedDiMuon',  
            'Hlt2UnbiasedDiMuonLowMass',
            'Hlt2DiMuonSameSign',
          
            ]
            
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }


        from Hlt2Lines.Hlt2InclusiveDiMuonLines  import Hlt2InclusiveDiMuonLinesConf
        d.update( { Hlt2InclusiveDiMuonLinesConf : { 
            ## Cut values
            'UnbiasedDiMuonPt'         : 0.0      # in MeV
            ,'UnbiasedJPsiPt'          : 0.0      # MeV
            ,'UnbiasedJPsiMassWindow'  : 120      # MeV
            ,'UnbiasedPsi2SPt'         : 0.0      # MeV
            ,'UnbiasedPsi2SMuPt'       : 500      # MeV
            ,'UnbiasedPsi2SMassWindow' : 120      # MeV
            ,'UnbiasedBmmMinMass'      : 5000     # MeV
            ,'DYPt'                    : 0.0      # MeV
            ,'DY1MinMass'              : 2500      # MeV
            ,'DY1MaxMass'              : 6000      # MeV
            ,'DY2MinMass'              : 5000      # MeV
            ,'DY2MaxMass'              :12000      # MeV
            ,'DY3MinMass'              :10000      # MeV
            ,'DY3MaxMass'              :25000      # MeV
            ,'DY4MinMass'              :20000      # MeV
            ,'DY4MaxMass'              :50000      # MeV
            ,'UnbiasedZmmMinMass'      :40000      # MeV
            ,'UnbiasedZmmPt'           :  0.0      # MeV
            , 'Prescale'   : { 'Hlt2UnbiasedDiMuon'          : 1.00
                               , 'Hlt2UnbiasedDiMuonLowMass' : 0.1
                               ,'Hlt2DiMuonDY1'              :  1
                               ,'Hlt2DiMuonDY2'              :  1
                               ,'Hlt2DiMuonSameSign'         :  1
                               }           
            }}
                  )

        return d
