#!/usr/bin/env python 
# =============================================================================
# $Id: Utils.py 133294 2011-12-18 14:37:25Z ibelyaev $ 
# =============================================================================
## @file AnalysisPython/Logger.py
#
#  copied from Bender
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
#  @date   2010-09-12
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#
#                    $Revision: 133294 $
#  Last modification $Date: 2011-12-18 15:37:25 +0100 (Sun, 18 Dec 2011) $
#                 by $Author: ibelyaev $ 
# =============================================================================
"""
Logger for Bender 

This file is a part of BENDER project:
   ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
   ``C++ ToolKit for Smart and Friendly Physics Analysis''
   
By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
   ``No Vanya's lines are allowed in LHCb/Gaudi software.''

                    $Revision: 133294 $
  Last modification $Date: 2011-12-18 15:37:25 +0100 (Sun, 18 Dec 2011) $
                 by $Author: ibelyaev $  
                 """
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2012-03-16"
__version__ = '$Revision: 133294 $'
__all__     = (
    'getLogger' , 
    )
# =============================================================================
import logging

# =============================================================================
## get logger 
def getLogger ( name                                               ,
                fmt  = '# %(name)-25s %(levelname)-7s %(message)s' ,
                lvl  = logging.INFO                                ) :  
    """
    Get the proper logger
    
    >>> logger = getLogger ( __name__ )
    
    """
    #
    # while 0 <= name.find ( '.' )  : name = name.replace('.','/')
    # 
    logger = logging.getLogger ( name )
    logger.propagate =  False 
    #
    while logger.handlers :
        logger.removeHandler ( logger.handlers[0] )
    #
    lh  = logging.StreamHandler ( )
    fmt = logging.Formatter ( fmt  )
    lh  . setFormatter  ( fmt )
    logger.addHandler( lh ) 
    #
    logger.setLevel  ( lvl )
    #
    return logger

# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
    
# =============================================================================
# The END 
# =============================================================================
