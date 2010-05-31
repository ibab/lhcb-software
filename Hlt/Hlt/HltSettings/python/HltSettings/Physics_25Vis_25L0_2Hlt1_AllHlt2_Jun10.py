from  Physics_25Vis_25L0_2Hlt1_ExpressHlt2_Jun10 import  Physics_25Vis_25L0_2Hlt1_ExpressHlt2_Jun10
class Physics_25Vis_25L0_2Hlt1_AllHlt2_Jun10           ( Physics_25Vis_25L0_2Hlt1_ExpressHlt2_Jun10 ):
    """
    Threshold settings for Hlt1 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-26
    """
    
    __all__ = ( 'HltType', 'ActiveHlt2Lines', 'Thresholds' )

    def HltType(self) :
        self.verifyType( Physics_25Vis_25L0_2Hlt1_AllHlt2_Jun10 )
        return          'Physics_25Vis_25L0_2Hlt1_AllHlt2_Jun10'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        thresholds = Physics_25Vis_25L0_2Hlt1_ExpressHlt2_Jun10.Thresholds(self)
        # Hlt2
        from DefaultHlt2Lines import DefaultHlt2Lines         
        thresholds.update( DefaultHlt2Lines().Thresholds() )

        return thresholds
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        list = Physics_25Vis_25L0_2Hlt1_ExpressHlt2_Jun10.ActiveHlt2Lines(self)
        from DefaultHlt2Lines import DefaultHlt2Lines 
        list.extend( DefaultHlt2Lines().ActiveHlt2Lines() )
        return list
