#
# High cuts (for hadronic)
#

# the list of lines is the same 

__phiLines__ = [ 'Hlt2IncPhi',
                 'Hlt2IncPhiSidebands' 
                ]
#
# Low cuts (for charming and leptonic)
#

class Phi_draft2012 :
    """
    Draft settings for 2011 running    

    (see presentation on Trigger & Physics meeting Jan 17th 2011)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author V. Gligorov, A. Puig 
    @date 2011-01-19
    """
    
    __all__ = ( 'ActiveHlt2Lines', 'Thresholds' )
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        from Hlt2Lines.Hlt2InclusivePhiLines import Hlt2InclusivePhiLinesConf
        
        return { Hlt2InclusivePhiLinesConf   : {
             'KaonPT'             : 800      # MeV
            ,'PhiMassWin'         : 20       # MeV
            ,'PhiPT'              : 1800     # MeV
            ,'PhiDOCA'            : 0.2      # mm
            ,'KaonIPS'            : 6        # dimensionless
            ,'PhiVCHI2'           : 20       # dimensionless
            ,'KaonRichPID'        : 0        # dimensionless
            ,'PhiMassWinSB'       : 30       # MeV            
            ,'Prescale'           : {'Hlt2IncPhiSidebands'   : 1.0
                                     ,'Hlt2IncPhi'           : 1.0
                                     }
            ,'Postscale'          : {'Hlt2IncPhiSidebands'  : 0.05
                                     ,'Hlt2IncPhi'          : 1.0
                                     }
            } }
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        
        return __phiLines__
