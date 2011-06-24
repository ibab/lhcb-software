class CharmLeptonic_July2011 :
    """
    Threshold settings for Hlt2 muon lines: minimal July for 2011
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J. Albrecht/V. Gligorov
    @date 2011-03-29
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [

            # Rare decays
            'Hlt2CharmRareDecayD02MuMu',    
            "Hlt2Dst2PiD02PiPi",            
            "Hlt2Dst2PiD02KPi",             
            "Hlt2Dst2PiD02MuMu",            
            "Hlt2Dst2PiD02KMu",  
  
            #Semileptonics
            "Hlt2CharmSemilepD02HMuNu_D02KMuNu",
            "Hlt2CharmSemilepD02HMuNu_D02KMuNuWS",
            "Hlt2CharmSemilepD02HMuNu_D02PiMuNu",
            "Hlt2CharmSemilepD02HMuNu_D02PiMuNuWS",
            "Hlt2CharmSemilepD02HMuNu_D02KMuNuTight",

            # mumuh and mumuhh 
            "Hlt2CharmSemilepD02HHMuMu",
            "Hlt2CharmSemilepD02HHMuMuWideMass",
            "Hlt2CharmSemilepD2HMuMu",
            "Hlt2CharmSemilepD2HMuMuWideMass",
            "Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuons",
            "Hlt2CharmSemilepD02HHMuMuHardHadronsAndMuonsWideMass",
            "Hlt2CharmSemilepD02HHMuMuHardHadronsSoftMuons",
            "Hlt2CharmSemilepD02HHMuMuHardHadronsSoftMuonsWideMass"


            ]
            
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Hlt2CharmRareDecayLines  import Hlt2CharmRareDecayLinesConf
        from Hlt2Lines.Hlt2Dst2D2XXLines import Hlt2Dst2D2XXLinesConf
        from Hlt2Lines.Hlt2CharmSemilepD02HMuNuLines import Hlt2CharmSemilepD02HMuNuLinesConf

        d.update({Hlt2CharmRareDecayLinesConf : { 
                          'D02MuMuMinDaughterPt'            : 1.0 # GeV
                        , 'D02MuMuMinLifeTime'              : 0.1 # ps
                        , 'D02PiPiForD02MuMuMinLifeTime'    : 0.3 # ps
                        , 'Prescale'                        : { 
                            'Hlt2CharmRareDecayD02MuMu'           : 1.0 
                            }
            }})

        d.update({Hlt2Dst2D2XXLinesConf : { 
                    'Prescale'                        : { 
                        'Hlt2Dst2PiD02PiPi'           : 0.03 
                        , 'Hlt2Dst2PiD02KPi'          : 0.01
                        , 'Hlt2Dst2PiD02MuMu'         : 1
                        , 'Hlt2Dst2PiD02KMu'          : 0.15
                          }
            }})

        d.update({ Hlt2CharmSemilepD02HMuNuLinesConf : {
               'Prescale' : {
                   'Hlt2CharmSemilepD02HMuNu_D02KMuNu'            : 0.05
                ,  'Hlt2CharmSemilepD02HMuNu_D02KMuNuWS'     : 0.01
                ,  'Hlt2CharmSemilepD02HMuNu_D02PiMuNu'           : 0.05
                ,  'Hlt2CharmSemilepD02HMuNu_D02PiMuNuWS'     : 0.01 }
        }})
        
        return d
    


