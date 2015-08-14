#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file BenderTools/Utils.py
#
#  Few pure technical utilities 
#  
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  <b>``C++ ToolKit for Smart and Friendly Physics Analysis''</b>
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software''
#
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2013-06-11
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""Few pure technical utilities

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2013-06-11"
# =============================================================================
__all__= (
    'silence'     ,    ## suppress some unnesessary printout from Gaudi
    'totalSilence',    ## further suppress printout 
    )
# ==============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.Utils' )
else                      : logger = getLogger ( __name__ )
# =============================================================================

# ==============================================================================
## suppress unnesessary printout in Gaudi
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2013-06-11
def silence ( lst = [ 'HcalDet.Quality'          ,
                      'EcalDet.Quality'          ,
                      'MagneticFieldSvc'         ,
                      'PropertyConfigSvc'        ,
                      'IntegrateBeamCrossing'    ,
                      'ToolSvc.L0DUConfig'       ,
                      'ToolSvc.L0CondDBProvider' , 
                      'L0MuonFromRaw'            ] ) :
    """    
    Suppress some unnesessary printout from Gaudi
    """
    #
    logger.info ( 'Suppress some unnesessary Gaudi prints' ) 
    if     isinstance ( lst , str  ) : lst =      [ lst ]
    if not isinstance ( lst , list ) : lst = list ( lst ) 
    #
    from GaudiPython.Bindings import _gaudi
    if not _gaudi :
        ## use configurables 
        from Configurables import MessageSvc
        msg = MessageSvc()
        
    else :
        ## use GaudiPython 
        msg = _gaudi.service('MessageSvc')
    #
    msg.setError += lst


# ============================================================================
## further suppress printout (suitbale doe "dst-dump"-like scripts
def totalSilence ( lst = [ 'RootCnvSvc'               ,
                           'IOManagerSvc'             ,
                           'RootHistSvc'              ,
                           'LHCb::RawDataCnvSvc'      ,
                           'HcalDet.Quality'          ,
                           'EcalDet.Quality'          ,
                           'MagneticFieldSvc'         ,
                           'PropertyConfigSvc'        ,
                           'ToolSvc.L0DUConfig'       ,
                           'ToolSvc.L0CondDBProvider' , 
                           'L0MuonFromRaw'            ,
                           'IntegrateBeamCrossing'    ] , dod = True ) :
    
    from Configurables import MessageSvc, DataOnDemandSvc, ToolSvc 
    from Configurables import Gaudi__RootCnvSvc    as RootCnvSvc 
    from Configurables import Gaudi__IODataManager as IODataManager
    from Configurables import LHCb__RawDataCnvSvc  as RawDataCnvSvc 
    
    msg = MessageSvc()
    msg.OutputLevel = 5
    
    ToolSvc           (                                  OutputLevel = 6 )
    RootCnvSvc        ( "RootCnvSvc"                   , OutputLevel = 6 )
    RawDataCnvSvc     (                                  OutputLevel = 6 )
    
    IODataManager     ( 'IODataManager'                , OutputLevel = 6 ,
                        AgeLimit = 1 , UseGFAL = False )
    
    if dod :
        DataOnDemandSvc   ( Dump = True  )
    else :
        DataOnDemandSvc   ( Dump = False , OutputLevel = 6 )
        msg.setFatal += [ 'DataOnDemandSvc' ] 
        
    msg.setFatal += lst

    
# =============================================================================
if '__main__' == __name__ :
    
    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) ) 
    logger.info ( 80*'*'  ) 
    

# ==============================================================================
# The END 
# ==============================================================================
