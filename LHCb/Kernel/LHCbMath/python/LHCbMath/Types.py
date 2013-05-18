#!/usr/bin/env python
# =============================================================================
# $Id$
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
#
#  Last modification $Date$
#                 by $Author$
#
#
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
  >>> p3 = Gaudi.XYZPoint(0,1,2)                  ## use C++ type Gaudi::XYZPoint

  >>> dir( Gaudi.Math )
  >>> dir( Gaudi      )
  
  Last modification $Date$
                 by $Author$

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__    = "2009-09-12"
__version__ = "Version$Revision$"
# =============================================================================
__all__     = () ## nothing to be imported !
# =============================================================================
import ROOT

## get global C++ namespace 
from GaudiPython.Bindings import gbl as cpp

## C++ namespace Gaudi
Gaudi = cpp.Gaudi
std   = cpp.std

## ROOT::Math namespace 
_RM = ROOT.ROOT.Math


Gaudi.XYZPoint            = _RM.PositionVector3D     ('ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag')
Gaudi.XYZVector           = _RM.DisplacementVector3D ('ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag')
Gaudi.LorentzVector       = _RM.LorentzVector        ('ROOT::Math::PxPyPzE4D<double>')
Gaudi.Plane3D             = _RM.Plane3D

Gaudi.Math.XYZPoint       = Gaudi.XYZPoint
Gaudi.Math.XYZVector      = Gaudi.XYZVector
Gaudi.Math.LorentzVector  = Gaudi.LorentzVector    
Gaudi.Math.Plane3D        = Gaudi.Plane3D  

Gaudi.Point3D             = Gaudi.XYZPoint
Gaudi.Math.Point3D        = Gaudi.XYZPoint

Gaudi.Vector3D            = Gaudi.XYZVector
Gaudi.Math.Vector3D       = Gaudi.XYZVector

# =============================================================================
## try to pickup the vector
@staticmethod 
def _vector_ ( i , typ = 'double' ) :
    """
    Pick up the vector of correspoding size
    
    >>>  V3   = Gaudi.Math.Vector(3)
    >>>  vct  = V3 ()
    
    """
    return _RM.SVector ( typ , i )
        
# =============================================================================
## try to pickup the matrix 
@staticmethod 
def _matrix_ ( i , j , typ = 'double' ) :
    """
    Pick up the matrix of correspoding size
    
    >>>  M3x4   = Gaudi.Math.Matrix(3,4)
    >>>  matrix = M3x4 ()
    
    """
    return _RM.SMatrix ( "%s,%d,%d" % ( typ , i , j ) )

# =============================================================================
## try to pickup the symmeric matrix 
@staticmethod 
def _sym_matrix_ ( i , typ = 'double' ) :
    """
    Pick up the symmetric matrix of correspoding size
    
    >>>  SymM3  = Gaudi.Math.SymMatrix(3)
    >>>  matrix = SymM3 ()
    
    """
    return _RM.SMatrix('%s,%d,%d,ROOT::Math::MatRepSym<%s,%d>' %  ( typ , i , i , typ , i ) )

Gaudi.Vector         =     _vector_
Gaudi.Math.Vector    =     _vector_
Gaudi.Matrix         =     _matrix_
Gaudi.Math.Matrix    =     _matrix_
Gaudi.SymMatrix      = _sym_matrix_
Gaudi.Math.SymMatrix = _sym_matrix_

Gaudi.Vector2             = Gaudi.Vector(2)
Gaudi.Vector3             = Gaudi.Vector(3)
Gaudi.Vector4             = Gaudi.Vector(4)
Gaudi.Vector5             = Gaudi.Vector(5)
Gaudi.Vector6             = Gaudi.Vector(6)
Gaudi.Vector8             = Gaudi.Vector(8)

Gaudi.Math.Vector2        = Gaudi.Vector2 
Gaudi.Math.Vector3        = Gaudi.Vector3 
Gaudi.Math.Vector4        = Gaudi.Vector4 
Gaudi.Math.Vector5        = Gaudi.Vector5 
Gaudi.Math.Vector8        = Gaudi.Vector8 


