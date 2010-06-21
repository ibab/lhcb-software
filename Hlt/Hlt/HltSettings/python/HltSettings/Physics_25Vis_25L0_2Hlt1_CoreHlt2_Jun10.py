from  Physics_25Vis_25L0_2Hlt1_ExpressHlt2_Jun10 import  Physics_25Vis_25L0_2Hlt1_ExpressHlt2_Jun10
class Physics_25Vis_25L0_2Hlt1_CoreHlt2_Jun10           ( Physics_25Vis_25L0_2Hlt1_ExpressHlt2_Jun10 ):
    """
    Threshold settings for Hlt1 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-26
    """
    
    __all__ = ( 'HltType', 'ActiveHlt2Lines', 'Thresholds' )

    def HltType(self) :
        self.verifyType( Physics_25Vis_25L0_2Hlt1_CoreHlt2_Jun10 )
        return          'Physics_25Vis_25L0_2Hlt1_CoreHlt2_Jun10'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        thresholds = Physics_25Vis_25L0_2Hlt1_ExpressHlt2_Jun10.Thresholds(self)
        # Hlt2
        from Hadrons_Minimal import Hadrons_Minimal         
        thresholds.update( Hadrons_Minimal().Thresholds() )
        from Muons_Minimal import Muons_Minimal        
        thresholds.update( Muons_Minimal().Thresholds() )

        return thresholds
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        list = Physics_25Vis_25L0_2Hlt1_ExpressHlt2_Jun10.ActiveHlt2Lines(self)
        from Hadrons_Minimal import Hadrons_Minimal 
        list.extend( Hadrons_Minimal().ActiveHlt2Lines() )
        from Muons_Minimal import Muons_Minimal 
        list.extend( Muons_Minimal().ActiveHlt2Lines() )
        return list
