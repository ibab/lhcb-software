#!/usr/bin/env python 
# =============================================================================
## @file Benedr/Fixes_LoKi.py
#  The helper Python module for Bender application 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
# 
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#
#                    $Revision: 129470 $
#  Last modification $Date: 2011-09-26 12:40:16 +0200 (Mon, 26 Sep 2011) $
#                 by $Author: ibelyaev $ 
# =============================================================================
"""
The helper module for Bender application.
It applies some last-moment (version-dependent) fixes

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
__date__    = "2004-07-11"
__version__ = '$Revision: 129470 $'
__all__     = ()
# =============================================================================

print '*'*120
print 'Bender.Fixes: - Fix some LoKi   features'
print '*'*120

# =============================================================================
## decorate the ranges
# =============================================================================
from   LoKiCore.basic      import std, Gaudi, LHCb, cpp 
import LoKiCore.decorators as     LCD

HepMC   = cpp.HepMC 
_iter_  = LCD._iter_1_
_slice_ = LCD._slice_

_rb     = Gaudi.RangeBase_
if not hasattr ( _rb , '__iter__'     ) or _rb.__iter__     != _iter_  : 
    _rb .__iter__     = _iter_  
    print 'Bender.Fixes: - Fix the iterator problem with %s' % type(_rb) 
if not hasattr ( _rb , '__getslice__' ) or _rb.__getslice__ != _slice_ : 
    _rb .__getslice__ = _slice_  
    print 'Bender.Fixes: - Fix the slicing  problem with %s' % type(_rb) 

import LoKiPhys.Phys
import LoKiMC.MC 
import LoKiGen.HepMC 

for _t in ( LHCb.Particle     ,
            #LHCb.VertexBase  , 
            LHCb.MCParticle   ,
            LHCb.MCVertex     ,
            HepMC.GenVertex   , 
            HepMC.GenParticle ) : 
    
    _r = _t.Range
    
    if not hasattr ( _r , '__iter__'     ) or _r.__iter__     != _iter_  : 
        _r .__iter__     = _iter_  
        print 'Bender.Fixes: - Fix iterator problem with %s' % type(_r) 
    if not hasattr ( _r , '__getslice__' ) or _r.__getslice__ != _slice_ : 
        _r .__getslice__ = _slice_  
        print 'Bender.Fixes: - Fix slicing  problem with %s' % type(_r) 
        
for t in  ( Gaudi.Range_      ( std.vector ( 'const LHCb::Particle*'     ) ) ,
            Gaudi.Range_      ( std.vector ( 'const LHCb::VertexBase*'   ) ) ,
            Gaudi.NamedRange_ ( std.vector ( 'const LHCb::VertexBase*'   ) ) ,
            Gaudi.Range_      ( std.vector ( 'const LHCb::MCParticle*'   ) ) ,
            Gaudi.Range_      ( std.vector ( 'const LHCb::MCVertex*'     ) ) ,
            Gaudi.Range_      ( std.vector ( 'const HepMC::GenParticle*' ) ) ,
            Gaudi.Range_      ( std.vector ( 'const HepMC::GenVertex*'   ) ) ) :
    
    _r = t 
    if not hasattr ( _r , '__iter__'     ) or _r.__iter__     != _iter_  : 
        _r .__iter__     = _iter_ 
        print 'Bender.Fixes: - Fix iterator problem with %s' % type(_r) 
    if not hasattr ( _r , '__getslice__' ) or _r.__getslice__ != _slice_ : 
        _r .__getslice__ = _slice_  
        print 'Bender.Fixes: - Fix slicing  problem with %s' % type(_r) 

            
print '*'*120


# =============================================================================
## fix for LoKiPhys 
LHCb.Particle.Container = cpp.KeyedContainer(LHCb.Particle,'Containers::KeyedObjectManager<Containers::hashmap>')
LHCb.Particle.Selection = cpp.SharedObjectsContainer (LHCb.Particle)

LHCb.Particle.Selection.__iter__    = _iter_  
        

# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120

# =============================================================================
# The END 
# =============================================================================