Gaudi.SymMatrix2x2        = Gaudi.SymMatrix(2)
Gaudi.SymMatrix3x3        = Gaudi.SymMatrix(3)    
Gaudi.SymMatrix4x4        = Gaudi.SymMatrix(4)    
Gaudi.SymMatrix5x5        = Gaudi.SymMatrix(5)    
Gaudi.SymMatrix6x6        = Gaudi.SymMatrix(6)    
## LHCb::Particle
Gaudi.SymMatrix7x7        = Gaudi.SymMatrix(7)
## Gaudi::Math::ParticleParams 
Gaudi.SymMatrix8x8        = Gaudi.SymMatrix(8)
## LHCb:TwoProngVertex
Gaudi.SymMatrix9x9        = Gaudi.SymMatrix(9)


Gaudi.Math.SymMatrix2x2   = Gaudi.SymMatrix2x2 
Gaudi.Math.SymMatrix3x3   = Gaudi.SymMatrix3x3 
Gaudi.Math.SymMatrix4x4   = Gaudi.SymMatrix4x4
Gaudi.Math.SymMatrix5x5   = Gaudi.SymMatrix5x5 
Gaudi.Math.SymMatrix6x6   = Gaudi.SymMatrix6x6 
Gaudi.Math.SymMatrix7x7   = Gaudi.SymMatrix7x7 
Gaudi.Math.SymMatrix8x8   = Gaudi.SymMatrix8x8 
Gaudi.Math.SymMatrix9x9   = Gaudi.SymMatrix9x9

#
# specific matrices for 'tracks'
#

Gaudi.Matrix5x5             = Gaudi.Matrix(5,5)
Gaudi.TrackMatrix           = Gaudi.Matrix5x5 
Gaudi.Math.Matrix5x5        = Gaudi.Matrix5x5 
Gaudi.Math.TrackMatrix      = Gaudi.TrackMatrix 

Gaudi.TrackSymMatrix        = Gaudi.SymMatrix5x5
Gaudi.Math.TrackSymMatrix   = Gaudi.TrackSymMatrix 

Gaudi.TrackVector           = Gaudi.Vector5 
Gaudi.Math.TrackVector      = Gaudi.TrackVector

#
# matrix from LHCb::Particle
#
Gaudi.Matrix4x3             = Gaudi.Matrix(4,3)
Gaudi.Math.Matrix4x3        = Gaudi.Matrix4x3
    
## Gaudi::Math
Gaudi.Math.XYZLine           = cpp.Gaudi.Math.Line(Gaudi.XYZPoint,Gaudi.XYZVector)
Gaudi.Math.SVector2WithError = cpp.Gaudi.Math.SVectorWithError(2,'double')
Gaudi.Math.SVector3WithError = cpp.Gaudi.Math.SVectorWithError(3,'double')
Gaudi.Math.SVector4WithError = cpp.Gaudi.Math.SVectorWithError(4,'double')
Gaudi.Math.SVector5WithError = cpp.Gaudi.Math.SVectorWithError(5,'double')
Gaudi.Math.SVector6WithError = cpp.Gaudi.Math.SVectorWithError(6,'double')
Gaudi.Math.SVector8WithError = cpp.Gaudi.Math.SVectorWithError(8,'double')

Gaudi.XYZLine                = Gaudi.Math.XYZLine
Gaudi.Line3D                 = Gaudi.Math.XYZLine
Gaudi.Math.Line3D            = Gaudi.Math.XYZLine


Gaudi.Math.frac              = Gaudi.Math.Functions.frac
Gaudi.Math.asym              = Gaudi.Math.Functions.asym
Gaudi.Math.binomEff          = Gaudi.Math.Functions.binomEff 

## vectors of vectors
Gaudi.Vectors2       = std.vector ( Gaudi.Vector2 )
Gaudi.Vectors3       = std.vector ( Gaudi.Vector3 )
Gaudi.Vectors4       = std.vector ( Gaudi.Vector4 )
Gaudi.Math.Vectors2  = Gaudi.Vectors2 
Gaudi.Math.Vectors3  = Gaudi.Vectors3
Gaudi.Math.Vectors4  = Gaudi.Vectors4 

Gaudi.Math.ValueWithError.Vector = std.vector(Gaudi.Math.ValueWithError)
Gaudi.Math.ValueWithError.Vector .__str__   = lambda s : str( [ i for i in s ])
Gaudi.Math.ValueWithError.Vector .__repr__  = lambda s : str( [ i for i in s ])

## Sum the contents of the vector
def _ve_sum_ ( s ) :
    """

    >>> v = ...
    >>> s = v.sum()
    
    """
    return Gaudi.Math.sum ( s )


