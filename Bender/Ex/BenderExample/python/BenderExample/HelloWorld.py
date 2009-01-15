#!/usr/bin/env python
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
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# =============================================================================
"""
The most trivial Bender-based  'Hello,World' example
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $ "
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
def configure ( **args ) :
    """
    Configure the job
    """
    
    from Configurables import DaVinci
    
    daVinci = DaVinci (
        DataType   = 'DC06'      , # default  
        Simulation = True        ,
        HltType    = 'Hlt1+Hlt2' ) 
    
    ## get or create the Application Manager
    gaudi = appMgr()
    
    ## create local algorithm:
    hello = HelloWorld ('Hello')
    
    ## set it as main algorithm to be executed 
    gaudi.setAlgorithms ( hello )
    
    ## get input data 
    import LoKiExample.Bs2Jpsiphi_mm_data as input 
    evtSel = gaudi.evtSel()    
    evtSel.open ( input.Files ) 
    evtSel.PrintFreq = 1    
    
    return SUCCESS                                          # RETURN 

# =============================================================================
## job steering 
if __name__ == '__main__' :
    
    ## print own documentation string
    print __doc__
    
    ## configure the job:
    configure ()
    
    ## run the job
    run(50)
    

# =============================================================================
# The END 
# =============================================================================
