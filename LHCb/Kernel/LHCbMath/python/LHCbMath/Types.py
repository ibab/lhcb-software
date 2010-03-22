#!/usr/bin/env python
# =============================================================================
# $Id: Types.py,v 1.4 2010-03-22 14:08:19 ibelyaev Exp $
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
  
  see $LHCBMATHROOT/LHCbMath/Line.h 
    
  The lines and planes are decorated:
     see $LHCBMATHROOT/LHCbMath/GeomFun.h

     
  The usage is fairly trivial:
  
  >>> import LHCbMath.Types
  
  Important: All types are defined in corresponding
               C++ namespaces: Gaudi & Gaudi::Math
  
  >>> import LHCbMath.Types
  >>> from GaudiPython.Bindings import gbl as cpp ## get global C++ namespace
  >>> Gaudi = cpp.Gaudi                           ## get C++ namespace Gaudi
  >>> p3 = Gaudi.XYZPoint(0,1,2)               ## use C++ type Gaudi::XYZPoint

  >>> dir( Gaudi.Math )
  >>> dir( Gaudi      )
  

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2009-09-12 "
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $ "
# =============================================================================
__all__     = ()  ## nothing to be imported !
# =============================================================================
import ROOT

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
if not hasattr ( Gaudi      , 'Plane3D'  ) :
    Gaudi.Plane3D       = _RM.Plane3D

if not hasattr ( Gaudi.Math , 'XYZPoint'       ) : Gaudi.Math.XYZPoint       = Gaudi.XYZPoint
if not hasattr ( Gaudi.Math , 'XYZVector'      ) : Gaudi.Math.XYZVector      = Gaudi.XYZVector
if not hasattr ( Gaudi.Math , 'LorentzVector'  ) : Gaudi.Math.LorentzVector  = Gaudi.LorentzVector    
if not hasattr ( Gaudi.Math , 'Plane3D'        ) : Gaudi.Math.Plane3D  = Gaudi.Plane3D  

if not hasattr ( Gaudi      , 'Point3D'        ) : Gaudi.Point3D            = Gaudi.XYZPoint
if not hasattr ( Gaudi.Math , 'Point3D'        ) : Gaudi.Math.Point3D       = Gaudi.XYZPoint

if not hasattr ( Gaudi      , 'Vector3D'       ) : Gaudi.Vector3D           = Gaudi.XYZVector
if not hasattr ( Gaudi.Math , 'Vector3D'       ) : Gaudi.Math.Vector3D      = Gaudi.XYZVector

    
if not hasattr ( Gaudi , 'Vector2' )  :
    Gaudi.Vector2      = _RM.SVector( 'double' , 2 )
if not hasattr ( Gaudi , 'Vector3' )  :
    Gaudi.Vector3      = _RM.SVector( 'double' , 3 )
if not hasattr ( Gaudi , 'Vector4' )  :
    Gaudi.Vector4      = _RM.SVector( 'double' , 4 )
if not hasattr ( Gaudi , 'Vector5' )  :
    Gaudi.Vector5      = _RM.SVector( 'double' , 5 )

if not hasattr ( Gaudi.Math , 'Vector2' ) : Gaudi.Math.Vector2 = Gaudi.Vector2 
if not hasattr ( Gaudi.Math , 'Vector3' ) : Gaudi.Math.Vector3 = Gaudi.Vector3 
if not hasattr ( Gaudi.Math , 'Vector4' ) : Gaudi.Math.Vector4 = Gaudi.Vector4 
if not hasattr ( Gaudi.Math , 'Vector5' ) : Gaudi.Math.Vector5 = Gaudi.Vector5 

if not hasattr ( Gaudi , 'SymMatrix2x2' ) :
    Gaudi.SymMatrix2x2 = _RM.SMatrix('double,2,2,ROOT::Math::MatRepSym<double,2>')
if not hasattr ( Gaudi , 'SymMatrix3x3' ) :
    Gaudi.SymMatrix3x3 = _RM.SMatrix('double,3,3,ROOT::Math::MatRepSym<double,3>')    
if not hasattr ( Gaudi , 'SymMatrix4x4' ) :
    Gaudi.SymMatrix4x4 = _RM.SMatrix('double,4,4,ROOT::Math::MatRepSym<double,4>')
if not hasattr ( Gaudi , 'SymMatrix5x5' ) :
    Gaudi.SymMatrix5x5 = _RM.SMatrix('double,5,5,ROOT::Math::MatRepSym<double,5>')
if not hasattr ( Gaudi , 'SymMatrix6x6' ) :
    Gaudi.SymMatrix6x6 = _RM.SMatrix('double,6,6,ROOT::Math::MatRepSym<double,6>')
