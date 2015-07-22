from GaudiKernel.SystemOfUnits import GeV, mm, MeV 

class DiMuon_25ns_August2015(object) :
    """
    Threshold settings for Hlt2 DiMuon lines
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author F. Dettori
    @date 2015-07-019
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            # control rate via: 1) mass: 120 --> 70,
            # DO NOT INCREASE PT!
            'Hlt2DiMuonJPsi',
            'Hlt2DiMuonJPsiHighPT',
            'Hlt2DiMuonPsi2S',
            'Hlt2DiMuonPsi2SHighPT',
            
            # Do not change 
            'Hlt2DiMuonB',
            'Hlt2DiMuonZ',

            # Control rate via IP cut
            'Hlt2DiMuonSoft',

            # control rate via: DLS cut, dimuon PT
            'Hlt2DiMuonDetached',
            'Hlt2DiMuonDetachedJPsi',
            'Hlt2DiMuonDetachedHeavy',
            'Hlt2DiMuonDetachedPsi2S',

            # Turbo lines 
            'Hlt2DiMuonJPsiTurbo',
            'Hlt2DiMuonPsi2STurbo',
            'Hlt2DiMuonBTurbo'
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
                                            'TrChi2Tight' :    5},
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
                                            'VertexChi2'  :    25},
                   'Z' :                   {'MinMass'     : 40000 * MeV,
                                            'Pt'          :     0 * MeV},
                   'Detached' :            {'IPChi2'      :     9,
                                            'DLS'         :     7},
                   'DetachedHeavy' :       {'MinMass'     :  2950 * MeV,
                                            'Pt'          :     0 * MeV,
                                            'MuPt'        :   300 * MeV,
                                            'VertexChi2'  :    25,
                                            'IPChi2'      :     0,
                                            'DLS'         :     5},
                   'Soft' :          {'IP'         :   0.3 * mm ,
                                      'IPChi2Min'  :   1.5,
                                      'IPChi2Max'  :   1500,
                                      'TTHits'     :      -1,
                                      'TRACK_TRGHOSTPROB_MAX': 0.4,
                                      'MaxMass'    :   1000 * MeV,
                                      'VertexChi2' :    25,
                                      'Rho'        :     3,
                                      'SVZ'        :   650,
                                      'doca'       :   0.3,
                                      'MinVDZ'     :     0,
                                      'MinBPVDira' :     0,
                                      'MaxIpDistRatio':  1./60,
                                      'cosAngle'   : 0.999998
                                    },                    
                   'DetachedJPsi' :        {'DLS'         :     3},
                   'DetachedPsi2S' :       {'DLS'         :     3},

                   # Turbo lines
                   'JPsiTurbo' :          {'MassWindow' :   120 * MeV,
                                           'Pt'         :     0 * MeV,
                                           'MuPt'       :     0 * MeV,
                                           'VertexChi2' :    25},
                   'Psi2STurbo' :    {'MassWindow' :   120 * MeV,
                                      'Pt'         :  2000 * MeV,
                                      'MuPt'       :     0 * MeV,
                                      'VertexChi2' :    25},
                   'BTurbo' :             {'MinMass'    :   4700 * MeV,
                                           'VertexChi2' :    25},
                   
                   # Prescales
                   'Prescale'   : {'Hlt2DiMuon'        :  0,   
                                   'Hlt2DiMuonJPsi'    :  0.2,
                                   'Hlt2DiMuonPsi2S'   :  0.1}
                  }
                 })
        
        return d
    


