# $Id: HelloWorld.py,v 1.1 2004-10-27 14:13:47 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================

# =============================================================================
# @file
#
# "Solution"-file for 'Hello,World!' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================

# import everything from BENDER
from bendermodule import *

# get the CONFIGURATION utilities
import benderconfig as bender

# =============================================================================
# @class HelloWorld
# simple Python/Bender class for classical 'Hello,World!' example
# =============================================================================
class HelloWorld(Algo):
    def analyse( self ) :
        print 'Hello, world!'
        return SUCCESS
# =============================================================================


# =============================================================================
# The configuration of the job
# =============================================================================
def configure() :

    bender.config( files = ['$BENDERTUTOROPTS/BenderTutor.opts' ] )

    # modify/update the configuration:
    
    # (1) create the algorithm
    alg = HelloWorld( 'Hello' )
    
    # (2) replace the list of top level algorithm by only *THIS* algorithm
    g.TopAlg = [ 'Hello' ]

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
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2004/10/13 17:45:21  ibelyaev
# New package: Star-Up Tutorial for Bender
# 
# =============================================================================
# The END
# =============================================================================


