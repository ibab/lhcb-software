# $Id: MCTrees.py,v 1.1.1.1 2004-10-13 17:45:21 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================

# =============================================================================
# @file
#
# "Solution"-file for 'MCTrees.py' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================

# import everything from BENDER
from bendermodule import *

# get the CONFIGURATION utilities
import benderconfig as bender

# =============================================================================
# The algorthmm itself 
# =============================================================================
class MCTrees( Algo ) :
    def analyse( self ) :

        # get the helper object 
        mc = self.mctruth()
        
        # find all MC trees of interest 
        trees  = mc.find( decay = '             [B_s0 -> ( J/psi(1S) -> mu+  mu- ) phi(1020) ]cc' )
        
        # get all kaons from the tree :
        phis   = mc.find( decay = ' phi(1020) : [B_s0 -> ( J/psi(1S) -> mu+  mu- ) phi(1020) ]cc' )
        
        # get marked particles from the tree:
        mus    = mc.find( decay = '             [B_s0 -> ( J/psi(1S) -> mu+ ^mu- ) phi(1020) ]cc' )


        print ' found MCtrees/Phis/Mus: ' , trees.size() , ' ' , phis.size() , ' ' , mus.size()   

        # fill the histogram 
        for mu in mus :
            self.plot ( title = ' PT of Muons from J/psi ' ,
                        value = MCPT( mu ) / GeV            ,
                        high  = 10                         ) 

        tup = self.nTuple( title = 'My N-Tuple' )
        zOrig = MCVXFUN( MCVZ ) 
        for mu in mus :
            tup.column( name = 'P'   , value = MCP ( mu ) / GeV )
            tup.column( name = 'PT'  , value = MCPT( mu ) / GeV )
            tup.column( name = 'ID'  , value = MCID( mu )       )
            tup.column( name = 'Q3'  , value = MC3Q( mu )       )
            tup.column( name = 'zOR' , value = zOrig( mu ) / mm ) 
            tup.write() 
            
        return SUCCESS
# =============================================================================
    
# =============================================================================
# configure the job
# =============================================================================
def configure() :
    
    bender.config ( files = ['$BENDERTUTOROPTS/BenderTutor.opts' ] )
    
    # modify/update the configuration:
    
    # 1) create the algorithm
    alg = MCTrees( 'McTree' )
    
    # 2) replace the list of top level algorithm by only *THIS* algorithm
    g.TopAlg = [ 'McTree' ]
    
    # configure the histograms:
    g.HistogramPersistency = 'HBOOK' 
    hsvc = g.service('HistogramPersistencySvc')
    hsvc.OutputFile = 'mctreeshistos.hbook'
    
    
    # configure the N-Tuples:
    ntsvc = g.service('NTupleSvc')
    ntsvc.Output = [ "MC DATAFILE='mctreestuples.hbook' TYP='HBOOK' OPT='NEW'" ]
    
    myAlg = g.algorithm('McTree')
    myAlg.NTupleLUN = 'MC'
    
    # redefine input files 
    evtsel = g.evtSel()
    evtsel.open( [ 'LFN:/lhcb/production/DC04/v1/DST/00000543_00000017_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000018_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000016_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000020_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000024_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000019_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000021_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000022_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000001_5.dst',
                   'LFN:/lhcb/production/DC04/v1/DST/00000543_00000002_5.dst' ] )
    
    return SUCCESS
# =============================================================================

# =============================================================================
# The control flow 
# =============================================================================
if __name__ == '__main__' :

    # job configuration
    configure()

    # event loop 
    g.run(50)

    # for the interactive mode it is better to comment the last line
    g.exit()
# =============================================================================

# =============================================================================
# The END 
# =============================================================================
