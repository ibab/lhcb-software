from   Bender.Awesome      import *
from   LoKiCore.math       import * 
from   ROOT                import Double
from   PartProp.Nodes      import * 
from   LoKiPhys.decorators import *
# =============================================================================
import LHCbMath.Types
from   GaudiKernel.PhysicalConstants import c_light
# =============================================================================
import GaudiPython
GaudiPython.loaddict( 'ReaderFunctionsDict' )
Random = GaudiPython.Bindings.gbl.Random

## @class SingleCandidateAlgo
#  Algorithm to read a DST, select 1 candidate and save it
#  @author Roel Aaij roel.aaij@cern.ch
#  @date 2010-07-15
class SingleCandidateAlgo( Algo ):
    """
    Algorithm to select and save a single candidate per event
    """
    ## initialize the algorithm 
    def initialize ( self ) :
        """
        Initialize the algorithm 
        """

        ## initialize the base 
        sc = Algo.initialize ( self ) ## initialize the base
        if sc.isFailure() : return sc

        ## Get the DistanceCalculator
        self._distanceCalculator = self.tool( cpp.IDistanceCalculator, 'LoKi::DistanceCalculator' )
        self._ipTool = LoKi.Vertices.ImpactParamTool( self._distanceCalculator )
        
        ## Create the TisTosTool
        self._tisTosTool = self.tool( cpp.ITriggerTisTos, 'TriggerTisTos' )
        self._tisTosTool.setOfflineInput()

        ## Get the cuts and parameter values
        self._cut = self.Configuration.cut()
        self._selection = self.Configuration.selection()

        self._initialRandom = Random.mixString( "Bs2JpsiPhiSelector" )

        return SUCCESS
    
    ## The standard method for analysis
    def analyse ( self ) :
        """
        The standard method for analysis
        
        """
        odin = self.get( self.RootInTES + 'DAQ/ODIN' )
        
        ## Check for PVs
        PVs = self.get( self.RootInTES + self.InputPrimaryVertices )
        if not PVs or PVs.size() == 0:
            self.setFilterPassed( False )
            return SUCCESS

        ## get recontructed B+ mesons
        Bs = self.select ( 'B' , eval( self._cut % self._selection ) )
                            
        if not Bs or Bs.size() == 0:
            self.setFilterPassed( False )
            return SUCCESS 

        ## Select random candidate
        r = self.random( odin )
        n = Bs.size()
        for i in xrange( n ):
            if r <= ( float( i ) / float( n ) ): break
        B = Bs[ i ]
        
        tisTos = self.tisTosSignal( B, "Hlt1Track(AllL0|Muon)Decision" )
        if tisTos.tos():
            ## This has to be a clone, otherwise it doesn't work...
            self.markParticle( B.clone() )
            self.setFilterPassed( True )
        else:
            self.setFilterPassed( False )

        return SUCCESS

    # convenience function
    def tisTosSignal( self, signal, line ):
        self._tisTosTool.setOfflineInput( signal )
        return self._tisTosTool.triggerTisTos( line )

    def finalize( self ):
        del self._ipTool

    def random( self, odin ):
        x = self._initial;
        x = Random.mix64( x, odin.gpsTime() )
        x = Random.mix32( x, odin.runNumber() )
        x = Random.mix64( x, odin.eventNumber() )
        return float( x ) / float( 0xFFFFFFFF )
