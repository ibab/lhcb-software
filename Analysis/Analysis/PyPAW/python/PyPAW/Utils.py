#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with some simple but useful utilities
#   - timing
#   - memory
#   - suppression of stdout/stderr 
#   - dumpting of stdout/stderr into file 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-02-10
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""
Module with some simple but useful utilities 

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2013-02-10"
# =============================================================================
__all__     = (
    #
    'virtualMemory'  , ## context manager to count virtual memory increase 
    'memory'         , ## ditto
    'clocks'         , ## context manager to count clocks 
    'timing'         , ## context manager to count time 
    'timer'          , ## ditto
    #
    'tee_py'         , ## tee for Python's printouts
    'tee_cpp'        , ## tee for C++'s    printouts
    'output'         , ## redirect stdout/stderr into the file 
    'mute_py'        , ## suppress stdout/strerr Python printout 
    'silence_py'     , ## ditto 
    'mute'           , ## context manager to suppress stdout/strerr printout 
    'silence'        , ## ditto 
    'rooSilent'      , ## control RooFit verbosity
    'NoContext'      , ## empty context manager 
    )
# =============================================================================
import warnings
warnings.warn (
    """PyPAW:
    Use 'Ostap.Utils' module instead of 'PyPAW.Utils'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from Ostap.Utils import *


# =============================================================================
if '__main__' == __name__ :
    
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' Symbols : %s'  %  list (  __all__ ) 
    print '*'*120
    
# =============================================================================
# The END 
# =============================================================================
