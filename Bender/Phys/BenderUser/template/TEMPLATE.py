#!/usr/bin/env python
# =============================================================================
# $Id: TEMPLATE.py,v 1.6 2010-09-13 13:27:52 ibelyaev Exp $ 
# =============================================================================
# $URL$ 
# =============================================================================
## @file
#  This a template file for the Bender-based scriopt/module
#
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
#  @author ...
#  @date   ...
#
#  Last modified $Date: 2010-09-13 13:27:52 $
#             by $Author: ibelyaev $
# =============================================================================
"""
This a template file for the Bender-based scriopt/module

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

  Last modified $Date: 2010-09-13 13:27:52 $
             by $Author: ibelyaev $
"""
# =============================================================================
__author__  = "Do not forget your name here"
__date__    = "20??-??-?? " 
__verison__ = "Verison $Revision: 1.6 $"
# =============================================================================
## import all nesessary stuff from Bender
from Bender.MainMC import * 
# =============================================================================
## @class Template
#  @author The Author 
#  @date   ????-??-??
class Tempate(AlgoMC) :
    """
    This is the template algorithm 
    """        
    
   ## standard mehtod for analyses
    def analyse( self ) :
        """
        Standard method for analysis
        """

        return SUCCESS

# =============================================================================
## job configuration:
def configure ( datafiles , catalogs = [] ) :
    """
    Configure the job
    """
    
    ##
    ## 1. Static configuration using "Configurables"
    ##
    
    from Configurables import DaVinci
    DaVinci (
        DataType   = '2010' , 
        Simulation = False  ) 
    

    ## define/set the input data 
    setData ( datafiles , catalogs )
    
    ##
    ## jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:

    ## alg = Template( .... )
    
    ## gaudi.addAlgorithm ( alg ) 
    ## gaudi.setAlgorithms( [alg] )
    
    
    return SUCCESS

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
    # configure ( ... )
    
    ## run the job
    run(100)

# =============================================================================
# The END 
# =============================================================================
