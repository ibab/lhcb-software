from Physics_MinBiasL0_PassThroughHlt_Feb10 import Physics_MinBiasL0_PassThroughHlt_Feb10
class Physics_MinBiasL0_PassThroughHlt12_Feb10( Physics_MinBiasL0_PassThroughHlt_Feb10 ) :
    """
    Threshold settings for Hlt1 and 2 for 2010 conditions
    Hlt2 : run (almost) all with default cuts
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    See https://savannah.cern.ch/task/index.php?14408
    
    @author P.Koppenburg
    @date 2010-03-11
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds','L0TCK' )

    def HltType(self) :
        self.verifyType( Physics_MinBiasL0_PassThroughHlt12_Feb10 )
        return          'Physics_MinBiasL0_PassThroughHlt12_Feb10'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        # keep pass through thresholds
        d = Physics_MinBiasL0_PassThroughHlt_Feb10.Thresholds(self)        
        return d
            
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        list =  Physics_MinBiasL0_PassThroughHlt_Feb10.ActiveHlt2Lines(self)
        #
        # Add Hlt2 as defined in the physics setting
        #
        from Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10 import Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10
        list.extend( Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10().ExplicitHlt2Lines() )
        return list
        
