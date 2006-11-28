#!/usr/bin/env python
# =============================================================================
# $Id: benderstartup.py,v 1.4 2006-11-28 18:24:17 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.4 $
# =============================================================================
## The trivial startup sctript frp python Bender session
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
""" This is a trivial startup script for python Bender session """
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@physics.syr.edu'
# =============================================================================


try:
    import os
    ## define the name of the history file 
    __history__ = os.environ['PWD'] + os.sep + '.BenderHistory'
    
    def _rename_ ( file , app ) :
        if os.path.exists( file + app ) :
            _rename_  ( file + app , app  )
        if os.path.exists( file )       :
            os.rename ( file , file + app )

        
    ## remove/backup the previous history file
    _rename_ ( __history__ , '.OLD' )
    
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
# The END 
# =============================================================================
