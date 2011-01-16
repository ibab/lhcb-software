#!/usr/bin/env python
# =============================================================================
# $URL$
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
#  with the campain of Dr.O.Callot et al.: 
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
with the campain of Dr.O.Callot et al.: 
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
def configure ( datafiles , catalogs = [] ) :
    """
    Configure the job
    """    
    ##
    ## Static Configuration (``Configurables'')
    ##
        
    from Configurables import DaVinci    
    daVinci = DaVinci (
        DataType   = 'DC06' , # default  
        Simulation = True   ) 
    
    ## define the input data 
    setData ( datafiles , catalogs ) 
    
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
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120  
    
    ## configure the job:
    inputdata = [
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000311_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005102/0000/00005102_00000322_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'"
        ]

    configure ( inputdata ) 
    ## run the job
    run(50)
    
    
# =============================================================================
# The END 
# =============================================================================
