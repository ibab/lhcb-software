#!/usr/bin/env python
# =============================================================================
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
#                   $Revision: 157296 $
# Last modification $Date: 2013-05-26 12:47:48 +0200 (Sun, 26 May 2013) $
#                by $Author: ibelyaev $
# =============================================================================
"""

Few pure technical utilities 

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

"""
# =============================================================================
__version__ = "$Revision: 157296 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2013-06-11"
# =============================================================================
__all__= (
    'silence'   ## suppress some unnesessary printout from Gaudi 
    )
# ==============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
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
    Suppress some unnesessary printout form Gaudi
    
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
            
# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'

# ==============================================================================
# The END 
# ==============================================================================
