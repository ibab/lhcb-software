#!/usr/bin/env python
# =============================================================================
## @file
#  The set of basic decorators for Hlt-event model objects 
#
#        This file is a part of LoKi project -
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
#  contributions and advices from G.Raven, J.van Tilburg,
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  By usage of this code one clearly states the disagreement
#  with the campain of Dr.O.Callot et al.:
#   ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#
#                   $Revision$
# Last Modification $Date$
#                by $Author$
#
#  @author Vanya BELYAEV  Ivan.BElyaev@cern.ch
#  @date 2011-02-03
# =============================================================================
"""
The set of basic decorators for Hlt-event model objects 

 This file is a part of LoKi project -
 ``C++ ToolKit  for Smart and Friendly Physics Analysis''
 
 The package has been designed with the kind help from
 Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 contributions and advices from G.Raven, J.van Tilburg,
 A.Golutvin, P.Koppenburg have been used in the design.

 By usage of this code one clearly states the disagreement
 with the campain of Dr.O.Callot et al.:
  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.h"
__date__    = "2011-02-03"
__version__ = "$Revision$"
# =============================================================================

from LoKiCore.basic import cpp, std, LoKi

LHCb     = cpp.LHCb
Hlt      = cpp.Hlt

_TC      = 'const Hlt::Candidate*'
_TS      = 'const Hlt::Stage*'

vTC      = std.vector ( _TC )
vTS      = std.vector ( _TS )

# =============================================================================
## add iterators over involved smart ref-vectors 
import LoKiCore.functions  as _LCF 
_t1      = Hlt.Candidate
_t2      = Hlt.Stage 
_t3      = LHCb.Track
for _t in  ( _t2 , _t3 ) :
    _tt = cpp.SmartRefVector ( _t )
    # redefine the iterator 
    _tt.__iter__ = _LCF._iter_SRV_
    
# =============================================================================
## iterator over stages in Hlt.Candidate
#  @see Hlt.Candidate 
#  @see Hlt.Candidate::get_
#  @author Vanya BELYAEV  Ivan.BElyaev@cern.ch
#  @date 2011-02-03
def _tc_iter_ ( self ) :
    """
    Iterator over stages in Hlt.Candidate
    Iteration is pefformed in inverse chronological order :
     - ``current'' stage is the first
     - ``initiator'' stage is the last

     >>> candidate = ...
     >>> for object in candidate : print object
     
     """
    size = vct.stages().size() 
    slot = 0
    while index < size :
        o = vct.get_( slot ) 
        yield o
        index += 1
        
## attach it to trigger candidate !
Hlt.Candidate.__iter__ = _tc_iter_ 

# =============================================================================
if '__main__' == __name__ :
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__
    print 80*'*'

# =============================================================================
# The END
# =============================================================================