## LHCb::Particle
if not hasattr ( Gaudi , 'SymMatrix7x7' ) :
    Gaudi.SymMatrix7x7 = _RM.SMatrix('double,7,7,ROOT::Math::MatRepSym<double,7>')
## LHCb:TwoProngVertex
if not hasattr ( Gaudi , 'SymMatrix9x9' ) :
    Gaudi.SymMatrix9x9 = _RM.SMatrix('double,9,9,ROOT::Math::MatRepSym<double,9>')


if not hasattr ( Gaudi.Math , 'SymMatrix2x2' ) : Gaudi.Math.SymMatrix3x3 = Gaudi.SymMatrix2x2 
if not hasattr ( Gaudi.Math , 'SymMatrix3x3' ) : Gaudi.Math.SymMatrix3x3 = Gaudi.SymMatrix3x3 
if not hasattr ( Gaudi.Math , 'SymMatrix4x4' ) : Gaudi.Math.SymMatrix4x4 = Gaudi.SymMatrix4x4
if not hasattr ( Gaudi.Math , 'SymMatrix5x5' ) : Gaudi.Math.SymMatrix5x5 = Gaudi.SymMatrix5x5 
if not hasattr ( Gaudi.Math , 'SymMatrix6x6' ) : Gaudi.Math.SymMatrix5x5 = Gaudi.SymMatrix6x6 
if not hasattr ( Gaudi.Math , 'SymMatrix7x7' ) : Gaudi.Math.SymMatrix5x5 = Gaudi.SymMatrix7x7 
if not hasattr ( Gaudi.Math , 'SymMatrix9x9' ) : Gaudi.Math.SymMatrix5x5 = Gaudi.SymMatrix9x9 

#
# specific matrices for 'tracks'
#

if not hasattr ( Gaudi      , 'Matrix5x5'      ) : Gaudi.Matrix5x5             = _RM.SMatrix('double,5,5')
if not hasattr ( Gaudi      , 'TrackMatrix'    ) : Gaudi.TrackMatrix           = Gaudi.Matrix5x5 
if not hasattr ( Gaudi.Math , 'Matrix5x5'      ) : Gaudi.Math.Matrix5x5        = Gaudi.Matrix5x5 
if not hasattr ( Gaudi.Math , 'TrackMatrix'    ) : Gaudi.Math.TrackMatrix      = Gaudi.TrackMatrix 

if not hasattr ( Gaudi      , 'TrackSymMatrix' ) : Gaudi.TrackSymMatrix        = Gaudi.SymMatrix5x5
if not hasattr ( Gaudi.Math , 'TrackSymMatrix' ) : Gaudi.Math.TrackSymMatrix   = Gaudi.TrackSymMatrix 

if not hasattr ( Gaudi      , 'TrackVector'    ) : Gaudi.TrackVector           = Gaudi.Vector5 
if not hasattr ( Gaudi.Math , 'TrackVector'    ) : Gaudi.Math.TrackVector      = Gaudi.TrackVector

#
# matrix from LHCb::Particle
#
if not hasattr ( Gaudi      , 'Matrix4x3'      ) : Gaudi.Matrix4x3             = _RM.SMatrix('double,4,3')
if not hasattr ( Gaudi.Math , 'Matrix4x3'      ) : Gaudi.Math.Matrix4x3        = Gaudi.Matrix4x3

    
## Gaudi::Math
        
if not hasattr ( Gaudi.Math , 'XYZLine' ) :
    Gaudi.Math.XYZLine = cpp.Gaudi.Math.Line(Gaudi.XYZPoint,Gaudi.XYZVector)

if not hasattr ( Gaudi.Math , 'SVector2WithError' ) :
    Gaudi.Math.SVector2WithError = cpp.Gaudi.Math.SVectorWithError(2,'double')
if not hasattr ( Gaudi.Math , 'SVector3WithError' ) :
    Gaudi.Math.SVector3WithError = cpp.Gaudi.Math.SVectorWithError(3,'double')
if not hasattr ( Gaudi.Math , 'SVector4WithError' ) :
    Gaudi.Math.SVector4WithError = cpp.Gaudi.Math.SVectorWithError(4,'double')
if not hasattr ( Gaudi.Math , 'SVector5WithError' ) :
    Gaudi.Math.SVector5WithError = cpp.Gaudi.Math.SVectorWithError(5,'double')