## Sum the contents of the vector
def _ve_asum_ ( s ) :
    """

    >>> v = ...
    >>> s = v.abssum()
    
    """
    return Gaudi.Math.abssum ( s )

_ve_sum_  . __doc__ += '\n' + Gaudi.Math.sum    .__doc__
_ve_asum_ . __doc__ += '\n' + Gaudi.Math.abssum .__doc__

_VVE = std.vector( Gaudi.Math.ValueWithError )

_VVE  . sum      = _ve_sum_
_VVE  . abssum   = _ve_asum_
_VVE  . sumabs   = _ve_asum_
_VVE  . __str__  = lambda s : str( [ i for i in s ] )
_VVE  . __repr__ = lambda s : str( [ i for i in s ] )
_VVE  . __len__  = lambda s : s.size ()

_VVVE = std.vector( _VVE )
_VVVE . __str__  = lambda s : str( [ i for i in s ] )
_VVVE . __repr__ = lambda s : str( [ i for i in s ] )
_VVVE . __len__  = lambda s : s.size ()

_VVE.Vector = _VVVE 
Gaudi.Math.ValueWithError.Vector = _VVE

_C2F            = Gaudi.Math.Chi2Fit
_C2F . __str__  = lambda s : s.toString ()
_C2F . __repr__ = lambda s : s.toString ()
## chi2-probabilty 
def _c2_prob_  ( s ) :
    """
    Chi2 probabiilty

    >>> r = h.hfit ( ... )
    >>> r.Prob()
    
    """
    dofs = s.points() - s.size() 
    return ROOT.TMath.Prob ( s.chi2() , dofs )

_C2F . Prob        = _c2_prob_
_C2F . Probability = _c2_prob_
_C2F . prob        = _c2_prob_
_C2F . probability = _c2_prob_
_C2F . __len__     = lambda s     : s.size  (   )
_C2F . __getitem__ = lambda s , i : s.param ( i )  

## ============================================================================
## some useful decoration:
## ============================================================================

_P3D = Gaudi.XYZPoint
_V3D = Gaudi.XYZVector
_V4D = Gaudi.LorentzVector

def _o_mul_ ( self , other ) :
    _typ  = self.__class__
    if isinstance ( other , _typ ) :
        return self.Dot ( other ) 
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
def _o1_str_ ( self , fmt = "( %g, %g, %g) ") :
    """    
    Self-printout of 3D-points and 3D-vectors
    
    """
    return fmt % ( self.X() , self.Y( ), self.Z() )

## Self-printout of 4D-vectors 
def _o2_str_ ( self , fmt = "[( %g, %g, %g), %g]" ) :
    """    
    Self-printout of 4D-vectors 
    
    """
    return fmt % ( self.X() , self.Y( ), self.Z() , self.E() )

_V3D.__mul__ = _o_mul_
_V4D.__mul__ = _o_mul_

_P3D.__add__ = _o_add_
_V3D.__add__ = _o_add_
_V4D.__add__ = _o_add_

_P3D.__sub__ = _o_sub_
_V3D.__sub__ = _o_sub_
_V4D.__sub__ = _o_sub_
    
_V3D. __rmul__ = _V3D.__mul__ 
_V4D. __rmul__ = _V4D.__mul__

_P3D. __radd__ = _P3D.__add__
_V3D. __radd__ = _V3D.__add__

if not hasattr ( _P3D , '_new_str_' ) :
    _P3D._new_str_ = _o1_str_  
    _P3D.__str__   = _o1_str_
    _P3D.__repr__  = _o1_str_

if not hasattr ( _V3D , '_new_str_' ) :
    _V3D._new_str_ = _o1_str_
    _V3D.__str__   = _o1_str_
    _V3D.__repr__  = _o1_str_
    
if not hasattr ( _V4D , '_new_str_' ) :    
    _V4D._new_str_ = _o2_str_
    _V4D.__str__   = _o2_str_
    _V4D.__repr__  = _o2_str_

# =============================================================================
## Self-printout of line
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-09-12
def _l_str_ ( self ) :
    """
    Self-printout of line: (point, direction)
    """
    return "(%s,%s)" % ( self.beginPoint() , self.direction() )

