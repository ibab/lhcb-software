#!/usr/bin/env python
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  helper module for decoration of standard N-tuples
#
#  This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2012-02-16
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""
Helper module for decoration of standard N-tuples

This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = "2012-02-16"
__version__ = "$Revision$"
# =============================================================================
from LoKiCore.basic import cpp


_Tuple  = cpp.Tuples.Tuple
_ColAux = cpp.LoKi.Dicts.TupleAux 

## simple function for decoration of "Tuples::Tuple::farray" method 
def _colAux_ ( self , *args ) :
    """
    Simple function for decoration of 'Tuples::Tuple::column_aux' method
    
    time = ...                    ## Gaudi::Time object
    tup.column_aux ( 't' , time ) ## name/prefix is optional
    tup.column_aux ( time )

    odin = ...                    ## LHCb::Odin object
    tup.column_aux ( 'o' , odin ) ## name/prefix is optional
    tup.column_aux ( odin )
    
    rhdr = ...                     ## LHCb::RecHeader object  
    tup.column_aux ( 'r' , rhdr )  ## name/prefix is optional
    tup.column_aux ( rhdr )

    rsum = ...                     ## LHCb::RecSummary object  
    tup.column_aux ( 's' , rsum )  ## name/prefix is optional
    tup.column_aux ( rsum )

    rnum = ...                     ## Gaudi::Numbers object  
    tup.column_aux ( 'n' , rnum )  ## name/prefix is optional
    tup.column_aux ( rnum )
    
    """
    return _ColAux.column ( self ,*args )

_colAux_ . __doc__ += '\n' + _ColAux_ . column . __doc__

# =============================================================================
## finally decorate tuple 
_Tuple.column_aux   = _colAux_


# =============================================================================
# The END 
# =============================================================================
