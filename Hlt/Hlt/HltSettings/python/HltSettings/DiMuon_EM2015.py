from GaudiKernel.SystemOfUnits import GeV, mm, MeV 

class DiMuon_EM2015(object) :
    """
    Threshold settings for Hlt2 DiMuon lines: EM 2015
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author C. Linn
    @date 2015-05-19
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2DiMuonJPsiTurbo',
            'Hlt2DiMuonPsi2STurbo',
            'Hlt2DiMuonBTurbo',
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
                   'JPsi' :                {'MassWindow'  :   150 * MeV,
                                            'Pt'          :     0 * MeV,
                                            'MuPt'        :     0 * MeV,
                                            'VertexChi2'  :    25},
                   'Psi2S' :               {'MassWindow'  :   150 * MeV,
                                            'Pt'          :     0 * MeV,
                                            'MuPt'        :     0 * MeV,
                                            'VertexChi2'  :    25},
                   'B' :                   {'MinMass'     :  4700 * MeV,
                                            'VertexChi2'  :    10},
                   'Prescale'   : {'Hlt2DiMuonJPsi'    :  1.0,
                                   'Hlt2DiMuonB'       :  1.0,
                                   'Hlt2DiMuonPsi2S'   :  1.0}
                  }
                 })
        
        return d
    


