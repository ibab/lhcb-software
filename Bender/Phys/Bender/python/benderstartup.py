#!/usr/bin/env python
# =============================================================================
# $Id: benderstartup.py,v 1.2 2006-10-11 14:45:11 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.2 $
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
""" This is a trivial startup script for python Bender session """
## @file
#  The trivial startup sctript frp python Bender session 
#  @author Vanya BELYAEV ibelyaev@physcis.syr.edu
#  @date   2006-10-08
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@physics.syr.edu'


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
    _rename ( __history__ , '.OLD' )
    
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
