#!/usr/bin/env python 
# =============================================================================
## @file Benedr/Fixes_LoKi.py
#  The helper Python module for Bender application 
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
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""Apply some last-moment (version-dependent) fixes

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
__author__  = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
__date__    = "2004-07-11"
__version__ = '$Revision$'
__all__     = ()
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'Bender.Fixes_LoKi' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
logger.info ( '*** Fix some LHCb/LoKi features' ) 
# =============================================================================

# =============================================================================
## decorate the ranges
# =============================================================================
from   LoKiCore.basic      import cpp , std, Gaudi, LHCb, cpp 
import LoKiCore.decorators as     LCD
            
# 
## decorate some "packed" objects:
for i in [
    ## RC 
    ( LHCb.PackedCaloHypos        , 'hypos'    ) ,
    ( LHCb.PackedRecVertices      , 'vertices' ) ,
    ( LHCb.PackedTwoProngVertices , 'vertices' ) ,
    ( LHCb.PackedParticles        , 'data'     ) ,
    ( LHCb.PackedVertices         , 'data'     ) ,
    ( LHCb.PackedMuonPIDs         , 'data'     ) ,
    ( LHCb.PackedRichPIDs         , 'data'     ) ,
    ( LHCb.PackedProtoParticles   , 'protos'   ) ,
    ( LHCb.PackedTracks           , 'tracks'   ) ,
    ## MC
    ( LHCb.PackedMCCaloHits       , 'data'     ) ,
    ( LHCb.PackedMCHits           , 'data'     )
    ] :
    
    t = i[0]
    m = i[1]
    
    if not hasattr ( t , 'size' ) :
        logger.debug ( 'decorate packed type: %s ' % t  ) 
        t.get_data    = getattr ( t , m ) 
        t.__len__     = lambda s : s.get_data().size()
        t.size        = lambda s : len  ( s )

        
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
