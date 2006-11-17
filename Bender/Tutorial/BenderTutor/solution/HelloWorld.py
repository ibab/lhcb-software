#!/usr/bin/env python2.4
# =============================================================================
# $Id: HelloWorld.py,v 1.7 2006-11-17 11:59:47 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.7 $
# =============================================================================
""" Standard 'Hello, world!' example. No way to avoid it! """
# =============================================================================
# @file
#
# "Solution"-file for 'Hello,World!' example (Bender Tutorial)
#
# @author Vanya BELYAEV ibelyaev@physics.syr.edu
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

## import everything from BENDER
from bendermodule import *

# =============================================================================
## @class HelloWorld
#  simple Python/Bender class for classical 'Hello,World!' example
class HelloWorld(Algo):
    """ the most trivial algorithm """

    ## the main 'analysis' method 
    def analyse( self ) :
        """ the main 'analysis' method """
        
        ## use Python printout:
        print 'Hello, world!'
        
        ## use Gaudi printout:
        self.Print( 'Hello, World! (using Gaudi)')
        
        return SUCCESS
# =============================================================================

# =============================================================================
## The configuration of the job
def configure() :
    
    gaudi.config( files = ['$DAVINCIROOT/options/DaVinciCommon.opts' ] )
    
    # modify/update the configuration:
    
    # (1) create the algorithm
    alg = HelloWorld( 'Hello' )
    
    # (2) replace the list of top level algorithm by
    #     new list, which contains only *THIS* algorithm
    gaudi.setAlgorithms( [ alg ] )
    
    ## redefine input files 
    evtSel = gaudi.evtSel()
    import data_tutorial as data 
    evtSel.open( data.FILES ) 
    
    return SUCCESS 
# =============================================================================

# =============================================================================
## Job steering 
if __name__ == '__main__' :

    ## job configuration
    configure()

    ## event loop 
    gaudi.run(50)
        
# =============================================================================
# $Log: not supported by cvs2svn $
#
# =============================================================================
# The END
# =============================================================================


