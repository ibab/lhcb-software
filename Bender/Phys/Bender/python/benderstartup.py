#!/usr/bin/env python
# =============================================================================
# $Id: benderstartup.py,v 1.1 2005-02-08 11:29:30 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ ; version $Revision: 1.1 $
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
""" This is a trivial startup script for Python/Bender """
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'

try:
    import os
    #define the name of history file 
    __history__ = os.environ['PWD'] + os.sep + '.BenderHistory'
    # remove previus histor file 
    if os.path.exists ( __history__ ) :
        os.rename( __history__  , __history__ + '.OLD')
    # lne completer 
    import rlcompleter
    import readline
    readline.parse_and_bind("tab: complete")
    # write history at the end 
    import atexit
    atexit.register( readline.write_history_file , __history__ )
except :
    pass

# =============================================================================
# The END 
# =============================================================================
