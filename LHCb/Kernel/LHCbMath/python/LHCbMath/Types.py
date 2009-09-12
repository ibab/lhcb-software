#!/usr/bin/env python
# =============================================================================
# $Id: Types.py,v 1.1 2009-09-12 19:29:26 ibelyaev Exp $
# =============================================================================
## @file
#
#  Simple file to provide "easy" access in python for
#  the basic ROOT::Math classes
#  @see GaudiKernel/Point3DTypes.h
#  @see GaudiKernel/Vector3DTypes.h
#  @see GaudiKernel/Vector4DTypes.h
#  @see GaudiKernel/GenericVectorTypes.h
#
#  The usage is fairly trivial:
#
#  @code
#
#  import LHCbMath.Types
#
#  @endcode
#
#  Important: All types are defined in corresponding C++ namespaces 
#
#  @code
#
#  import LHCbMath.Types
#
#  from GaudiPython.Bindings import gbl as cpp ## get global C++ namespace
#  Gaudi = cpp.Gaudi                           ## get C++ namespace Gaudi
#
#  p3 = Gaudi.XYZPoint(0,1,2)               ## use C++ type Gaudi::XYZPoint
#
#  @endcode
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-09-12
# =============================================================================
"""

Simple file to provide 'easy' access in python for the basic ROOT::Math classes
  
  see $GAUDIKERNELROOT/GaudiKernel/Point3DTypes.h
  see $GAUDIKERNELROOT/GaudiKernel/Vector3DTypes.h
  see $GAUDIKERNELROOT/GaudiKernel/Vector4DTypes.h
  see $GAUDIKERNELROOT/GaudiKernel/GenericVectorTypes.h
  
  The usage is fairly trivial:
  
  >>> import LHCbMath.Types
  
  Important: All types are defined in corresponding
               C++ namespaces: Gaudi & Gaudi::Math
  
  >>> import LHCbMath.Types
  >>> from GaudiPython.Bindings import gbl as cpp ## get global C++ namespace
  >>> Gaudi = cpp.Gaudi                           ## get C++ namespace Gaudi
  >>> p3 = Gaudi.XYZPoint(0,1,2)               ## use C++ type Gaudi::XYZPoint
  

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__     = ()  ## nothing to be imported !
# =============================================================================

## get global C++ namespace 
from GaudiPython.Bindings import gbl as cpp

## C++ namespace Gaudi
Gaudi = cpp.Gaudi

## ROOT::Math namespace 
_RM = cpp.ROOT.Math

if not hasattr ( Gaudi , 'XYZPoint'      ) : 
    Gaudi.XYZPoint      = _RM.PositionVector3D     ('ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag')
if not hasattr ( Gaudi , 'XYZVector'     ) :     
    Gaudi.XYZVector     = _RM.DisplacementVector3D ('ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag')
if not hasattr ( Gaudi , 'LorentzVector' ) :     
    Gaudi.LorentzVector = _RM.LorentzVector        ('ROOT::Math::PxPyPzE4D<double>')

if not hasattr ( Gaudi , 'Plane3D'  ) :
    Gaudi.Plane3D       = _RM.Plane3D

    
if not hasattr ( Gaudi , 'Vector2' )  :
    Gaudi.Vector2      = _RM.SVector( 'double' , 2 )
if not hasattr ( Gaudi , 'Vector3' )  :
    Gaudi.Vector3      = _RM.SVector( 'double' , 3 )
if not hasattr ( Gaudi , 'Vector4' )  :
    Gaudi.Vector4      = _RM.SVector( 'double' , 4 )
if not hasattr ( Gaudi , 'Vector5' )  :
    Gaudi.Vector5      = _RM.SVector( 'double' , 5 )

if not hasattr ( Gaudi , 'SymMatrix3x3' ) :
    Gaudi.SymMatrix3x3 = _RM.SMatrix('double,3,3,ROOT::Math::MatRepSym<double,3>')
    
if not hasattr ( Gaudi , 'SymMatrix4x4' ) :
    Gaudi.SymMatrix4x4 = _RM.SMatrix('double,4,4,ROOT::Math::MatRepSym<double,4>')
    
## Gaudi::Math
        
if not hasattr ( Gaudi.Math , 'XYZLine' ) :
    Gaudi.Math.XYZLine = cpp.Gaudi.Math.Line(Gaudi.XYZPoint,Gaudi.XYZVector)

if not hasattr ( Gaudi.Math , 'SVector3WithError' ) :
    Gaudi.Math.SVector3WithError = cpp.Gaudi.Math.SVectorWithError(3,'double')
    
if not hasattr ( Gaudi.Math , 'SVector4WithError' ) :
    Gaudi.Math.SVector4WithError = cpp.Gaudi.Math.SVectorWithError(4,'double')


## ============================================================================
## some useful decoration:
## ============================================================================

_P3D = Gaudi.XYZPoint
_V3D = Gaudi.XYZVector
_V4D = Gaudi.LorentzVector

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


## Self-printout of 3D-points and 3D-vectors
def _o1_str_ ( self ) :
    """    
    Self-printout of 3D-points and 3D-vectors
    
    """
    return "( %g, %g, %g)"       % ( self.X() , self.Y( ), self.Z() )

## Self-printout of 4D-vectors 
def _o2_str_ ( self ) :
    """    
    Self-printout of 4D-vectors 
    
    """
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


## Self-printout of line
def _l_str_ ( self ) :
    """
    Self-printout of line: (point, direction)
    """
    return "(%s,%s)" % ( self.beginPoint() , self.direction() )

Gaudi.Math.XYZLine.__str__  = _l_str_
Gaudi.Math.XYZLine.__repr__ = _l_str_


## Self-printout of 3D-plane
def _p_str_ ( self ) :
    """
    Self-printout of 3D-plane: (point, normal)
    """
    return "(%s,%s)" % ( self.ProjectOntoPlane( Gaudi.XYZPoint()) , self.Normal() )

Gaudi.Plane3D.__str__   = _p_str_
Gaudi.Plane3D.__repr__  = _p_str_

def _v_str_ ( self ) :
    """
    Self-printout of SVectors: (...)
    """
    index  = 0
    result = ''
    while index < self.kSize :
        if 0 != index : result += ', '
        result += " %g" % self.At( index )
        index  += 1 
    return "( " + result + ' )'

for t in  ( Gaudi.Vector2 ,
            Gaudi.Vector3 ,
            Gaudi.Vector4 ,
            Gaudi.Vector5 ) :
    
    t.__str__  = _v_str_
    t.__repr__ = _v_str_


Gaudi.Math.ValueWithError.__str__      = Gaudi.Math.ValueWithError.toString 
Gaudi.Math.ValueWithError.__repr__     = Gaudi.Math.ValueWithError.toString 

Gaudi.Math.Point3DWithError.__str__    = Gaudi.Math.Point3DWithError.toString 
Gaudi.Math.Point3DWithError.__repr__   = Gaudi.Math.Point3DWithError.toString

Gaudi.Math.Vector3DWithError.__str__   = Gaudi.Math.Vector3DWithError.toString 
Gaudi.Math.Vector3DWithError.__repr__  = Gaudi.Math.Vector3DWithError.toString 

Gaudi.Math.LorentzVectorWithError.__str__  = Gaudi.Math.LorentzVectorWithError.toString 
Gaudi.Math.LorentzVectorWithError.__repr__ = Gaudi.Math.LorentzVectorWithError.toString 

Gaudi.Math.SVector3WithError.__str__   = Gaudi.Math.SVector3WithError.toString 
Gaudi.Math.SVector3WithError.__repr__  = Gaudi.Math.SVector3WithError.toString 

Gaudi.Math.SVector4WithError.__str__   = Gaudi.Math.SVector4WithError.toString 
Gaudi.Math.SVector4WithError.__repr__  = Gaudi.Math.SVector4WithError.toString 


if '__main__' == __name__ :
    print __doc__ , __author__ , dir ()
    
    
# =============================================================================
# The  END 
# =============================================================================
