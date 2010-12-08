class Hadrons_draft2011 :
    """
    Threshold settings for Hlt2 hadronic lines: minimal draft for 2011
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author V. Gligorov 
    @date 2010-12-02
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            # The topological 
            'Hlt2TopoOSTF2Body',
            'Hlt2TopoOSTF3Body',
            'Hlt2TopoOSTF4Body'
            ]

        return lines

    def Thresholds(self) : 
       
        d = { }
 
        from TopoLines_September10 import TopoLines
        d.update ( TopoLines().Thresholds() )

        return d
