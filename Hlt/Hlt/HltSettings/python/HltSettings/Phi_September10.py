#
# High cuts (for hadronic)
#

# the list of lines is the same 

__phiLines__ = [ 'Hlt2IncPhi'#,
                 #'Hlt2IncPhiRobust',
                 #'Hlt2IncPhiTrackFit',
                 #'Hlt2IncPhiSidebands' 
                ]
#
# Low cuts (for charming and leptonic)
#

class Phi_September10 :
    """
    Threshold settings for the nominal running of Hlt2 while
    leaving the PID cut to -5 for commissioning
    
    (see presentation on Trigger & Physics meeting May 2009)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven, K. Vervink
    @date 2009-08-05
    """
    
    __all__ = ( 'ActiveHlt2Lines', 'Thresholds' )
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        from Hlt2Lines.Hlt2InclusivePhiLines import Hlt2InclusivePhiLinesConf
        
        return { Hlt2InclusivePhiLinesConf   : {
            'KaonPT'              : 800      # MeV
            ,'KaonIP'             : 0.05     # mm  
            ,'PhiMassWin'         : 12       # MeV
            ,'PhiPT'              : 1800     # MeV
            ,'PhiDOCA'            : 0.2      # mm
            ,'TFKaonPT'           : 800        # MeV
            ,'TFKaonIPS'          : 6        # dimensionless
            ,'TFPhiMassWin'       : 12       # MeV
            ,'TFPhiPT'            : 1800     # MeV
            ,'TFPhiVCHI2'         : 20       # dimensionless
            ,'TFKaonRichPID'      : 0        # dimensionless
            ,'PhiMassWinSB'       : 30       # MeV            
            ,'TFPhiMassWinSB'     : 30       # MeV
            ,'Prescale'           : {'Hlt2IncPhiSidebands'   : 1.0
                                     ,'Hlt2IncPhi'           : 1.0
                                     ,'Hlt2IncPhiRobust'     : 1.0
                                     ,'Hlt2IncPhiTrackFit'   : 1.0
                                     }
            ,'Postscale'          : {'Hlt2IncPhiSidebands'  : 'RATE(10)'
                                     ,'Hlt2IncPhi'          : 1.0
                                     ,'Hlt2IncPhiRobust'    : 'RATE(1)' # no real cuts applied so conservatively take a 50th
                                     ,'Hlt2IncPhiTrackFit'  : 'RATE(1)'
                                     }
            } }
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        
        return __phiLines__
