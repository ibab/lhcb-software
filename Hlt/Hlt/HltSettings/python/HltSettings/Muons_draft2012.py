class Muons_draft2012 :
    """
    Threshold settings for Hlt2 muon lines: setting for high lumi running in 2012
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J. Albrecht
    @date 2012-03-01
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [

            #'Hlt2MuonFromHLT1',
            'Hlt2SingleMuon',
            #'Hlt2SingleMuonRateLimited',
            'Hlt2SingleMuonHighPT',
            'Hlt2SingleMuonVHighPT',
            'Hlt2SingleMuonLowPT',

            #'Hlt2DiMuon',  
            #'Hlt2DiMuonLowMass',
            #'Hlt2DiMuonSameSign',

            # control rate via: 1) mass: 120 --> 70,
            # then rediscuss with beta_s DO NOT INCREASE PT!!
            'Hlt2DiMuonJPsi',
            'Hlt2DiMuonJPsiHighPT',
            'Hlt2DiMuonPsi2S',
            'Hlt2DiMuonPsi2SHighPT',
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
                       
            'Hlt2TriMuonDetached',
            'Hlt2TriMuonTau',
            'Hlt2DiMuonAndMuon',
            'Hlt2DoubleDiMuon',
                 
            'Hlt2TFBc2JpsiMuX',
            'Hlt2TFBc2JpsiMuXSignal',

            #JPSi+X lines 
            'Hlt2DiMuonAndGamma',
            'Hlt2DiMuonAndD0',
            'Hlt2DiMuonAndDp',
            'Hlt2DiMuonAndDs',
            'Hlt2DiMuonAndLc'

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
            'TrChi2Tight'              : 4
            ,'UnbiasedDiMuonMinMass'    : 2900      # MeV
            ,'UnbiasedDiMuonPt'        : 0.0      # MeV
            ,'UnbiasedDiMuonMuPt'      : 0.0      # MeV
            ,'UnbiasedDiMuonLowPt'     : 0.0      # MeV
            ,'UnbiasedDiMuonLowMuPt'   : 0.0      # MeV
            ,'UnbiasedJPsiPt'          : 0.0      # MeV
            ,'UnbiasedJPsiMuPt'        : 0.0      # MeV
            ,'UnbiasedJPsiMassWindow'  : 120      # MeV
            ,'UnbiasedJPsiHighPt'      : 2000     # MeV
            ,'UnbiasedJPsiHighPTMassWindow' : 100
            ,'UnbiasedPsi2SPt'         :   0      # MeV
            ,'UnbiasedPsi2SMuPt'       : 0.0      # MeV
            ,'UnbiasedPsi2SMassWindow' : 120      # MeV
            ,'UnbiasedPsi2SPtHigh'     : 3500      #MeV
            ,'UnbiasedBmmMinMass'      : 4700     # MeV
            ,'UnbiasedBmmVertexChi2'   : 10
            ,'UnbiasedZmmMinMass'      :40000     # MeV
            ,'UnbiasedZmmPt'           :  0.0     # MeV
            ,'DetachedDiMuonPt'        :   600     #MeV 
            ,'DetachedDiMuonDLS'       :   7
            ,'DetachedDiMuonMuPt'      : 300      #Mev 
            ,'DetachedDiMuonMinMass'   :   0 #MeV 
            ,'DetachedDiMuIPChi2'      :   25
            ,'DetachedDiMuonVertexChi2':   8
            ,'DetachedHeavyDiMuonMinMass': 2950 # MeV
            ,'DetachedHeavyDiMuonDLS'  :   5
            ,'DetachedJPsiDLS'           : 3
            ,'DetachedJPsiMassWindow'    : 120 #MeV 

            ,'MultiMu_GoodMuon_Chi2_IP'   : 4
            ,'MultiMu_TightMuon_Chi2_IP'  : 36
            ,'MultiMu_DiMuon_DLS'         : 6
            ,'MultiMu_Tau3Mu_MassWindow'  : 300   #MeV
            ,'MultiMu_Tau3Mu_max_PT'      : 0
           
            , 'Prescale'   : { 'Hlt2DiMuon'                  :  0   
                               , 'Hlt2DiMuonLowMass'         :  0   
                               , 'Hlt2DiMuonJPsi'            :  1   #JA 20120301: try to release prescale from 0.2
                               , 'Hlt2DiMuonPsi2S'           :  1
                               , 'Hlt2DiMuonDY1'             :  0.005
                               , 'Hlt2DiMuonDY2'             :  0.03
                               , 'Hlt2DiMuonAndGamma'        :  0
                               }           
            }}
                  )

        d.update( { Hlt2InclusiveMuonLinesConf : {
            'SingleMuonPt'         : 1300      # MeV
            ,'SingleMuonIP'        : 0.5     # mm
            , 'SingleMuonIPChi2'   : 200
            ,'SingleMuonHighPt'    : 10000     # MeV
            ,'Prescale'   : { 'Hlt2MuonFromHLT1'       : 0.0
                              ,'Hlt2SingleMuon'        : 1 #JA 20120301: try to release prescale from 0.5
                              ,'Hlt2SingleMuonRateLimited'     : 'RATE(200)'
                              ,'Hlt2SingleHighPTMuon'  : 1.0
                              ,'Hlt2SingleMuonLowPT'   : 0.002
                              }
            }}
                  )
           
        return d
    


