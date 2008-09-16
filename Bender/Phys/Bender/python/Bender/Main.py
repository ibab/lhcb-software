#!/usr/bin/env python 
# =============================================================================
## The major Python module for Bender application 
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
This is a major Python Module for Bender application
"""
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $' 
# =============================================================================

import os 

try:
    ## try to get the startup script from environment 
    startup = os.environ.get('PYTHONSTARTUP',None)
    ## use the default startup script
    if not startup :
        startup = os.sep + 'Bender' + os.sep + 'Startup.py'
        bp = os.environ.get('BENDERPYTHON',None)
        if bp : startup = bp + startup
        else  : startup = 'Startup.py'
    
    if os.path.exists( startup ) :
        execfile( startup )
    else :
        import Bender.Startup
        
except:
    pass



## massive imports of everything 
from LoKiCore.decorators          import *
from LoKiPhys.decorators          import *
from LoKiArrayFunctors.decorators import *
from LoKiTrigger.decorators       import *
from LoKiAlgo.decorators          import *

from LoKiCore.functions           import *

## Get the application manager 
def appMgr( *varg , **kwarg ) :
    """
    Get the application manager 
    """
    import GaudiPython.Bindings
    _g = GaudiPython.Bindings._gaudi
    if not _g : _g = GaudiPython.Bindings.AppMgr( *varg , **kwarg )
    if not 'LoKiSvc' in _g.ExtSvc : _g.ExtSvc += [ 'LoKiSvc']
    return _g

# =============================================================================
## run events 
def run ( n = -1 ) :
    """
    Run gaudi

    >>> run(50)
    
    """
    ## get the application manager
    _g = appMgr() 
    return _g.run ( n )

# =============================================================================
from Gaudi.Configuration import importOptions

# =============================================================================
## apply some last-minutes fixes
import Bender.Fixes


## @var LoKi   : define namespace LoKi 
LoKi   = cpp.LoKi
## @var Bender : define namespace Bender 
Bender = cpp.Bender
## @var LHCb   : define namespace LHCb
LHCb   = cpp.LHCb
## @var Gaudi  : define namespace Gaudi
Gaudi  = cpp.Gaudi

_SC=cpp.StatusCode
SUCCESS = _SC(_SC.SUCCESS,True)
FAILURE = _SC(_SC.FAILURE,True)

_SE = cpp.StatEntity
_iadd_old_ = _SE.__iadd__
def _iadd_new_ (s,v) :
    return _iadd_old_(s,float(v))
_SE.__iadd__ = _iadd_new_
_SE.__str__  = _SE.toString 



# =============================================================================
## Welcome message:
Bender.Welcome.instance()




if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 


# =============================================================================
# The END 
# =============================================================================
