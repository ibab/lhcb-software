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

            'Hlt2MuonFromHLT1',
            'Hlt2SingleMuon',
            #'Hlt2SingleMuonRateLimited',
            'Hlt2SingleMuonHighPT',
            'Hlt2SingleMuonLowPT',

            'Hlt2DiMuon',  
            'Hlt2DiMuonLowMass',
            #'Hlt2DiMuonSameSign',

            # control rate via: 1) mass: 120 --> 70,
            # then rediscuss with beta_s DO NOT INCREASE PT!!
            'Hlt2DiMuonJPsi',
            'Hlt2DiMuonJPsiHighPT',
            'Hlt2DiMuonPsi2S',
            'Hlt2DiMuonB',
            'Hlt2DiMuonZ',

            # control rate via: DLS cut, dimuon PT
            'Hlt2DiMuonDetached',
            'Hlt2DiMuonDetachedJPsi',
            'Hlt2DiMuonDetachedHeavy',

            #'Hlt2DiMuonNoPV',#remove on request buy Dermot Morgan, May3, 2011

            # control rate via: prescale
            'Hlt2DiMuonDY1',
            'Hlt2DiMuonDY2',
            'Hlt2DiMuonDY3',
            'Hlt2DiMuonDY4',

            # control rate via flight distance chi2, sum IP chi2 cuts - avoid PT cuts
            #'Hlt2MuTrack1',
            #'Hlt2MuTrack2',
            #'Hlt2MuTrack3',
             
            'Hlt2TriMuonDetached',
            'Hlt2TriMuonTau',
      
            'Hlt2TFBc2JpsiMuX',
            'Hlt2TFBc2JpsiMuXSignal'
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
        from Hlt2Lines.Hlt2MuNTrackLines  import Hlt2MuNTrackLinesConf
        from Hlt2Lines.Hlt2CharmRareDecayLines  import Hlt2CharmRareDecayLinesConf
        from Hlt2Lines.Hlt2Dst2D2XXLines import Hlt2Dst2D2XXLinesConf

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
            ,'UnbiasedJPsiHighPt'      : 3000     # MeV
            ,'UnbiasedPsi2SPt'         : 0.0      # MeV
            ,'UnbiasedPsi2SMuPt'       : 0.0      # MeV
            ,'UnbiasedPsi2SMassWindow' : 120      # MeV
            ,'UnbiasedBmmMinMass'      : 4700     # MeV
            ,'UnbiasedBmmVertexChi2'   : 10
            ,'UnbiasedZmmMinMass'      :40000     # MeV
            ,'UnbiasedZmmPt'           :  0.0     # MeV
            ,'DetachedDiMuonPt'        : 1500     #MeV
            ,'DetachedDiMuonDLS'       :   7
            ,'DetachedDiMuonMinMass'   : 1000 #MeV
            ,'DetachedDiMuIPChi2'      :   9
            ,'DetachedHeavyDiMuonMinMass': 2950 # MeV
            ,'DetachedHeavyDiMuonDLS'  :   5
            ,'DetachedJPsiDLS'           : 3
            ,'DetachedJPsiMassWindow'    : 120 #MeV 

            , 'Prescale'   : { 'Hlt2DiMuon'          : 0.01
                               , 'Hlt2DiMuonLowMass' : 0.002
                               , 'Hlt2DiMuonDY1'              :  0.05
                               ,'Hlt2DiMuonDY2'              :  0.2
                               }           
            }}
                  )

        d.update( { Hlt2InclusiveMuonLinesConf : {
            'SingleMuonPt'         : 1300      # MeV
            ,'SingleMuonIP'        : 0.5     # mm
            , 'SingleMuonIPChi2'   : 200
            ,'SingleMuonHighPt'    : 10000     # MeV
            ,'Prescale'   : { 'Hlt2MuonFromHLT1'       : 0.0
                              ,'Hlt2SingleMuon'        : 0.5
                              ,'Hlt2SingleMuonRateLimited'     : 'RATE(200)'
                              ,'Hlt2SingleHighPTMuon'  : 1.0
                              ,'Hlt2SingleMuonLowPT'   : 0.02
                              }
            }}
                  )
           
        return d
    


