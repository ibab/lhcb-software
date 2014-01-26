#!/usr/bin/env python
# =============================================================================
# -*- coding: utf-8 -*-
# =============================================================================
# @file BenderTools/MicroDST.py
#
# Helper module to deal with uDST, mainly TisTos'sing for uDst 
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
# @author VanyaBELYAEV Ivan.Belyaev@itep.ru
# @date   2011-10-05
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""

Helper module to deal with uDST, mainly TisTos'sing for uDst 

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-10-05"
__version__ = "$Revision$"
__all__     = (
    'uDstConf'    ,
    'uDstConf13b'    ## version for Stripping13b processing
    )
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
## Configure node killing agents for uDST processing
#  @author VanyaBELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-10-05
def killDAQ ( nodes = [ '/Event/DAQ' , '/Event/pRec' ] , logger_ = None )  :
    """
    Configure node killing agents for uDST processing
    """
    #
    from PhysConf.MicroDST import killDAQ as _killDAQ
    #
    return _killDAQ ( nodes , logger ) 

# ======================================================================================
## configure various uDst settings from RootInTES 
def uDstConf ( rootInTes           ,
               nodes     = [ '/Event/DAQ'  ,
                             '/Event/pRec' ] ,
               logger_   = None )  :
    """
    Various settings for mDst
    
    """
    from PhysConf.MicroDST import uDstConf as _uDstConf 
    #
    return _uDstConf ( rootInTes , nodes , logger ) 

# ======================================================================================
## configure various uDst settings from RootInTES (verision for Stripping13b processing)
#  @attention it is valid only for Stripping13b
def uDstConf13b ( rootInTes , logger_ = None ) :
    """
    Various settings for mDst (version for Stirpping13b processing)
    
    """
    from PhysConf.MicroDST import uDstConf as _uDstConf13b 
    #
    return _uDstConf13b ( rootInTes , nodes , logger ) 

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