if not hasattr ( Gaudi      , 'XYZLine' ) : Gaudi.XYZLine     = Gaudi.Math.XYZLine
if not hasattr ( Gaudi      , 'Line3D'  ) : Gaudi.Line3D      = Gaudi.Math.XYZLine
if not hasattr ( Gaudi.Math , 'Line3D'  ) : Gaudi.Math.Line3D = Gaudi.Math.XYZLine


if not hasattr ( Gaudi.Math , 'frac' ) :
    Gaudi.Math.frac     = Gaudi.Math.Functions.frac
if not hasattr ( Gaudi.Math , 'asym' ) :
    Gaudi.Math.asym     = Gaudi.Math.Functions.asym
if not hasattr ( Gaudi.Math , 'binomEff' ) :
    Gaudi.Math.binomEff = Gaudi.Math.Functions.binomEff 

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

    
if not hasattr ( _V3D , '__rmul__' ) : _V3D. __rmul__ = _V3D.__mul__ 
if not hasattr ( _V4D , '__rmul__' ) : _V4D. __rmul__ = _V4D.__mul__ 

    
_P3D.__str__  = _o1_str_
_V3D.__str__  = _o1_str_
_V4D.__str__  = _o2_str_

_P3D.__repr__ = _o1_str_
_V3D.__repr__ = _o1_str_
_V4D.__repr__ = _o2_str_

# =============================================================================
## Self-printout of line
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-09-12
def _l_str_ ( self ) :
    """
    Self-printout of line: (point, direction)
    """
    return "(%s,%s)" % ( self.beginPoint() , self.direction() )

Gaudi.Math.XYZLine.__str__  = _l_str_
Gaudi.Math.XYZLine.__repr__ = _l_str_

# =============================================================================
## Self-printout of 3D-plane
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-09-12
def _p_str_ ( self ) :
    """
    Self-printout of 3D-plane: (point, normal)
    """
    return "(%s,%s)" % ( self.ProjectOntoPlane( Gaudi.XYZPoint()) , self.Normal() )

Gaudi.Plane3D.__str__   = _p_str_
Gaudi.Plane3D.__repr__  = _p_str_

# =============================================================================
## self-printout of S-vectors 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-09-12
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

# =============================================================================
## various decorators for GeomFun.h
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
if not hasattr ( Gaudi.Math , 'XYZGeomFun' ) :
    Gaudi.Math.XYZGeomFun = cpp.Gaudi.Math.GF(
        Gaudi.XYZPoint  ,
        Gaudi.XYZLine   ,
        Gaudi.Plane3D 
        )
if not hasattr ( Gaudi , 'XYZGeomFun' ) :
    Gaudi.XYZGeomFun = Gaudi.Math.XYZGeomFun
    
_GeomFun = Gaudi.Math.XYZGeomFun


# =============================================================================
## intersection of line and plane 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __intersect_line_and_plane__ ( line , plane ) :
    """
    Find the intersection of line and plane

    >>> line  = ...
    >>> plane = ...
    >>> ok, point, mu = line.intersect ( plane )

    The return value is a tuple: 
    - the point
    - the parameter along the line
    - the flag (true if intersection exists)
    
    """
    _point = Gaudi.XYZPoint(0,0,-1.e+10)
    _mu    = ROOT.Double(-1.e+10)
    _flag  = _GeomFun.intersection ( line   ,
                                     plane  ,
                                     _point ,
                                     _mu    )
    if _flag : _flag = True
    else     : _flag = False 
    return (_point,_mu,_flag)
    
__intersect_line_and_plane__ . __doc__ += '\n' + _GeomFun.intersection . __doc__ 

if not hasattr ( Gaudi.XYZLine , 'intersect' ) :
    Gaudi.XYZLine.intersect = __intersect_line_and_plane__

# =============================================================================
## intersect two planes 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __intersect_two_planes__ ( plane , plane1 ) :
    """
    Find the intersection line for two planes:

    >>> plane  = ...
    >>> plane1 = ...
    >>> line, flag = plane.line(plane1)

    Return value is a tuple:
    
    - the intersection line
    - the flag (true if intersection exists)
    
    """
    _line = Gaudi.XYZLine()
    _flag = _GeomFun.intersection ( plane , plane1 , _line )
    if _flag : _flag = True
    else     : _flag = False 
    return (_line,_flag)

__intersect_two_planes__ . __doc__ += '\n' + _GeomFun.intersection . __doc__ 

if not hasattr ( Gaudi.Plane3D , 'line' ) :
    Gaudi.Plane3D.line = __intersect_two_planes__


