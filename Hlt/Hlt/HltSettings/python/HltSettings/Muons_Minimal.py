class Muons_Minimal :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of exclusive)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J. Albrecht
    @date 2010-05-28
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
            'Hlt2DiMuonUnbiasedJPsi',
            'Hlt2DiMuonUnbiasedPsi2S',
            'Hlt2DiMuonUnbiasedBmm',
            'Hlt2BiasedDiMuonMass',
            'Hlt2BiasedDiMuonIP',
            'Hlt2UnbiasedDiMuon',  
            'Hlt2UnbiasedDiMuonLowMass'
            ]
            
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Hlt2InclusiveMuonLines    import Hlt2InclusiveMuonLinesConf
        from Hlt2Lines.Hlt2InclusiveDiMuonLines  import Hlt2InclusiveDiMuonLinesConf
        d.update( { Hlt2InclusiveDiMuonLinesConf : { 
            ## Cut values
            'UnbiasedDiMuonPt'         : 0.0      # in MeV
            ,'UnbiasedDiMuonMuPt'      : 0.0      # in MeV
            ,'UnbiasedDiMuonLowPt'     : 0.0
            ,'UnbiasedDiMuonLowMuPt'   : 0.0
            ,'UnbiasedJPsiPt'          : 0.0      # MeV
            ,'UnbiasedJPsiMuPt'        : 0.0      # MeV
            ,'UnbiasedJPsiMassWindow'  : 120      # MeV
            ,'UnbiasedPsi2SPt'         : 0.0      # MeV
            ,'UnbiasedPsi2SMuPt'       :   0      # MeV
            ,'UnbiasedPsi2SMassWindow' : 120      # MeV
            ,'UnbiasedBmmMinMass'      : 4700     # MeV
            , 'Prescale'   : { 'Hlt2UnbiasedDiMuon'          : 1.0
                               , 'Hlt2UnbiasedDiMuonLowMass' : 0.1
                               ,'Hlt2BiasedDiMuonIP'         :  1
                               }           
            }
              , Hlt2InclusiveMuonLinesConf : {
            'SingleMuonPt'        : 1300      # MeV
            ,'SingleMuonIP'        : 0.08     # mm
            ,'SingleMuonHighPt'    : 10000     # MeV
            ,'MuTrackMuPt'         : 1000      # MeV
            ,'MuTrackTrPt'         : 600       # MeV
            ,'MuTrackMuIP'         : 0.08     # mm
            ,'MuTrackTrIP'         : 0.08     # mm
            ,'MuTrackDoca'         : 0.2     # mm
            ,'MuTrackDz'           : 1.0       # mm
            ,'MuTrackMass'         : 2000      # MeV
            ,'MuTrackPoint'        : 0.4       # dimensionless
            ,'Prescale'   : { 'Hlt2MuonFromHLT1'     : 0.01   
                            ,'Hlt2SingleMuon'        : 1.0
                            ,'Hlt2SingleHighPTMuon'  : 1.0
                            ,'Hlt2IncMuTrack'        : 1.0
                            }
            }
                    }
                  )
        
        return d
    
