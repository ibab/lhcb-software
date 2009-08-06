#
# High cuts (for hadronic)
#

# the list of lines is the same 

__phiLines__ = [ 'Hlt2IncPhi',
                 'Hlt2IncPhiRobust',
                 'Hlt2IncPhiTF',
                 'Hlt2IncPhiSidebands' ]

class PhiHigh_Nominal :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (Phi subset at ~200 Hz)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven, K. Vervink
    @date 2009-08-05
    """
    
    __all__ = ( 'ActiveLines', 'Thresholds' )
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        from Hlt2Lines.Hlt2InclusivePhiLines import Hlt2InclusivePhiLinesConf
    
        return { Hlt2InclusivePhiLinesConf : { } }
    
    def ActiveLines(self) :
        """
        Returns a list of active lines
        """
        
        return __phiLines__
#
# Low cuts (for charming and leptonic)
#
class PhiLow_Nominal :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (Phi subset at ~100 Hz)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven, K. Vervink
    @date 2009-08-05
    """
    
    __all__ = ( 'ActiveLines', 'Thresholds' )
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        from Hlt2Lines.Hlt2InclusivePhiLines import Hlt2InclusivePhiLinesConf
    
        return { Hlt2InclusivePhiLinesConf   : { 'TFPhiPT'       : 2000
                                                 , 'TFPhiVCHI2'    : 9
                                                 , 'TFKaonRichPID' : 2
                                                 , 'TFKaonIPS'     : 9
                                                 } }
    
    def ActiveLines(self) :
        """
        Returns a list of active lines
        """
        
        return __phiLines__
    
    
    
