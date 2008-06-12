#!/usr/bin/env python
# =============================================================================
# $Id: Startup.py,v 1.2 2008-06-12 10:54:21 ibelyaev Exp $ 
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


print ' STARTUP (1) '

try:

    print ' STARTUP (2) '
    import os
    print ' STARTUP (3) '
    ## define the name of the history file
    dr = os.environ.get('PWD',None)
    if not dr : dr = '.'
    __history__ = dr + os.sep + '.BenderHistory'
    print ' STARTUP (4) ', __history__
    
    def _rename_ ( file , app ) :
        print ' STARTUP (5.1) '
        if os.path.exists( file + app ) :
            print ' STARTUP (5.2) '
            _rename_  ( file + app , app  )
        print ' STARTUP (5.3) '
        if os.path.exists( file )       :
            print ' STARTUP (5.4) '
            os.rename ( file , file + app )
        print ' STARTUP (5.5) '

    print ' STARTUP (5) '
        
    ## remove/backup the previous history file
    _rename_ ( __history__ , '.OLD' )

    print ' STARTUP (6) '

    ## line completer 
    import rlcompleter
    import readline
    readline.parse_and_bind("tab: complete")
    
    ## write history at the end 
    import atexit
    atexit.register( readline.write_history_file , __history__ )
    
except :
    pass

# =============================================================================
# The END 
# =============================================================================