# =============================================================================
## intersect three planes 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __intersect_three_planes__ ( plane , plane1 , plane2 ) :
    """
    Find the intersection point for three planes:

    >>> plane  = ...
    >>> plane1 = ...
    >>> plane3 = ...
    >>> point, flag = plane.point(plane1,plane2)

    Return value is a tuple:
    
    - the intersection point
    - the flag (true if intersection exists)
    
    """
    _point = Gaudi.XYZPoint(0,0,-1.e+10)
    _flag = _GeomFun.intersection ( plane , plane1 , plane2 , _point )
    if _flag : _flag = True
    else     : _flag = False 
    return (_point,_flag)


__intersect_three_planes__ . __doc__ += '\n' + _GeomFun.intersection . __doc__ 

if not hasattr ( Gaudi.Plane3D , 'point' ) :
    Gaudi.Plane3D.point = __intersect_three_planes__


# =============================================================================
## intersect the planes 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __intersect_the_planes__ ( plane , plane1 , plane2 = None ) :
    """
    Find the intersection line/point for tteo/three planes:

    >>> plane  = ...
    >>> plane1 = ...
    >>> line, flag = plane.intersect(plane1)

    >>> plane  = ...
    >>> plane1 = ...
    >>> plane2 = ...
    >>> point, flag = plane.intersect(plane1,plane2)

    Return value is a tuple:
    
    - the intersection line/point
    - the flag (true if intersection exists)
    
    """
    if not plane2 : return __intersect_two_planes__ ( plane , plane1 )
    return __intersect_three_planes__ ( plane , plane1 , plane2 )
    
__intersect_the_planes__ . __doc__ += '\n' + _GeomFun.intersection . __doc__ 

if not hasattr ( Gaudi.Plane3D , 'intersect' ) :
    Gaudi.Plane3D.intersect = __intersect_the_planes__


# =============================================================================
## calculate the impact parameter of the line & point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __imp_par_1__ ( line , point ) :
    """
    Calculate the impact parameter of the line and the point

    >>> line  = ...
    >>> point = ...
    >>> ip = line.impactParameter ( point )
    
    """
    return _GeomFun.impactParameter ( point , line )

__imp_par_1__ . __doc__ += '\n' + _GeomFun.impactParameter . __doc__ 

if not hasattr ( Gaudi.XYZLine , 'impactParameter' ) :
    Gaudi.XYZLine.impactParameter = __imp_par_1__
if not hasattr ( Gaudi.XYZLine , 'ip'              ) :
    Gaudi.XYZLine.ip              = __imp_par_1__

# =============================================================================
## calculate the impact parameter of the line & point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __imp_par_2__ ( point , line ) :
    """
    Calculate the impact parameter of the line and the point

    >>> point = ...
    >>> line  = ...
    >>> ip = point.impactParameter ( line )
    
    """
    return _GeomFun.impactParameter ( point , line )


__imp_par_2__ . __doc__ += '\n' + _GeomFun.impactParameter . __doc__ 

if not hasattr ( Gaudi.XYZPoint , 'impactParameter' ) :
    Gaudi.XYZPoint.impactParameter = __imp_par_2__
if not hasattr ( Gaudi.XYZPoint , 'ip'              ) :
    Gaudi.XYZPoint.ip              = __imp_par_2__



# =============================================================================
## distance between two lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __distance_between_two_lines__ ( line , line1 ) :
    """
    Find the distance netween two lines :

    >>> line  = ...
    >>> line1 = ...
    >>> dist = line.distance ( line1 )
    
    """
    return _GeomFun.distance ( line , line1 )


__distance_between_two_lines__ . __doc__ += '\n' + _GeomFun.distance. __doc__

if not hasattr ( Gaudi.XYZLine , 'distance' ) :
    Gaudi.XYZLine.distance = __distance_between_two_lines__


# =============================================================================
## find the closest points for two lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __closest_points__ ( line , line1 ) :
    """
    Calculate the two closest points between two lines

    >>> line1 = ...
    >>> line2 = ...
    >>> point1 , point2 , flag = line1.closestPoints ( line2 )

    The return values is a tuple:
    - the point onthe fist line
    - the point on the second line
    - the flag (true is everything OK)
    
    """
    _point1 = Gaudi.XYZPoint(0,0,-1.e+10)
    _point2 = Gaudi.XYZPoint(0,0,-1.e+11)
    _flag   = _GeomFun.closestPoints ( line , line1 , _point1 , _point2 )
    if    _flag : _flag = True
    else        : _flag = False 
    return (_point1,_point2,_flag)
    

__closest_points__ . __doc__ += '\n' + _GeomFun.closestPoints . __doc__

