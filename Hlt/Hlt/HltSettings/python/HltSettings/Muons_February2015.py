from GaudiKernel.SystemOfUnits import GeV, MeV 

class Muons_February2015(object) :
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
            'Hlt2DiMuonDetachedPsi2S',

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

        from Hlt2Lines.Hlt2InclusiveMuonLines import Hlt2InclusiveMuonLinesConf
        from Hlt2Lines.Hlt2InclusiveDiMuonLines import Hlt2InclusiveDiMuonLinesConf
        from Hlt2Lines.DiMuon.Lines import DiMuonLines
        from Hlt2Lines.Hlt2Dst2D2XXLines import Hlt2Dst2D2XXLinesConf

        d.update({DiMuonLines :
                  {'Common' :              {'TrChi2'      :   10,
                                            'TrChi2Tight' :    4},
                   'DiMuon' :              {'MinMass'     :  2900 * MeV,
                                            'Pt'          :     0 * MeV,
                                            'MuPt'        :     0 * MeV,
                                            'VertexChi2'  :    25},
                   'JPsi' :                {'MassWindow'  :   120 * MeV,
                                            'Pt'          :     0 * MeV,
                                            'MuPt'        :     0 * MeV,
                                            'VertexChi2'  :    25},
                   'JPsiHighPT' :          {'Pt'          :  2000 * MeV,
                                            'MuPt'        :     0 * MeV,
                                            'VertexChi2'  :    25,
                                            'MassWindow'  :   120 * MeV},
                   'Psi2S' :               {'MassWindow'  :   120 * MeV,
                                            'Pt'          :  2000 * MeV,
                                            'MuPt'        :     0 * MeV,
                                            'VertexChi2'  :    25},
                   'Psi2SHighPT' :         {'Pt'          :  3500 * MeV,
                                            'MassWindow'  :   120 * MeV,
                                            'MuPt'        :     0 * MeV,
                                            'VertexChi2'  :    25},
                   'B' :                   {'MinMass'     :  4700 * MeV,
                                            'VertexChi2'  :    10},
                   'Z' :                   {'MinMass'     : 40000 * MeV,
                                            'Pt'          :     0 * MeV},
                   'Detached' :            {'IPChi2'      :    25,
                                            'DLS'         :     7},
                   'DetachedHeavy' :       {'MinMass'     :  2950 * MeV,
                                            'Pt'          :     0 * MeV,
                                            'MuPt'        :   300 * MeV,
                                            'VertexChi2'  :    25,
                                            'IPChi2'      :     0,
                                            'DLS'         :     5},
                   'DetachedJPsi' :        {'DLS'         :     3},
                   'DetachedPsi2S' :       {'DLS'         :     3},
                   'Prescale'   : {'Hlt2DiMuon'        :  0,   
                                   'Hlt2DiMuonJPsi'    :  0.2,
                                   'Hlt2DiMuonPsi2S'   :  0.1}
                  }
                 })
        
        d.update( { Hlt2InclusiveDiMuonLinesConf : { 
            ## Cut values
            'TrChi2'                   : 10
            ,'TrChi2Tight'              : 4
            ,'UnbiasedJPsiMassWindow'  :  120
            ,'UnbiasedJPsiPt'          :    0
            ,'UnbiasedJPsiMuPt'        :    0
            ,'UnbiasedJPsiVertexChi2'  :   25
            ,'MultiMu_GoodMuon_Chi2_IP'   : 9
            ,'MultiMu_TightMuon_Chi2_IP'  : 36
            ,'MultiMu_DiMuon_DLS'         : 6
            ,'MultiMu_Tau3Mu_MassWindow'  : 300   #MeV
            ,'MultiMu_Tau3Mu_max_PT'      : 0
            ,'MultiMu_Tau3Mu_ctau'        : 75
           
            , 'Prescale'   : {   'Hlt2DiMuonDY1'             :  0.005
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
                              ,'Hlt2SingleMuon'        : 0.5
                              ,'Hlt2SingleHighPTMuon'  : 1.0
                              ,'Hlt2SingleMuonLowPT'   : 0.002
                              }
            }}
                  )
           
        return d
    


