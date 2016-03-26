#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file
#
#  This a template file for the Bender-based script/module
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
#
#  @author ...
#  @date   ...
#
# Last modification $Date$
#                by $Author$
# =============================================================================
""" This a template file for the Bender-based module

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
__author__  = " Do not forget your name here "
__date__    = " 20??-??-?? " 
__version__ = " $Revision$ "
# =============================================================================
## import all nesessary stuff from Bender
from Bender.Main import * 
# =============================================================================
## @class Template
class Template(Algo) :
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
#  @attention the function with such signature is required
#             by Ganga for submission of Grid jobs!
def configure ( datafiles        ,
                catalogs = []    ,
                castor   = False ,
                params   = {}    ) :
    """
    Configure the job
    """
    
    ##
    ## 1. Static configuration using "Configurables"
    ##
    
    the_year = '2012'
    from Configurables import DaVinci
    dv = DaVinci (
        DataType = the_year ,
        ) 
    
    alg_name = 'MyAlg'
    davinci.UserAlgorithms += [ alg_name ]
   
    from Configurables import CondDB
    CondDB ( LatestGlobalTagByDataType = the_year )
    
    from Configurables import LHCbApp
    LHCbApp().XMLSummary = 'summary.xml'
    
    from BenderTools.Utils import silence
    silence()
    
    ## define/set the input data 
    setData ( datafiles , catalogs )
    
    
    ##
    ## jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    
    alg = Template(
        alg_name          ## algorithm instance name 
        )
    
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
    ## configure ( ... )
    
    ## run the job
    run(1000)

# =============================================================================
# The END 
# =============================================================================
