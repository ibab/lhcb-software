#!/usr/bin/env python
## -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
## @file 	
#  Trivial script get list of input fiels from list of configuration files 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-01-28
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$
#
# =============================================================================
"""
Trivial script get list of input fiels from list of configuration files 
"""
# =============================================================================
__author__  = "Vanya BELYAEV"
__date__    = "2016-01-28"
__version__ = "Revision"
# =============================================================================
import sys,os 
from   Gaudi.Configuration import importOptions

## loop over command line arguments 
for a in sys.argv[1:] :
    
    fname = str(a)
    fname = os.path.expandvars ( fname )
    fname = os.path.expandvars ( fname )
    fname = os.path.expandvars ( fname )
    fname = os.path.expanduser ( fname )
    
    if not os.path.exists ( fname ) : continue 
    if not os.path.isfile ( fname ) : continue 

    ## import options from configuration files 
    try : 
        importOptions( fname )
    except : pass

## 
from Configurables import EventSelector

evtsel = EventSelector()
flist  = evtsel.Input 
print '##  number of Input files %s' % len(flist)
print flist  

# =============================================================================
# The END 
# =============================================================================
