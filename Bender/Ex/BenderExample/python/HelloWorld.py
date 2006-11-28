#!/usr/bin/env python2.4
# =============================================================================
# $Id: HelloWorld.py,v 1.2 2006-11-28 18:26:45 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
# =============================================================================
## The most trivial Bender-based : HelloWorld example
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The most trivial Bender-based : HelloWorld example """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
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