if not hasattr ( Gaudi.Math.XYZLine , '_new_str_' ) :
    Gaudi.Math.XYZLine._new_str_ = _l_str_
    Gaudi.Math.XYZLine.__str__   = _l_str_
    Gaudi.Math.XYZLine.__repr__  = _l_str_

# ============================================================================
## self-printout of matrices
def _mg_str_ ( self , fmt = ' %+11.4g') :
    """
    Self-printout of matrices 
    """
    _rows = self.kRows 
    _cols = self.kCols
    _line = ''
    for _irow in range ( 0 , _rows ) :
        _line += ' |' 
        for _icol in range ( 0 , _cols ) :
            _line += fmt % self( _irow , _icol )
        _line += ' |'
        if ( _rows - 1 )  != _irow : _line += '\n'
    return _line
#
## self-printout of symmetrical matrices
def _ms_str_ ( self , fmt = ' %+11.4g' , width = 12 ) :
    """
    Self-printout of symetrical matrices 
    """
    _rows = self.kRows 
    _cols = self.kCols
    _line = ''
    for _irow in range ( 0 , _rows ) :
        _line += ' |' 
        for _icol in range ( 0 , _cols  ) :
            if _icol < _irow : _line += width*' ' 
            else             : _line += fmt % self( _irow , _icol )
        _line += ' |'
        if ( _rows - 1 )  != _irow : _line += '\n'
    return _line

## get the correlation matrix
def _m_corr_ ( self ) :
    """
    Get the correlation matrix
    
    >>> mtrx = ...
    >>> corr = mtrx.correlations()
    
    """
    from math import sqrt

    _t = type ( self )
    _c = _t   () 
    _rows = self.kRows
    for i in range ( 0 , _rows ) :
        _dI = sqrt ( self ( i , i ) ) 
        for j in range ( i + 1 , _rows ) :
            _dJ = sqrt ( self ( j , j ) ) 
            _c [ i , j ] = self ( i , j ) /  (  _dI * _dJ )
        _c[ i , i ] = 1.0
        
    return _c 
    
    
for m in ( Gaudi.Matrix5x5      ,
           Gaudi.TrackMatrix    ,
           Gaudi.Matrix4x3      ) :
    if not hasattr ( m , '_new_str_' ) : 
        m. _new_str_ = _mg_str_
        m. __repr__  = _mg_str_
        m. __str__   = _mg_str_
    
for m in ( Gaudi.SymMatrix2x2   ,
           Gaudi.SymMatrix3x3   ,
           Gaudi.SymMatrix4x4   ,
           Gaudi.SymMatrix5x5   ,
           Gaudi.SymMatrix6x6   ,
           Gaudi.SymMatrix7x7   ,
           Gaudi.SymMatrix8x8   ,
           Gaudi.SymMatrix9x9   ,
           Gaudi.TrackSymMatrix ) :
    
    if not hasattr ( m , 'correlations' ) :
        m.correlations = _m_corr_
        
    if not hasattr ( m , '_new_str_' ) : 
        m. _new_str_ = _ms_str_
        m. __repr__  = _ms_str_
        m. __str__   = _ms_str_
        
                      
# =============================================================================
## Self-printout of 3D-plane
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-09-12
def _p_str_ ( self ) :
    """
    Self-printout of 3D-plane: (point, normal)
    """
    return "(%s,%s)" % ( self.ProjectOntoPlane( Gaudi.XYZPoint()) , self.Normal() )

if not hasattr ( Gaudi.Plane3D , '_new_str_' ) :
    Gaudi.Plane3D._new_str_ = _p_str_
    Gaudi.Plane3D.__str__   = _p_str_
    Gaudi.Plane3D.__repr__  = _p_str_

# =============================================================================
## self-printout of S-vectors 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-09-12
def _v_str_ ( self , fmt = ' %g' ) :
    """
    Self-printout of SVectors: (...)
    """
    index  = 0
    result = ''
    while index < self.kSize :
        if 0 != index : result += ', '
        result += fmt % self.At( index )
        index  += 1 
    return "( " + result + ' )'

for t in  ( Gaudi.Vector2 ,
            Gaudi.Vector3 ,
            Gaudi.Vector4 ,
            Gaudi.Vector5 ,
            Gaudi.Vector6 ) :
            
    if not hasattr ( t , '_new_str_' ) :
        t._new_str_ = _v_str_
        t.__str__   = _v_str_
        t.__repr__  = _v_str_
        
