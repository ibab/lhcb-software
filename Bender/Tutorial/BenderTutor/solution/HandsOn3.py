# $Id: HandsOn3.py,v 1.1 2004-10-27 14:13:47 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================

# =============================================================================
# @file
#
# "Solution"-file for 'HandsOn3.py' example (Bender Tutorial)
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
class MCKaons( Algo ) :
    def analyse( self ) :

        # get the helper object 
        mc = self.mctruth()
        
        # get all kaons from the tree :
        kaons  = mc.find( decay = ' [B_s0 -> J/psi(1S) ( phi(1020) -> ^K+ ^K- ) ]cc' )
        
        print ' foud Kaons: ' , kaons.size()    

        tup = self.nTuple( title = 'My N-Tuple' )

        for K in kaons :
            
            print ' Kaon: ', nameFromPID( K.particleID() ) , K.momentum().perp() / GeV
            
            self.plot ( title = ' PT of Kaons from psi '   ,
                        value = MCPT( K ) / GeV            ,
                        high  = 5                          ) 

            tup.column( name = 'P'  , value = MCP ( K ) / GeV )
            tup.column( name = 'PT' , value = MCPT( K ) / GeV )
            tup.column( name = 'ID' , value = MCID( K )       )
            tup.column( name = 'Q3' , value = MC3Q( K )       )
            # put all components of 4-vector 
            tup.column( name = 'PK'  , value = K.momentum() )
            # put all components of 3-vector 
            tup.column( name = 'VK'  , value = K.originVertex().position() )
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
    alg = MCKaons( 'MCKaons' )
    
    # 2) replace the list of top level algorithm by only *THIS* algorithm
    g.TopAlg = [ 'MCKaons' ]
    
    # configure the histograms:
    hsvc = g.service('HistogramPersistencySvc')
    hsvc.OutputFile = 'mckaonhistos.hbook'
    g.HistogramPersistency = 'HBOOK' 
    
    # configure the N-Tuples:
    ntsvc = g.service('NTupleSvc')
    ntsvc.Output = [ "MC DATAFILE='mckaontuples.hbook' TYP='HBOOK' OPT='NEW'" ]
    
    myAlg = g.algorithm('MCKaons')
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
    g.run(500)

    # for the interactive mode it is better to comment the last line
    g.exit()
# =============================================================================

# =============================================================================
# The END 
# =============================================================================
