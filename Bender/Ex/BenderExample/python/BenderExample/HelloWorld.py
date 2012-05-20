#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file BenderExample/HelloWorld.py
#  The most trivial Bender-based : HelloWorld example
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
#  @date 2006-10-12
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The most trivial Bender-based  ``Hello,World'' example

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2006-10-12 "
__version__ = " Version $Revision$ "
# =============================================================================
## import everything from Bender 
from Bender.Main import * 
# =============================================================================
## the most primitive algorithm 
class HelloWorld(Algo) :
    """
    The most primitive Bender-based algorithm
    """
    def analyse( self ) :

        ## use thenative Pthon printout 
        print 'PYTHON: Hello world!'

        ## use the standard Gaudi streams 
        self.Print('Hello, World!')
        
        self.setFilterPassed( True ) 
        return SUCCESS                                     # RETURN 
    
# =============================================================================
## configure the job
def configure ( datafiles , catalogs = [] , castor = False ) :
    """
    Configure the job
    """    
    ##
    ## Static Configuration (``Configurables'')
    ##
        
    from Configurables import DaVinci    
    daVinci = DaVinci (
        DataType    = '2010' ,
        Simulation  = True   ) 
    
    ## define the input data 
    setData ( datafiles , catalogs , castor ) 
    
    ##
    ## Dynamic Configuration: Jump into the wonderful world of GaudiPython 
    ##
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    hello = HelloWorld ('Hello')
    
    ## set it as main algorithm to be executed 
    gaudi.setAlgorithms ( hello )
    
    return SUCCESS                                          # RETURN 

# =============================================================================
## job steering 
if __name__ == '__main__' :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  
    
    ## configure the job:
    inputdata = [
        '/lhcb/MC/MC10/ALLSTREAMS.DST/00008919/0000/00008919_00000%03d_1.allstreams.dst' % i for i in range ( 1 , 90 ) 
        ]
    
    configure( inputdata , castor  = True )  

    ## run the job
    run(50)
    
    
# =============================================================================
# The END 
# =============================================================================