for t in ( Gaudi.Math.ValueWithError         ,
           Gaudi.Math.Point3DWithError       ,
           Gaudi.Math.Vector3DWithError      ,
           Gaudi.Math.LorentzVectorWithError ,
           Gaudi.Math.SVector3WithError      , 
           Gaudi.Math.SVector4WithError      , 
           Gaudi.Math.SVector5WithError      , 
           Gaudi.Math.SVector6WithError      , 
           Gaudi.Math.SVector8WithError      ) :
    if not hasattr ( t , '_new_str_' ) :
        t._new_str_ = t.toString
        t.__str__   = t.toString
        t.__repr__  = t.toString

        
## get the eigenvalues for symmetric matrices :
def _eigen_1_ ( self , sorted = True ) :
    """
    
    >>> mtrx = ...
    >>> values = mtrx.eigenValues ( sorted = True )
    
    """
    return Gaudi.Math.EigenSystems.eigenValues ( self , sorted )


## get the eigevectors for symmetric matrices :
def _eigen_2_ ( self , sorted = True ) :
    """
    
    >>> mtrx = ...
    >>> values, vectors = mtrx.eigenVectors( sorted = True )
    
    """
    if   2 == self.kCols :
        _values  = Gaudi.Vector2  ()
        _vectors = Gaudi.Vectors2 ()
    elif 3 == self.kCols :
        _values  = Gaudi.Vector3  ()
        _vectors = Gaudi.Vectors3 ()
    elif 4 == self.kCols :
        _values  = Gaudi.Vector4  ()
        _vectors = Gaudi.Vectors4 ()
    else :
        raise AttributeError, "Not implemented for dimention: %s" % self.kCols
    
    st = Gaudi.Math.EigenSystems.eigenVectors ( self , _values , _vectors , sorted )
    if st.isFailure () :
        print 'EigenVectors: Failure from EigenSystems' , st
        
    return ( _values , _vectors )

    
_eigen_1_ .__doc__ += '\n' +  Gaudi.Math.EigenSystems.eigenValues  . __doc__
_eigen_2_ .__doc__ += '\n' +  Gaudi.Math.EigenSystems.eigenVectors . __doc__

for m in ( Gaudi.SymMatrix2x2   ,
           Gaudi.SymMatrix3x3   ,
           Gaudi.SymMatrix4x4   ) :
    if not hasattr ( m , 'eigenValues'  ) : m.eigenValues  = _eigen_1_
    if not hasattr ( m , 'eigenVectors' ) : m.eigenVectors = _eigen_2_
        

# =============================================================================
## self-printout of Gaudi::Math::ParticleParams  
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-26
def _pp_str_ ( self ) :
    """
    Self-printout of ParticleParams
    """
    pos = self.position    ()
    mom = self.momentum    ()
    len = self.decayLength () 
    return " ( %s/%s/%s ) " % ( pos, mom , len )

if not hasattr ( Gaudi.Math.ParticleParams , '_new_str_' ) :
    Gaudi.Math.ParticleParams._new_str_ = _pp_str_
    Gaudi.Math.ParticleParams.__str__   = _pp_str_
    Gaudi.Math.ParticleParams.__repr__  = _pp_str_

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
def _intersect_line_and_plane_ ( line , plane ) :
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
    
_intersect_line_and_plane_ . __doc__ += '\n' + _GeomFun.intersection . __doc__ 

if not hasattr ( Gaudi.XYZLine , 'intersect' ) :
    Gaudi.XYZLine.intersect = _intersect_line_and_plane_

# =============================================================================
## intersect two planes 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _intersect_two_planes_ ( plane , plane1 ) :
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

_intersect_two_planes_ . __doc__ += '\n' + _GeomFun.intersection . __doc__ 

if not hasattr ( Gaudi.Plane3D , 'line' ) :
    Gaudi.Plane3D.line = _intersect_two_planes_


# =============================================================================
## intersect three planes 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _intersect_three_planes_ ( plane , plane1 , plane2 ) :
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


_intersect_three_planes_ . __doc__ += '\n' + _GeomFun.intersection . __doc__ 

if not hasattr ( Gaudi.Plane3D , 'point' ) :
    Gaudi.Plane3D.point = _intersect_three_planes_


