from GaudiKernel.SystemOfUnits import GeV, mm, MeV 

class Muons_March2015(object) :
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
            'Hlt2EWDiMuonDY1',
            'Hlt2EWDiMuonDY2',
            'Hlt2EWDiMuonDY3',
            'Hlt2EWDiMuonDY4',
                                   ]
            
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }

        from Hlt2Lines.DiMuon.Lines     import DiMuonLines            
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
        
        from Hlt2Lines.EW.Lines import EWLines
        d.update({EWLines : {'Prescale' : {'Hlt2DiMuonDY1' :  0.005,
                                           'Hlt2DiMuonDY2' :  0.03}
                            }
                 })

        from Hlt2Lines.SingleMuon.Lines import SingleMuonLines
        d.update({SingleMuonLines : {'Prescale'   : { 'Hlt2SingleMuon'      : 0.5
                                                    , 'Hlt2SingleHighPT'    : 1.0
                                                    , 'Hlt2SingleMuonLowPT' : 0.002}
                                    }
                 })
           
        return d
    


