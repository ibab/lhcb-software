#!/usr/bin/env python 
# =============================================================================
## The helper Python module for Bender application 
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
"""
The helper module for Bender application.
It applies some last-moment (version-dependent) fixes 
"""
# =============================================================================
__author__  = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $' 
# =============================================================================



from LoKiAlgo.decorators    import Algo   as _Algo
from LoKiAlgoMC.decorators  import AlgoMC as _AlgoMC
from GaudiPython.GaudiAlgs  import _start_
from GaudiPython.GaudiAlgs  import _stop_

_Algo   .start = _start_
_AlgoMC .start = _start_
_Algo   .stop  = _stop_
_AlgoMC .stop  = _stop_


print __name__ + ': LoKi.Algo*.start '
print __name__ + ': LoKi.Algo*.stop  '




# =============================================================================
# The END 
# =============================================================================
