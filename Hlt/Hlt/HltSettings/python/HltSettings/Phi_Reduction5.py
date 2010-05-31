#
# High cuts (for hadronic)
#

# the list of lines is the same 

__phiLines__ = [ 'Hlt2IncPhi',
                 'Hlt2IncPhiRobust',
                 'Hlt2IncPhiTrackFit',
                 'Hlt2IncPhiSidebands' ]
#
# Low cuts (for charming and leptonic)
#

class Phi_HadronReduction5 :
    """
    Threshold settings for an Hlt2 reduction factor of 5
    and running in a hadronic scenario -> 10% of Hlt2 bandwidth
    Meaning a total reduction factor for the Incl. Phi of 50
    
    (see presentation on Trigger & Physics meeting November 2009)
    
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
             'KaonPT'             : 400      # MeV
            ,'KaonIP'             : 0.07     # mm  same as in GoodKaons
            ,'PhiMassWin'         : 25       # MeV
            ,'PhiPT'              : 800      # MeV
            ,'PhiDOCA'            : 1        # mm
            ,'TFKaonPT'           : 0        # MeV
            ,'TFKaonIPS'          : 4        # dimensionless
            ,'TFPhiMassWin'       : 20       # MeV
            ,'TFPhiPT'            : 900      # MeV
            ,'TFPhiVCHI2'         : 25       # dimensionless
            ,'TFKaonRichPID'      : -5        # dimensionless
            ,'PhiMassWinSB'       : 30       # MeV            
            ,'TFPhiMassWinSB'     : 30       # MeV
            ,'Prescale'           : {'Hlt2IncPhiSidebands'   : 1.0
                                     ,'Hlt2IncPhi'           : 1.0
                                     ,'Hlt2IncPhiRobust'     : 1.0
                                     ,'Hlt2IncPhiTrackFit'   : 1.0
                                     }
            ,'Postscale'          : {'Hlt2IncPhiSidebands'  : 'RATE(20)'
                                     ,'Hlt2IncPhi'          : 1.0
                                     ,'Hlt2IncPhiRobust'    : 'RATE(1)'
                                     ,'Hlt2IncPhiTrackFit'  : 'RATE(1)'
                                     }
            } }
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        
        return __phiLines__
#
# Low cuts (for charming and leptonic)
#
class PhiCharmLepton_Reduction5 :
    """
    Threshold settings for an Hlt2 reduction factor of 5
    and running in a lepton or charm scenario -> 5% of Hlt2 bandwidth
    Meaning a total reduction factor for the Incl. Phi of 100

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
             'KaonPT'             : 400      # MeV
            ,'KaonIP'             : 0.07     # mm  
            ,'PhiMassWin'         : 25       # MeV
            ,'PhiPT'              : 900      # MeV
            ,'PhiDOCA'            : 1        # mm
            ,'TFKaonPT'           : 0        # MeV
            ,'TFKaonIPS'          : 6.25     # dimensionless
            ,'TFPhiMassWin'       : 20       # MeV
            ,'TFPhiPT'            : 1000      # MeV
            ,'TFPhiVCHI2'         : 25       # dimensionless
            ,'TFKaonRichPID'      : -5        # dimensionless
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
    
    
    
