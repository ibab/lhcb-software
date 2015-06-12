from   LoKiAlgo.decorators    import *
from   LoKiCore.math          import * 
from   LoKiPhys.decorators    import *
from   LoKiTrigger.decorators import *
# =============================================================================
import LHCbMath.Types
# =============================================================================
from GaudiPython.Bindings import gbl, SUCCESS
## @class VertexMonitor
#  Algorithm to read DST and store variables in an nTuple
#  @author Roel Aaij roel.aaij@cern.ch
#  @date 2010-07-15
class VertexMonitor( Algo ):
    """
    Algorithm to get numbers related to proper time acceptance due to
    trigger lines
    """
    ## initialize the algorithm 
    def initialize ( self ) :
        """
        Initialize the algorithm 
        """

        ## initialize the base 
        sc = Algo.initialize ( self ) ## initialize the base
        if sc.isFailure() : return sc

        self._beamspot = BEAMSPOT( 1 * mm )

        return SUCCESS
    
    ## The standard method for analysis
    def analyse ( self ) :
        """
        The standard method for analysis
        """
        distances = dict( [ ( name, [] ) for name in self.Vertices.keys() ] )
        for name, location in self.Vertices.iteritems():
            vertices = self.get( '/Event/' + location )
            if not vertices: continue
            lst = distances[ name ]
            for vertex in vertices:
                dx = vertex.position().x() - self._beamspot.x()
                dy = vertex.position().y() - self._beamspot.y()
                lst.append( ( dx, dy ) )

        self.OutQueue.put( distances )
        
        self.setFilterPassed ( True )

        return SUCCESS

    def finalize( self ):
        del self._beamspot
        return Algo.finalize( self )
