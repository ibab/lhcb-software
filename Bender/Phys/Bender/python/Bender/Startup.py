#!/usr/bin/env python
# =============================================================================
# $Id: Startup.py,v 1.3 2008-06-12 12:44:07 ibelyaev Exp $ 
# =============================================================================
## The trivial startup sctript for python Bender session
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
#  @author Vanya BELYAEV ibelyaev@physcis.syr.edu
#  @date   2006-10-08
# =============================================================================
"""
This is a trivial startup script for python Bender session
"""
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@physics.syr.edu'
# =============================================================================

try:

    import os
    ## define the name of the history file
    __history__ = os.path.curdir + os.sep + '.BenderHistory'
    
    def _rename_ ( file , app ) :
        if os.path.exists ( file + app ) :   _rename_ ( file + app ,        app )
        if os.path.exists ( file       ) : os.rename  ( file       , file + app )

    ## remove/backup the previous history file
    _rename_ ( __history__ , '.OLD' )

    ## line completer 
    import rlcompleter
    import readline
    readline.parse_and_bind("tab: complete")
    
    ## write history at the end 
    def _prnt_() :
        print 'BENDER history file: %s' % __history__
        
    import atexit
    atexit.register ( readline.write_history_file , __history__ )
    atexit.register ( _prnt_ )

    _prnt_() 
    
except :
    pass

# =============================================================================
# The END 
# =============================================================================
