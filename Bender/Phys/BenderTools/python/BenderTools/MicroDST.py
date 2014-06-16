#!/usr/bin/env python
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
"""Helper module to deal with uDST, mainly TisTos'sing for uDst 

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    
                                                                 
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
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.MicroDST' )
else                      : logger = getLogger ( __name__ )
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

    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) ) 
    logger.info ( 80*'*'  ) 

# =============================================================================
# The END 
# =============================================================================
