class Muons_Retention50_Oct10 :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of exclusive)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J. Albrecht
    @date 2010-08-18
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [

            'Hlt2MuonFromHLT1',
            'Hlt2SingleMuon',
            'Hlt2SingleHighPTMuon',
            'Hlt2MuTrack',
            'Hlt2MuTrackNoIP',
            'Hlt2DiMuonUnbiasedJPsi',
            'Hlt2DiMuonUnbiasedPsi2S',
            'Hlt2DiMuonUnbiasedBmm',
            'Hlt2DiMuonUnbiasedZmm',
            'Hlt2DiMuonBiasedJPsi',
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
            'Hlt2PromptJPsi',
            'Hlt2PromptJPsiHighPT'
            
            ]
            
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Hlt2InclusiveMuonLines  import Hlt2InclusiveMuonLinesConf
        from Hlt2Lines.Hlt2InclusiveDiMuonLines  import Hlt2InclusiveDiMuonLinesConf
        d.update( { Hlt2InclusiveDiMuonLinesConf : { 
            ## Cut values
            'UnbiasedDiMuonMinMass'    : 2900      # MeV
            ,'UnbiasedDiMuonPt'        : 0.0      # MeV
            ,'UnbiasedDiMuonMuPt'      : 0.0      # MeV
            ,'UnbiasedDiMuonLowPt'     : 0.0      # MeV
            ,'UnbiasedDiMuonLowMuPt'   : 0.0      # MeV
            ,'UnbiasedJPsiPt'          : 0.0      # MeV
            ,'UnbiasedJPsiMuPt'        : 0.0      # MeV
            ,'UnbiasedJPsiMassWindow'  : 120      # MeV
            ,'UnbiasedPsi2SPt'         : 0.0      # MeV
            ,'UnbiasedPsi2SMuPt'       : 0.0      # MeV
            ,'UnbiasedPsi2SMassWindow' : 120      # MeV
            ,'UnbiasedBmmMinMass'      : 4700     # MeV
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
            , 'Prescale'   : { 'Hlt2UnbiasedDiMuon'          : 0.2
                               , 'Hlt2UnbiasedDiMuonLowMass' : 0.002
                               ,'Hlt2DiMuonDY1'              :  0.1
                               ,'Hlt2DiMuonDY2'              :  1
                               ,'Hlt2DiMuonSameSign'         :  0.1
                               ,'Hlt2BiasedDiMuonIP'         :  1
                               }           
            }}
                  )

        d.update( { Hlt2InclusiveMuonLinesConf : {
            'SingleMuonPt'         : 1300      # MeV
            ,'SingleMuonIP'        : 0.08     # mm
            ,'SingleMuonHighPt'    : 10000     # MeV
            ,'MuTrackMuPt'         : 1000       # MeV
            ,'MuTrackTrPt'         : 600       # MeV
            ,'MuTrackTrChi2'       : 5.0       # dimensionless
            ,'MuTrackChi2'         : 15.0      # dimensionless
            ,'MuTrackMuIPChi2'     : 9.0       # dimensionless
            ,'MuTrackTrIPChi2'     : 9.0       # dimensionless
            ,'MuTrackDoca'         : 0.200     # mm
            ,'MuTrackFDChi2'       : 64.0      # dimensionless
            ,'MuTrackMass'         : 2000      # MeV
            ,'MuTrackSumPt'        : 2200      # MeV
            ,'MuTrackCorMass'      : 7000.0    # MeV
            ,'MuTrackDIRA'         : 0.995       # dimensionless
            ,'Prescale'   : { 'Hlt2MuonFromHLT1'       : 0.001
                              ,'Hlt2SingleMuon'        : 0.02 # make sure that Hlt1SingleMuonNoIPL0 * Hlt2SingleMuon = 0.02
                              ,'Hlt2SingleHighPTMuon'  : 1.0
                              ,'Hlt2IncMuTrack'        : 1.0
                              ,'Hlt2MuTrackNoIP'       : 0.
                              }
            }}
                  )
        
        return d
    


