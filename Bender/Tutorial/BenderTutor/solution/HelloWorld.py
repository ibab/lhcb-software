#!/usr/bin/env python
# =============================================================================
# $Id: HelloWorld.py,v 1.15 2010-09-13 13:24:04 ibelyaev Exp $ 
# =============================================================================
# $URL$
# =============================================================================
## @file solution/Helloworld.py
#
#  The standard 'Hello,world!'-example
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-12
#
#  Last modification $Date: 2010-09-13 13:24:04 $
#                 by $Author: ibelyaev $
# =============================================================================
"""

The standard 'Hello,world!'-example

This file is a part of BENDER project:
   ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date: 2010-09-13 13:24:04 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2006-10-12 " 
__version__ = " Version $Revision: 1.15 $ "
# =============================================================================
## import everything from bender 
from Bender.Main import *
# =============================================================================
## @class HelloWorld
#  simple Python/Bender class for classical 'Hello,World!' example
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
class HelloWorld(Algo):
    """
    The most trivial algorithm to print 'Hello,world!'
    """

    ## the main 'analysis' method 
    def analyse( self ) :
        """
        The main 'analysis' method
        """
        
        ## use the native Python print to stdout:
        print 'Hello, world!'
        
        ## use Gaudi-style printout:
        self.Print( 'Hello, World! (using Gaudi)')
        
        return SUCCESS
# =============================================================================

# =============================================================================
## The configuration of the job
def configure ( inputdata , catalogs = [] , castor = False ) :
    
    
    from Configurables import DaVinci
    
    DaVinci ( DataType   = '2011' ) 
    
    ## get/create application manager
    gaudi = appMgr() 

    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    # modify/update the configuration:
    
    # (1) create the algorithm
    alg = HelloWorld( 'Hello' )
    
    # (2) replace the list of top level algorithm by
    #     new list, which contains only *THIS* algorithm
    gaudi.setAlgorithms( [ alg ] )
    
    return SUCCESS 
# =============================================================================

# =============================================================================
## Job steering 
if __name__ == '__main__' :

    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
        
    ## job configuration
    inputdata = [
        '/lhcb/LHCb/Collision11/MINIBIAS.DST/00011900/0000/00011900_000000%02d_1.minibias.dst' % i for i in range(5,25)
        ]
    
    configure( inputdata , castor = True )
    
    ## event loop 
    run(50)
        
# =============================================================================
# The END
# =============================================================================