if not hasattr ( Gaudi.XYZLine , 'closestPoints' ) :
    Gaudi.XYZLine.closestPoints = __closest_points__


# =============================================================================
## find the closest points for two lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __closest_point_params__ ( line , line1 ) :
    """
    Calculate the parameters for two closest points between two lines

    >>> line1 = ...
    >>> line2 = ...
    >>> mu1 , mu2 , flag = line1.closestPointParams ( line2 )

    The return values is a tuple:
    - the 'mu-parameter of closest point along the first  line 
    - the 'mu-parameter of closest point along the second line 
    - the flag (true is everything OK)
    
    """
    _mu1    = ROOT.Double(-1.e+10)
    _mu2    = ROOT.Double(-1.e+11)
    _flag   = _GeomFun.closestPointParams ( line , line1 , _mu1 , _mu2 )
    if    _flag : _flag = True
    else        : _flag = False 
    return (_mu1,_mu2,_flag)
    

__closest_point_params__ . __doc__ += '\n' + _GeomFun.closestPointParams . __doc__

if not hasattr ( Gaudi.XYZLine , 'closestPointParams' ) :
    Gaudi.XYZLine.closestPointParams = __closest_point_params__

# =============================================================================
## find the point on ilne closest to the given point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __closest_point_1__ ( line , point ) :
    """
    Find the point on line closest to the given point

    >>> line  = ...
    >>> point = ...
    >>> ClosestPoint  = line.closestPoint ( point )
    
    """
    return _GeomFun.closestPoint ( point , line )

__closest_point_1__ . __doc__ += '\n' + _GeomFun.closestPoint . __doc__

if not hasattr ( Gaudi.XYZLine , 'closestPoint' ) :
    Gaudi.XYZLine.closestPoint = __closest_point_1__
    
# =============================================================================
## find the point on ilne closest to the given point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __closest_point_2__ ( point , line ) :
    """
    Find the point on line closest to the given point

    >>> point = ...
    >>> line  = ...
    >>> ClosestPoint  = point.closestPoint ( line )
    
    """
    return _GeomFun.closestPoint ( point , line )

__closest_point_2__ . __doc__ += '\n' + _GeomFun.closestPoint . __doc__

if not hasattr ( Gaudi.XYZPoint , 'closestPoint' ) :
    Gaudi.XYZPoint.closestPoint = __closest_point_2__


# =============================================================================
## find the parameter along the line to the closest point to the given point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __closest_point_param_1__ ( line , point ) :
    """
    Find the parameter along the line to the closest point 

    >>> line  = ...
    >>> point = ...
    >>> mu = line.closestPointParam ( point )

    """
    return _GeomFun.closestPointParam ( point , line )


__closest_point_param_1__ . __doc__ += '\n' + _GeomFun.closestPointParam .__doc__

if not hasattr ( Gaudi.XYZLine , 'closestPointParam' ) :
    Gaudi.XYZLine.closestPointParam = __closest_point_param_1__

    
# =============================================================================
## find the parameter along the line to the closest point to the given point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __closest_point_param_2__ ( point , line ) :
    """
    Find the parameter along the line to the closest point 

    >>> point = ...
    >>> line  = ...
    >>> mu = point.closestPointParam ( line )

    """
    return _GeomFun.closestPointParam ( point , line )

__closest_point_param_2__ . __doc__ += '\n' + _GeomFun.closestPointParam .__doc__

if not hasattr ( Gaudi.XYZPoint , 'closestPointParam' ) :
    Gaudi.XYZPoint.closestPointParam = __closest_point_param_2__


# =============================================================================
## check if two lines are parallel
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __parallel_lines__ ( line , line1 ) :
    """
    Check if two lines are parallel:

    >>> line  = ...
    >>> line1 = ...
    >>> par   = line.parallel ( line1 ) 
    """
    _flag = _GeomFun.parallel ( line , line1 )
    if not _flag : return False
    return True

__parallel_lines__ . __doc__ += '\n' + _GeomFun.parallel . __doc__

if not hasattr ( Gaudi.XYZLine , 'parallel' ) :
    Gaudi.XYZLine.parallel = __parallel_lines__
    

# =============================================================================
if '__main__' == __name__ :

    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  

    _v = [ l for l in dir(Gaudi     ) if 0 != l.find('__') ]
    print ' dir(Gaudi)      : '
    _v.sort() 
    for v in _v : print v
    _v = [ l for l in dir(Gaudi.Math) if 0 != l.find('__') ]
    print ' dir(Gaudi.Math) : '
    _v.sort() 
    for v in _v : print v
         
# =============================================================================
# The  END 
# =============================================================================
