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
            'Hlt2DiMuonUnbiasedJPsi',
            'Hlt2DiMuonUnbiasedPsi2S',
            'Hlt2DiMuonUnbiasedBmm',
#            'Hlt2BiasedDiMuonRefined',
            'Hlt2BiasedDiMuonMass',
#            'Hlt2BiasedDiMuonIP',
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

        from Hlt2Lines.Hlt2InclusiveMuonLines  import Hlt2InclusiveMuonLinesConf
        from Hlt2Lines.Hlt2InclusiveDiMuonLines  import Hlt2InclusiveDiMuonLinesConf
        d.update( { Hlt2InclusiveDiMuonLinesConf : { 
            ## Cut values
            'UnbiasedDiMuonPt'         : 0.0      # in MeV
            ,'UnbiasedDiMuonMuPt'      : 0.0      # in MeV
            ,'UnbiasedDiMuonLowPt'     : 0.0
            ,'UnbiasedDiMuonLowMuPt'   : 0.0
            ,'UnbiasedJPsiPt'          : 0.0      # MeV
            ,'UnbiasedJPsiMassWindow'  : 120      # MeV
            ,'UnbiasedPsi2SPt'         : 0.0      # MeV
            ,'UnbiasedPsi2SMuPt'       : 500      # MeV
            ,'UnbiasedPsi2SMassWindow' : 120      # MeV
            ,'UnbiasedBmmMinMass'      : 5000     # MeV
            , 'Prescale'   : { 'Hlt2UnbiasedDiMuon'          : 1.00
                               , 'Hlt2UnbiasedDiMuonLowMass' : 0.1
                               }           
            }
              , Hlt2InclusiveMuonLinesConf : {
            'SingleMuonPt'        : 1000      # MeV
            ,'SingleMuonIP'        : 0.08     # mm
            ,'SingleMuonHighPt'    : 10000     # MeV
            ,'Prescale'   : { 'Hlt2MuonFromHLT1'       : 0.01   
                              ,'Hlt2SingleMuon'        : 1.0
                              ,'Hlt2SingleHighPTMuon'  : 1.0
                              }
            }
                    }
                  )
        
        return d
    
