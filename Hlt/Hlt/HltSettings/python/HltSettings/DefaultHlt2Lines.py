class DefaultHlt2Lines :
    """
    Default Hlt2 lines 
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg
    @date 2010-05-07
    """
    
    __all__ = ( 'ActiveHlt2Lines', 'Thresholds' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        list = []
##
## Topo : whole list
##
        from TopoLines import TopoLines
        list.extend( TopoLines().ActiveHlt2Lines() )
##
## Charm : whole list
##
        from CharmLines import CharmLines
        list.extend( CharmLines().ActiveHlt2Lines() )
##
## DiMuon : whole list
##
        from DiMuonLines import DiMuonLines
        list.extend( DiMuonLines().ActiveHlt2Lines() )
##
## phi : whole list
##
        from Phi_Reduction5 import Phi_HadronReduction5
        list.extend( Phi_HadronReduction5().ActiveHlt2Lines() )
##
## B 2DX
##
        from B2DX_320Vis import B2DX_320Vis
        list.extend( B2DX_320Vis().ActiveHlt2Lines() )
        
##
## exclusive : whole list
##
        
        from Exclusive_Nominal import Exclusive_Nominal
        list.extend( Exclusive_Nominal().ActiveHlt2Lines() )
               
        return list

        
    def Thresholds(self) :
        """
        Hlt2 thresholds when available
        """
        from CharmLines import CharmLines
        thresholds = CharmLines().Thresholds()
        
        from TopoLines import TopoLines
        thresholds.update( TopoLines().Thresholds())

        from DiMuonLines import DiMuonLines
        thresholds.update( DiMuonLines().Thresholds())

        from Phi_Reduction5 import Phi_HadronReduction5
        thresholds.update(Phi_HadronReduction5().Thresholds())
 
        return thresholds
