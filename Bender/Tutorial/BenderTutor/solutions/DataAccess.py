# $Id: DataAccess.py,v 1.3 2004-10-27 14:08:19 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================

# =============================================================================
# @file
#
# "Solution"-file for 'DataAccess/GetData' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================

# import everything from BENDER
from bendermodule import *

# get the CONFIGURATION utilities
import benderconfig as bender

# =============================================================================
# @class GetData
# define the primitive algorithm 
# =============================================================================
class GetData(Algo):
    def analyse( self ) :

        # get and print the event number 
        hdr = self.get( address = 'Header' )
        print ' Event# ', hdr.evtNum()
        
        # get all MC particles as 'native' KeyedContainer
        mcps1 = self.get( address = 'MC/Particles' )
        print ' The type of "mcps1" is ' , type(mcps1)

        # get all MC particles as std::vector ( useful for explicit loops)
        mcps2 = self.get( address = 'MC/Particles' ,
                          vector  = TRUE           )
        print ' The type of "mcps2" is ' , type(mcps2)
        
        # get all MC particles as python's list  ( useful for explicit loops)
        mcps3 = self.get( address = 'MC/Particles' ,
                          list    = TRUE           )
        print ' The type of "mcps3" is ' , type(mcps3)

        # for the first 5 particles print PID information
        num = 0 
        for mcp in mcps2 :
            num += 1 
            if num > 5 : continue 
            print ' MCParticle name is ', nameFromPID( mcp.particleID() ) 

        
        return SUCCESS
# =============================================================================

# =============================================================================
# The configuration fo the job
# =============================================================================
def configure() :
    
    bender.config ( files = ['$BENDERTUTOROPTS/BenderTutor.opts' ] )
    
    # modify/update the configuration:
    
    # 1) create the algorithm
    alg = GetData( 'GetData' )
    
    # 2) replace the list of top level algorithm by only *THIS* algorithm
    g.TopAlg = [ 'GetData' ]

    return SUCCESS 
# =============================================================================
    
# =============================================================================
# Job steering 
# =============================================================================
if __name__ == '__main__' :
    
    # job configuration
    configure()
    
    # event loop 
    g.run(50)
    
    # for the interactive mode it is better to comment the last line
    g.exit()
    
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2004/10/13 17:45:21  ibelyaev
# New package: Star-Up Tutorial for Bender
# 
# =============================================================================
# The END
# =============================================================================
