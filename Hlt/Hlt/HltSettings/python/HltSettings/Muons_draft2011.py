class Muons_draft2011 :
    """
    Threshold settings for Hlt2 muon lines: minimal draft for 2011
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J. Albrecht
    @date 2010-12-02
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [

            #'Hlt2MuonFromHLT1',
            'Hlt2SingleMuon',
            'Hlt2SingleMuonHighPT',

            'Hlt2DiMuon',  
            'Hlt2DiMuonLowMass',
            #'Hlt2DiMuonSameSign',

            # control rate via: 1) mass: 120 --> 70,
            # then rediscuss with beta_s DO NOT INCREASE PT!!
            'Hlt2DiMuonJPsi',
            'Hlt2DiMuonPsi2S',
            'Hlt2DiMuonB',
            'Hlt2DiMuonZ',

            # control rate via: DLS cut, dimuon PT
            'Hlt2DiMuonDetached',
            'Hlt2DiMuonDetachedJPsi',
            'Hlt2DiMuonDetachedHeavy',

            # control rate via: prescale
            'Hlt2DiMuonDY1',
            'Hlt2DiMuonDY2',
            'Hlt2DiMuonDY3',
            'Hlt2DiMuonDY4'
                 
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
            ,'UnbiasedZmmMinMass'      :40000     # MeV
            ,'UnbiasedZmmPt'           :  0.0     # MeV
            ,'DetachedDiMuonPt'        : 1500     #MeV
            ,'DetachedDiMuonDLS'       :   7

            
            #,'DYPt'                    : 0.0      # MeV

            , 'Prescale'   : { 'Hlt2DiMuon'          : 0.1
                               , 'Hlt2DiMuonLowMass' : 0.002
                               , 'Hlt2DiMuonDY1'              :  0.1
                               #,'Hlt2DiMuonDY2'              :  1
                               #,'Hlt2BiasedDiMuonIP'         :  1
                               }           
            }}
                  )

        d.update( { Hlt2InclusiveMuonLinesConf : {
            'SingleMuonPt'         : 1300      # MeV
            ,'SingleMuonIP'        : 0.11     # mm
            ,'SingleMuonHighPt'    : 10000     # MeV
          ##   ,'MuTrackMuPt'         : 1000       # MeV
##             ,'MuTrackTrPt'         : 600       # MeV
##             ,'MuTrackTrChi2'       : 5.0       # dimensionless
##             ,'MuTrackChi2'         : 15.0      # dimensionless
##             ,'MuTrackMuIPChi2'     : 9.0       # dimensionless
##             ,'MuTrackTrIPChi2'     : 9.0       # dimensionless
##             ,'MuTrackDoca'         : 0.200     # mm
##             ,'MuTrackFDChi2'       : 64.0      # dimensionless
##             ,'MuTrackMass'         : 2000      # MeV
##             ,'MuTrackSumPt'        : 2200      # MeV
##             ,'MuTrackCorMass'      : 7000.0    # MeV
##             ,'MuTrackDIRA'         : 0.995       # dimensionless
            ,'Prescale'   : { 'Hlt2MuonFromHLT1'       : 0.0001
                              ,'Hlt2SingleMuon'        : 0.02 # make sure that Hlt1SingleMuonNoIPL0 * Hlt2SingleMuon = 0.02
                              ,'Hlt2SingleHighPTMuon'  : 1.0
                           #   ,'Hlt2IncMuTrack'        : 1.0

                              }
            }}
                  )
        
        return d
    