# =============================================================================
## intersect the planes 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _intersect_the_planes_ ( plane , plane1 , plane2 = None ) :
    """
    Find the intersection line/point for two or three planes:

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
    if not plane2 : return _intersect_two_planes_ ( plane , plane1 )
    return _intersect_three_planes_ ( plane , plane1 , plane2 )
    
_intersect_the_planes_ . __doc__ += '\n' + _GeomFun.intersection . __doc__ 

if not hasattr ( Gaudi.Plane3D , 'intersect' ) :
    Gaudi.Plane3D.intersect = _intersect_the_planes_


# =============================================================================
## calculate the impact parameter of the line & point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _imp_par_1_ ( line , point ) :
    """
    Calculate the impact parameter of the line and the point

    >>> line  = ...
    >>> point = ...
    >>> ip = line.impactParameter ( point )
    
    """
    return _GeomFun.impactParameter ( point , line )

_imp_par_1_ . __doc__ += '\n' + _GeomFun.impactParameter . __doc__ 

if not hasattr ( Gaudi.XYZLine , 'impactParameter' ) :
    Gaudi.XYZLine.impactParameter = _imp_par_1_
if not hasattr ( Gaudi.XYZLine , 'ip'              ) :
    Gaudi.XYZLine.ip              = _imp_par_1_

# =============================================================================
## calculate the impact parameter of the line & point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _imp_par_2_ ( point , line ) :
    """
    Calculate the impact parameter of the line and the point

    >>> point = ...
    >>> line  = ...
    >>> ip = point.impactParameter ( line )
    
    """
    return _GeomFun.impactParameter ( point , line )


_imp_par_2_ . __doc__ += '\n' + _GeomFun.impactParameter . __doc__ 

if not hasattr ( Gaudi.XYZPoint , 'impactParameter' ) :
    Gaudi.XYZPoint.impactParameter = _imp_par_2_
if not hasattr ( Gaudi.XYZPoint , 'ip'              ) :
    Gaudi.XYZPoint.ip              = _imp_par_2_

# =============================================================================
## distance between two lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _distance_between_two_lines_ ( line , line1 ) :
    """
    Find the distance netween two lines :

    >>> line  = ...
    >>> line1 = ...
    >>> dist = line.distance ( line1 )
    
    """
    return _GeomFun.distance ( line , line1 )

_distance_between_two_lines_ . __doc__ += '\n' + _GeomFun.distance. __doc__

if not hasattr ( Gaudi.XYZLine , 'distance' ) :
    Gaudi.XYZLine.distance = _distance_between_two_lines_


# =============================================================================
## find the closest points for two lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _closest_points_ ( line , line1 ) :
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
    

_closest_points_ . __doc__ += '\n' + _GeomFun.closestPoints . __doc__

if not hasattr ( Gaudi.XYZLine , 'closestPoints' ) :
    Gaudi.XYZLine.closestPoints = _closest_points_


# =============================================================================
## find the closest points for two lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _closest_point_params_ ( line , line1 ) :
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
    

_closest_point_params_ . __doc__ += '\n' + _GeomFun.closestPointParams . __doc__

if not hasattr ( Gaudi.XYZLine , 'closestPointParams' ) :
    Gaudi.XYZLine.closestPointParams = _closest_point_params_

# =============================================================================
## find the point on ilne closest to the given point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _closest_point_1_ ( line , point ) :
    """
    Find the point on line closest to the given point

    >>> line  = ...
    >>> point = ...
    >>> ClosestPoint  = line.closestPoint ( point )
    
    """
    return _GeomFun.closestPoint ( point , line )

_closest_point_1_ . __doc__ += '\n' + _GeomFun.closestPoint . __doc__

if not hasattr ( Gaudi.XYZLine , 'closestPoint' ) :
    Gaudi.XYZLine.closestPoint = _closest_point_1_
    
# =============================================================================
## find the point on ilne closest to the given point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _closest_point_2_ ( point , line ) :
    """
    Find the point on line closest to the given point

    >>> point = ...
    >>> line  = ...
    >>> ClosestPoint  = point.closestPoint ( line )
    
    """
    return _GeomFun.closestPoint ( point , line )

_closest_point_2_ . __doc__ += '\n' + _GeomFun.closestPoint . __doc__

if not hasattr ( Gaudi.XYZPoint , 'closestPoint' ) :
    Gaudi.XYZPoint.closestPoint = _closest_point_2_


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
def _closest_point_param_2_ ( point , line ) :
    """
    Find the parameter along the line to the closest point 

    >>> point = ...
    >>> line  = ...
    >>> mu = point.closestPointParam ( line )

    """
    return _GeomFun.closestPointParam ( point , line )

_closest_point_param_2_ . __doc__ += '\n' + _GeomFun.closestPointParam .__doc__

if not hasattr ( Gaudi.XYZPoint , 'closestPointParam' ) :
    Gaudi.XYZPoint.closestPointParam = _closest_point_param_2_


# =============================================================================
## check if two lines are parallel
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _parallel_lines_ ( line , line1 ) :
    """
    Check if two lines are parallel:

    >>> line  = ...
    >>> line1 = ...
    >>> par   = line.parallel ( line1 ) 
    """
    _flag = _GeomFun.parallel ( line , line1 )
    if not _flag : return False
    return True

_parallel_lines_ . __doc__ += '\n' + _GeomFun.parallel . __doc__

if not hasattr ( Gaudi.XYZLine , 'parallel' ) :
    Gaudi.XYZLine.parallel = _parallel_lines_

# =============================================================================
## helper function/wrapper for Gaudi::Math:FitMass
#  @see Gaudi::Math::FitMass 
#  @see Gaudi::Math::FitMass::fit 
#  @see Gaudi::Math::ParticleParams
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @param  particle (INPUT) "particle"
#  @param  mass     (INPUT) the nominal mass 
#  @return the tuple (fitter,chi2) 
#  @date 2009-10-22
def _fit_mass_ ( particle , mass )  :
    """
    Helper function/wrapper for Gaudi::Math::MassFit

    >>> particle  =  ...   ## get Gaudi::Math::ParticleParams object
    >>> mass = 5.279 * GeV ## get the mass
    >>> result, chi2 = particle.fitMass ( particle , mass )
    >>> print 'result/chi2:', result, chi2
    
    """
    _fitted = Gaudi.Math.ParticleParams()
    _chi2   = Gaudi.Math.FitMass.fit ( particle  ,
                                       mass      ,
                                       _fitted   )
    return (_fitted,_chi2)

Gaudi.Math.ParticleParams.fitMass = _fit_mass_


# =============================================================================
## decorate some basic vectors 
for t in ( 'int'                ,
           ##'long'               ,
           ##'long long'          ,
           ##'unsigned int'       ,
           ##'unsigned long'      , 
           ##'unsigned long long' ,
           ##'float'
           'double'               ) :
    v = std.vector( t )
    v.asList   = lambda s :       [ i for i in s ]   ## convert vector into list
    v.toList   = v.asList
    v.__repr__ = lambda s : str ( [ i for i in s ] ) ## print it !
    v.__str__  = lambda s : str ( [ i for i in s ] ) ## print it !



LHCb  = cpp.LHCb
Gaudi.Math.round                 = LHCb.Math.round 
Gaudi.Math.equal_to_int          = LHCb.Math.equal_to_int 
Gaudi.Math.equal_to_uint         = LHCb.Math.equal_to_uint 
Gaudi.Math.isint                 = LHCb.Math.isint
Gaudi.Math.islong                = LHCb.Math.islong
Gaudi.Math.knuth_equal_to_double = LHCb.Math.knuth_equal_to_double
__lomont = Gaudi.Math.lomont_compare_double
Gaudi.Math.lomont_compare_double = __lomont

if not hasattr ( Gaudi.Math.Splines , 'DATA'    ) or \
   not hasattr ( Gaudi.Math.Splines , 'PAIR'    ) or \
   not hasattr ( Gaudi.Math.Splines , 'DATAERR' ) : 
    
    _pair_ = std.pair('double','double')
    Gaudi.Math.Splines.DATA    = std.vector ( _pair_ ) 
    Gaudi.Math.Splines.PAIR    = std.pair   ( 'double',Gaudi.Math.ValueWithError )
    Gaudi.Math.Splines.DATAERR = std.vector ( Gaudi.Math.Splines.PAIR ) 

# =============================================================================
## make some simple interpolation for set of data points
#  @see Gaudi::Math::Splines::interpolate
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2013-03-17
def _v_interp_ ( self , x ) :
    """
    Simple interpolation for set of data points.
    The vector of pairs is interpreted as vector of (x,y) pairs

    >>> vct_of_pairs = ...

    >>> result = vct_of_pairs.interpolate ( x )
    
    """
    if hasattr ( x , 'value' ) : x = x.value()
    #
    ## make the interpolation
    #
    return Gaudi.Math.Splines.interpolate ( self , x )

_v_interp_ . __doc__  += '\n' + Gaudi.Math.Splines.interpolate. __doc__

Gaudi.Math.Splines.DATA    .interpolate = _v_interp_
Gaudi.Math.Splines.DATAERR .interpolate = _v_interp_

# =============================================================================
## build a spline to approximate the data poinst
#  @see Gaudi::Math::Spline
#  @see GaudiMath::Spline
#  @see GaudiMath::SplineBase 
#  @see Genfun::GaudiMathImplementation::SplineBase 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2013-03-17
def _v_spline_ ( self                                      ,
                 type  = cpp.GaudiMath.Interpolation.Akima ,
                 null  = False                             ,
                 scale = 1                                 ,
                 shift = 0                                 ) :
    """
    Create spline object for the data vector 

    >>> vdata  = ...
    >>> spline = vdata.spline ()

    >>> value = spline ( 10 ) 
    """
    return cpp.Gaudi.Math.Spline ( self , type , null , scale , shift )

# =============================================================================
## build a spline to approximate the data poinst
#  @see Gaudi::Math::SplineErrors
#  @see GaudiMath::Spline
#  @see GaudiMath::SplineBase 
#  @see Genfun::GaudiMathImplementation::SplineBase 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2013-03-17
def _v_spline_err_ ( self                                      ,
                     type  = cpp.GaudiMath.Interpolation.Akima ,
                     null  = False                             ,
                     scale = 1                                 ,
                     shift = 0                                 ) :
    """
    Create spline object for the data vector 

    >>> vdata  = ...
    >>> spline = vdata.splineErr()

    >>> value = spline ( 10 ) 
    """
    return cpp.Gaudi.Math.SplineErrors ( self , type , null , scale , shift )

_v_spline_     . __doc__ += '\n' + cpp.Gaudi.Math.Spline       .__init__ .__doc__
_v_spline_err_ . __doc__ += '\n' + cpp.Gaudi.Math.SplineErrors .__init__ .__doc__

Gaudi.Math.Splines.DATA    .spline     = _v_spline_
Gaudi.Math.Splines.DATAERR .spline     = _v_spline_
Gaudi.Math.Splines.DATAERR .splineErr  = _v_spline_err_

# =============================================================================
## get values from the DATAERR 
def _v_get_values_ ( self ) :
    """
    Get values from vector

    >>> vdataerr = ...
    >>> vvalues   = vdataerr.getValues()
    
    """
    return Gaudi.Math.Spline.getValues ( self )


## get values from the DATAERR 
def _v_get_errors_ ( self ) :
    """
    Get errors from vector
    
    >>> vdataerr = ...
    >>> verrors   = vdataerr.getErrors()
    
    """
    return Gaudi.Math.Spline.getErrors ( self )

_v_get_values_ . __doc__  += '\n' + Gaudi.Math.Splines.getValues. __doc__
_v_get_errors_ . __doc__  += '\n' + Gaudi.Math.Splines.getErrors. __doc__

Gaudi.Math.Splines.DATAERR .getValues = _v_get_values_
Gaudi.Math.Splines.DATAERR .getErrors = _v_get_errors_

Gaudi.Math.Splines.DATA    . __repr__ = lambda s : str ( [ ( i.first , i.second ) for i in s ] )
Gaudi.Math.Splines.DATAERR . __repr__ = lambda s : str ( [ ( i.first , i.second ) for i in s ] )
Gaudi.Math.Splines.DATA    . __str__  = lambda s : str ( [ ( i.first , i.second ) for i in s ] )
Gaudi.Math.Splines.DATAERR . __str__  = lambda s : str ( [ ( i.first , i.second ) for i in s ] )

if not hasattr ( Gaudi.Math.Spline       , 'DATA'    ) :
    Gaudi.Math.Spline.DATA          = Gaudi.Math.Splines.DATA
if not hasattr ( Gaudi.Math.Spline       , 'DATAERR' ) :
    Gaudi.Math.Spline.DATAERR       = Gaudi.Math.Splines.DATAERR
if not hasattr ( Gaudi.Math.SplineErrors , 'DATAERR' ) :
    Gaudi.Math.SplineErrors.DATAERR = Gaudi.Math.Splines.DATAERR
    
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
