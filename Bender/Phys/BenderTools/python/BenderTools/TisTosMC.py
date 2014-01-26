#!/usr/bin/env python
# =============================================================================
# -*- coding: utf-8 -*-
# =============================================================================
# @file BenderTools/TisTosMC.py
#
# Decorate the algorithm with helper methods for TisTosTob'ing 
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
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
# @author Vanya BELYAEV Ivan.Belyaev@cern.ch
# @date   2011-06-21
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""
Decorate the algorithm with helper methods for TisTosTob'ing 

This file is a part of BENDER project:
    ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campain of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2011-06-07"
# =============================================================================
__all__= (
    'decision'     , ## collect trigger decisions
    'trgDecs'      , ## print trigger statistics 
    'tistos'       , ## fill N-tuple with TisTos information
    )
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# ==============================================================================
from   LoKiCore.basic  import cpp
from   Bender.MainMC   import SUCCESS, AlgoMC 
# ==============================================================================
ITriggerSelectionTisTos = cpp.ITriggerSelectionTisTos
# ==============================================================================
import BenderTools.TisTos  as TT
decisions = TT.decisions
trgDecs   = TT.trgDecs
tisTos    = TT.tisTos
# ==============================================================================
logger.info ("Add useful methods to class AlgoMC for TisTos'sing ") 
# =============================================================================
AlgoMC.decisions         = decisions
AlgoMC.trgDecs           = trgDecs 
AlgoMC.tisTos            = tisTos 
AlgoMC.tisTos_initialize = TT._tisTosInit
AlgoMC.tisTos_finalize   = TT._tisTosFini
# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
