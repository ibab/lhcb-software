#!/usr/bin/env python
# =============================================================================
## @file LoKiCore/math.py
#  helper file for redefine the standard mathematical functions
#  using pseudo-operators
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-07-17
# =============================================================================
"""
The set of basic math for various ROOT objects
"""
# =============================================================================
__author__  = "Vanya BELYAEV ibelyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $" 
# =============================================================================
__all__     = ()
# =============================================================================
from GaudiPython.Bindings import gbl as cpp


_P3D = cpp.Gaudi.XYZPoint
_V3D = cpp.Gaudi.XYZVector
_V4D = cpp.Gaudi.LorentzVector

def _o_mul_ ( self , other ) :
    _typ  = self.__class__
    tmp   = _typ ( self )
    tmp  *= other 
    return tmp

def _o_add_ ( self , other ) :
    _typ  = self.__class__
    tmp   = _typ ( self )
    tmp  += other 
    return tmp

def _o_sub_ ( self , other ) :
    _typ  = self.__class__
    tmp   = _typ ( self )
    tmp  -= other 
    return tmp

def _o1_str_ ( self ) :
    return "( %g, %g, %g)" % ( self.X() , self.Y( ), self.Z() )
def _o2_str_ ( self ) :
    return "[( %g, %g, %g), %g]" % ( self.X() , self.Y( ), self.Z() , self.E() )


if not hasattr ( _P3D , '__mul__' ) : _P3D.__mul__ = _o_mul_
if not hasattr ( _V3D , '__mul__' ) : _V3D.__mul__ = _o_mul_
if not hasattr ( _V4D , '__mul__' ) : _V4D.__mul__ = _o_mul_

if not hasattr ( _P3D , '__add__' ) : _P3D.__add__ = _o_add_
if not hasattr ( _V3D , '__add__' ) : _V3D.__add__ = _o_add_
if not hasattr ( _V4D , '__add__' ) : _V4D.__add__ = _o_add_

if not hasattr ( _P3D , '__sub__' ) : _P3D.__sub__ = _o_sub_
if not hasattr ( _V3D , '__sub__' ) : _V3D.__sub__ = _o_sub_
if not hasattr ( _V4D , '__sub__' ) : _V4D.__sub__ = _o_sub_
    
    
_P3D.__str__  = _o1_str_
_V3D.__str__  = _o1_str_
_V4D.__str__  = _o2_str_

_P3D.__repr__ = _o1_str_
_V3D.__repr__ = _o1_str_
_V4D.__repr__ = _o2_str_

    
# =============================================================================
if '__main__' == __name__ :
    print dir() 
# =============================================================================

# =============================================================================
# The end 
# =============================================================================
