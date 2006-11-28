# =============================================================================
# $Id: benderrange.py,v 1.4 2006-11-28 18:24:17 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
# =============================================================================
## This is helper module for decoration of various LoKi.Range functions
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
#  @see LoKi::Range_
#  @see LoKi::NamedRange_
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" This is helper module for decoration of various LoKi.Range functions """
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

import gaudimodule

_gbl  = gaudimodule.gbl
_LoKi = _gbl.LoKi
#_LHCb = _gbl.LHCb

#print ' GBL   is %s/%s' % ( _gbl  , type( _gbl  ) )
#print ' LHCb  is %s/%s' % ( _LHCb , type( _LHCb ) )
#print ' LoKi  is %s/%s/%s' % ( _LoKi , type( _LoKi ), dir(_LoKi) )

_Base = _LoKi.RangeBase_

## helper adapter fuction to provide python semantic : len(object)
def _len_  ( self ) :
    """
    Helper adapter fuction to provide python semantic : len(object)
    """
    return self.size()

_Base.__len__ = lambda s: _len_(s) 

## helper fuction for iteration over the range
def _iter_ ( self ) :
    """
    Helper fuction for iteration over the range
    """
    _r   = self
    _num = _r.size()
    _i   = 0 
    while _i < _num :
        yield _r.at(_i)
        _i += 1

_Base.__iter__ = lambda s: _iter_(s) 


_Range = _LoKi.NamedRange_

Range    = _Range ( 'std::vector<const LHCb::Particle*>'     )
VRange   = _Range ( 'std::vector<const LHCb::VertexBase*>'   )
MCRange  = _Range ( 'std::vector<const LHCb::MCParticle*>'   )
MCVRange = _Range ( 'std::vector<const LHCb::MCVertex*>'     )
GRange   = _Range ( 'std::vector<const HepMC::GenParticle*>' )
GVRange  = _Range ( 'std::vector<const HepMC::GenVertex*>'   )

_ranges_ = ( Range , VRange , MCRange , MCVRange , GRange , GVRange )

if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 


# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
