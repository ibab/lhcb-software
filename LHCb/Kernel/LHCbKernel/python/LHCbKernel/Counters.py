# $Id$
# =============================================================================
## @file
#  Helper module for dcoration of various TES-Counters with the proper
#   mapping interface 
#
#  @see Gaudi::Counter 
#  @see Gaudi::Counters 
#  @see Gaudi::Numbrs
#
#  @see GaudiUtils::VectorMap
#  @see Gaudi::Utils::MapBase
# 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2010-11-09
#
#                    $Revision$
#  Last modification $Date$
#                 by $    $
#
# =============================================================================
"""
Helper module for decoration of various ``TES-Counters'' with
the proper mapping interface 

>>> import LHCbKernel.Counters

"""

# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__version__ = "$Revision$"
__date__    = " 2010-11-09"
## NB: nothing to be imported 
__all__     = ()                                   # NB: nothing to be imported 
# =============================================================================

from   GaudiPython.Bindings      import gbl as cpp

from   GaudiPython.Pythonizations import ( __mapbase_setitem__ ,
                                           __mapbase_delitem__ ) 

std   = cpp.std
Gaudi = cpp.Gaudi

## the actual types
Gaudi.Counters.Key     = str
Gaudi.Counters.Counter = cpp.StatEntity 
Gaudi.Counters.Value   = cpp.StatEntity 
Gaudi.Counters.Map     = type( Gaudi.Counters().counters() ) 
## the actual types
Gaudi.Numbers.Key      = str
Gaudi.Numbers.Counter  = float 
Gaudi.Numbers.Value    = float
Gaudi.Numbers.Map      = type( Gaudi.Numbers().counters() ) 

# =============================================================================

for c in ( Gaudi.Counters ,
           Gaudi.Numbers  ) :

    c . Map . __getitem__ = Gaudi.Utils.MapBase . __getitem__ 
    c . Map . __setitem__ =               __mapbase_setitem__
    c . Map . __delitem__ =               __mapbase_delitem__
    
    c . __iter__      = lambda s         : s . counters () . __iter__      (   ) 
    c .   keys        = lambda s         : s . counters () .   keys        (   ) 
    c . __iteritems__ = lambda s         : s . counters () . __iteritems__ (   )
    c .   iteritems   = lambda s         : s . counters () .   iteritems   (   ) 
    c .   items       = lambda s         : s . counters () .       items   (   ) 
    c .   values      = lambda s         : s . counters () .   values      (   ) 
    c . __contains__  = lambda s , k     : s . counters () . __contains__  ( k ) 
    c . __has_key__   = lambda s , k     : s . counters () . __has_key__   ( k ) 
    c .   has_key     = lambda s , k     : s . counters () . __has_key__   ( k ) 
    c .   get         = lambda s , k , v = None : s . counters () .   get  ( k , v ) 
    c . __getitem__   = lambda s , k     : s . counters () . __getitem__   ( k ) 
    c . __setitem__   = lambda s , k , v : s . counters () . __setitem__   ( k , v ) 
    c . __delitem__   = lambda s , k     : s . counters () . __delitem__   ( k ) 
    c .   at          = lambda s , k     : s . counters () .   at          ( k ) 

# =============================================================================
if '__main__' == __name__ :
    
    print 120*'*'
    print __doc__
    print ' Author  : ' , __author__ 
    print ' Version : ' , __version__ 
    print ' Date    : ' , __date__ 
    print 120*'*'
    
    
# =============================================================================
# The END 
# =============================================================================
