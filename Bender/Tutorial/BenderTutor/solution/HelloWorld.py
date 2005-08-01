#!/usr/bin/env python
# =============================================================================
# $Id: HelloWorld.py,v 1.5 2005-08-01 16:04:24 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.5 $
# =============================================================================
"""
Standard 'Hello, world!' example. No way to avoid it!
"""
# =============================================================================
# @file
#
# "Solution"-file for 'Hello,World!' example (Bender Tutorial)
#
# @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
# @date   2004-10-12
# =============================================================================
__author__ = 'Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr'
# =============================================================================

# import everything from BENDER
from bendermodule import *

# =============================================================================
# @class HelloWorld
# simple Python/Bender class for classical 'Hello,World!' example
# =============================================================================
class HelloWorld(Algo):
    def analyse( self ) :
        
        # use Python printout:
        print 'Hello, world!'
        
        # use Gaudi printout:
        self.Print( message = 'Hello, World! (using Gaudi)')
        
        return SUCCESS
# =============================================================================

# =============================================================================
# The configuration of the job
# =============================================================================
def configure() :
    
    gaudi.config( files = ['$DAVINCIROOT/options/DaVinciCommon.opts' ] )
    
    # modify/update the configuration:
    
    # (1) create the algorithm
    alg = HelloWorld( 'Hello' )
    
    # (2) replace the list of top level algorithm by
    #     new list, which contains only *THIS* algorithm
    gaudi.setAlgorithms( [ alg ] )
    
    # redefine input files 
    evtSel = gaudi.evtSel()
    # files from $DAVINCIROOT/options/DaVinciTestData.opts 
    evtSel.open( [
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000665_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000645_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000648_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000652_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000656_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000658_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000659_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000667_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000670_9.dst' ,
        'PFN:castor:/castor/cern.ch/lhcb/DC04/00000541_00000672_9.dst' ] ) 
    
          
    return SUCCESS 
# =============================================================================

# =============================================================================
# Job steering 
# =============================================================================
if __name__ == '__main__' :

    # job configuration
    configure()

    # event loop 
    gaudi.run(50)
        
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END
# =============================================================================


