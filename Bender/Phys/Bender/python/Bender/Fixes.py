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
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $' 
# =============================================================================
    
## "at-exit action 
def _bender_at_exit_ () :
    """
    
    At-Exit action
    
    """

    print '*'*80
    print 'Bender.Fixes:  custom "atexit" handler is being invoked'
    print '*'*80
    
    from GaudiPython.Bindings import AppMgr
    g = AppMgr()
    g.exit()

    print '*'*80
    print 'Bender.Fixes:  custom "atexit" handler has been invoked'
    print '*'*80


import atexit

atexit.register( _bender_at_exit_ ) 


# =============================================================================
# The END 
# =============================================================================
