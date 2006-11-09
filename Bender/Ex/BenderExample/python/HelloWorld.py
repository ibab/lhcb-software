#!/usr/bin/env python2.4
# =============================================================================
# $Id: HelloWorld.py,v 1.1 2006-11-09 14:16:00 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
# =============================================================================
""" The most trivial Bender-based : HelloWorld example """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================
## @file
#  The most trivial Bender-based : HelloWorld example """
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================

## import everything form bender 
from bendermodule import * 

## the most primitive algorithm 
class HelloWorld(Algo) :
    """ The most primirive algorithm """
    def analyse( self ) :
        print 'PYTHON: Hello world!'
        self.Print('Hello, World!')
        self.setFilterPassed( True ) 
        return SUCCESS
    

## configure the job
def configure ( **args ) :
    """ Configure the job """

    ## read external configruation files
    gaudi.config ( files = [
        '$DAVINCIROOT/options/DaVinciCommon.opts',
        '$DAVINCIROOT/options/DaVinciTestData.opts'
        ] )
    
    ## create local algorithm:
    hello = HelloWorld('Hello')

    ## set it as main algorithm toebexecuted 
    gaudi.setAlgorithms ( hello )
    
    return SUCCESS 
    
## report about what is imported
if __name__ == '__main__' :

    ## configure the job:
    configure()

    ## run the job
    gaudi.run(50)
    

# =============================================================================
# $Log: not supported by cvs2svn $ 
# =============================================================================
# The END 
# =============================================================================
