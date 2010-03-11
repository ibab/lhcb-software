from Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10 import Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10
class Physics_320Vis_300L0_10Hlt1_PassThroughHlt2_Feb10( Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10 ) :
    """
    Threshold settings for Hlt1 and 2 for 2010 conditions
    Hlt2 : run (almost) all with default cuts
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    See https://savannah.cern.ch/task/index.php?14409
    
    @author P.Koppenburg
    @date 2010-03-11
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds','L0TCK' )

    def HltType(self) :
        self.verifyType( Physics_320Vis_300L0_10Hlt1_PassThroughHlt2_Feb10 )
        return          'Physics_320Vis_300L0_10Hlt1_PassThroughHlt2_Feb10'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        d = Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10.Thresholds(self)        
        from Hlt2Lines.Hlt2CommissioningLines import Hlt2CommissioningLinesConf
        d.update( {Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 1 # changed back
                                                                 }              } } )
        return d
            
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        list =  Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10.ActiveHlt2Lines(self)
        return list
        
