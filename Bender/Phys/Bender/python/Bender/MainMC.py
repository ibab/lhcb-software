#!/usr/bin/env python 
# =============================================================================
## The major Python module for Bender application in MC-mode 
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
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" This is a major Python Module for Bender application in MC-mode """
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

from Bender.Main import *

## MC :
from LoKiGen.decorators           import *
from LoKiGenMC.decorators         import *
from LoKiMC.decorators            import *
from LoKiPhysMC.decorators        import *
from LoKiAlgoMC.decorators        import *

# =============================================================================
# The END 
# =============================================================================
