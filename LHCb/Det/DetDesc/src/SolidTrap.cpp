#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "DetDesc/SolidTrd.h" 
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 
#include "DetDesc/SolidTrap.h" 

///
///
/// constructor 
SolidTrap::SolidTrap( const std::string&  Name             , /* name of this solid */ 
		      const double        ZHalfLength      , /* half length along z-axes for given solid */
		      const double        Theta            , /* polar angle of "axe" of this trapezoid   */  
		      const double        Phi              , /* asimuthal angle of the "axe" of this trapezoid */   
		      const double        DyAtMinusZ       ,  
		      const double        DxAtMinusZMinusY , 
		      const double        DxAtMinusZPlusY  ,
		      const double        AlphaAtMinusZ    ,
		      const double        DyAtPlusZ        ,  
		      const double        DxAtPlusZMinusY  , 
		      const double        DxAtPlusZPlusY   ,
		      const double        AlphaAtPlusZ    )
  ///
  : m_trap_name             ( Name             ) 
  ///
  , m_trap_zHalfLength      ( ZHalfLength      ) 
  ///
  , m_trap_theta            ( Theta            ) 
  , m_trap_phi              ( Phi              ) 
  ///
  , m_trap_dyAtMinusZ       ( DyAtMinusZ       ) 
  , m_trap_dxAtMinusZMinusY ( DxAtMinusZMinusY ) 
  , m_trap_dxAtMinusZPlusY  ( DxAtMinusZPlusY  ) 
  , m_trap_alphaAtMinusZ    ( AlphaAtMinusZ    ) 
  ///
  , m_trap_dyAtPlusZ        ( DyAtPlusZ        ) 
  , m_trap_dxAtPlusZMinusY  ( DxAtPlusZMinusY  ) 
  , m_trap_dxAtPlusZPlusY   ( DxAtPlusZPlusY   ) 
  , m_trap_alphaAtPlusZ     ( AlphaAtPlusZ     ) 
  ///
  , m_trap_cover            (  0               ) 
  ///
  , m_trap_faces            (                  )
  ///
  , m_trap_vertices         (                  )
  ///
{
  ///
  if( 0 >= ZHalfLength      ) { throw SolidException("SolidTrap constructor::ZHalfLength      is not positive!"   ); } 
  ///
  if( 0 >= DyAtMinusZ       ) { throw SolidException("SolidTrap constructor::dyAtMinusZ       is not positive!"   ); } 
  ///
  if( 0 >= DxAtMinusZMinusY ) { throw SolidException("SolidTrap constructor::dxAtMinusZMinusY is not positive!"   ); } 
  ///
  if( 0 >= DxAtMinusZPlusY  ) { throw SolidException("SolidTrap constructor::dxAtMinusZPlusY  is not positive!"   ); } 
  ///
  if( 0 >= DyAtPlusZ        ) { throw SolidException("SolidTrap constructor::dyAtMinusZ       is not positive!"   ); } 
  ///
  if( 0 >= DxAtPlusZMinusY  ) { throw SolidException("SolidTrap constructor::dxAtMinusZMinusY is not positive!"   ); } 
  ///
  if( 0 >= DxAtPlusZPlusY   ) { throw SolidException("SolidTrap constructor::dxAtMinusZPlusY  is not positive!"   ); } 
  ///
  
  { 
    ///
    /// construct points (vertoces)
    /// codes are copied from G4 
    ///
    double fDz         = zHalfLength(); 
    double fTthetaCphi = tan( theta() ) * cos( phi() ); 
    double fTthetaSphi = tan( theta() ) * sin( phi() ); 
    double fDy1        = dyAtMinusZ()           ; 
    double fDx1        = dxAtMinusZMinusY()     ;
    double fDx2        = dxAtMinusZPlusY ()     ; 
    double fTalpha1    = tan( alphaAtMinusZ() ) ; 
    double fDy2        = dyAtPlusZ()            ; 
    double fDx3        = dxAtPlusZMinusY()      ;
    double fDx4        = dxAtPlusZPlusY ()      ; 
    double fTalpha2    = tan( alphaAtPlusZ() )  ; 
    ///
    HepPoint3D p0(-fDz*fTthetaCphi-fDy1*fTalpha1-fDx1,-fDz*fTthetaSphi-fDy1,-fDz);
    HepPoint3D p1(-fDz*fTthetaCphi-fDy1*fTalpha1+fDx1,-fDz*fTthetaSphi-fDy1,-fDz);
    HepPoint3D p2(-fDz*fTthetaCphi+fDy1*fTalpha1-fDx2,-fDz*fTthetaSphi+fDy1,-fDz);
    HepPoint3D p3(-fDz*fTthetaCphi+fDy1*fTalpha1+fDx2,-fDz*fTthetaSphi+fDy1,-fDz);
    HepPoint3D p4(+fDz*fTthetaCphi-fDy2*fTalpha2-fDx3,+fDz*fTthetaSphi-fDy2,+fDz);
    HepPoint3D p5(+fDz*fTthetaCphi-fDy2*fTalpha2+fDx3,+fDz*fTthetaSphi-fDy2,+fDz);
    HepPoint3D p6(+fDz*fTthetaCphi+fDy2*fTalpha2-fDx4,+fDz*fTthetaSphi+fDy2,+fDz);
    HepPoint3D p7(+fDz*fTthetaCphi+fDy2*fTalpha2+fDx4,+fDz*fTthetaSphi+fDy2,+fDz);
    ///
    m_trap_vertices.push_back( p0 ) ; 
    m_trap_vertices.push_back( p1 ) ; 
    m_trap_vertices.push_back( p2 ) ; 
    m_trap_vertices.push_back( p3 ) ; 
    m_trap_vertices.push_back( p4 ) ; 
    m_trap_vertices.push_back( p5 ) ; 
    m_trap_vertices.push_back( p6 ) ; 
    ///
  }
  if( 8 != m_trap_vertices.size() ) 
    { throw SolidException("SolidTrap constructor:: wrong dimension of array of vertices!"); } 
  
  ///
  /// make faces
  ///
  addFace( point(0) , point(4) , point(5) , point(1) ) ;
  addFace( point(2) , point(3) , point(7) , point(6) ) ;
  addFace( point(0) , point(2) , point(6) , point(4) ) ;
  addFace( point(1) , point(5) , point(7) , point(3) ) ;
  addFace( point(0) , point(1) , point(3) , point(2) ) ; /// bottom face
  addFace( point(5) , point(4) , point(6) , point(7) ) ; /// top    face
  ///
  if( 6 != m_trap_faces.size() ) { throw SolidException("SolidTrap constructor::wrong number of constructed faces"); } 
  ///
}; 

///
///
///

SolidTrap::~SolidTrap()
{
  ///
  if( 0 != m_trap_cover ) { delete m_trap_cover ; m_trap_cover = 0 ; }  
  ///
};


///
///
/// cover for Trap is Trd 
const ISolid*           SolidTrap::cover         () const 
{
  ///
  if( 0 != m_trap_cover ) { return m_trap_cover; }              // cover is calculated already 
  ///

  ///
  /// go to minus Z plane 
  ///
  double ymn1 = point(0).y(); 
  double ymx1 = point(0).y(); 
  double xmn1 = point(0).x(); 
  double xmx1 = point(0).x();
  ///  
  for( VERTS::size_type i = 1 ; i <  4 ; ++i )
    {
      if( point(i).y() < ymn1 ) { ymn1 = point(i).y(); } 
      if( point(i).y() > ymx1 ) { ymx1 = point(i).y(); }  
      if( point(i).x() < xmn1 ) { ymn1 = point(i).x(); } 
      if( point(i).x() > xmx1 ) { ymx1 = point(i).x(); }  
    }  
  
  if( abs(ymn1) > ymx1 ) { ymx1 = abs( ymn1 ); } 
  if( abs(xmn1) > xmx1 ) { xmx1 = abs( xmn1 ); } 
  
  ///
  /// go to plus Z plane 
  ///
  double ymn2 = point(4).y(); 
  double ymx2 = point(4).y(); 
  double xmn2 = point(4).x(); 
  double xmx2 = point(4).x();
  
  for( i = 5 ; i < 8 ; ++i )
    {
      if( point(i).y() < ymn2 ) { ymn2 = point(i).y(); } 
      if( point(i).y() > ymx2 ) { ymx2 = point(i).y(); }  
      if( point(i).x() < xmn2 ) { ymn2 = point(i).x(); } 
      if( point(i).x() > xmx2 ) { ymx2 = point(i).x(); }  
    }
  
  if( abs(ymn2) > ymx2 ) { ymx1 = abs( ymn2 ); } 
  if( abs(xmn2) > xmx2 ) { xmx1 = abs( xmn2 ); } 
  
  ISolid* cov = new SolidTrd( "Cover for " + name  () , 
			      zHalfLength          () ,
                              xmx1         , ymx1     , 
                              xmx2         , ymx2     );
  ///
  m_trap_cover = cov; 
  ///
  return m_trap_cover;
};


///
///
///

void SolidTrap::addFace( const HepPoint3D& p1 , 
			 const HepPoint3D& p2 , 
			 const HepPoint3D& p3 , 
			 const HepPoint3D& p4 ) const 
{
  ///
  /// calculate center of gravity = center of the face 
  ///  
  HepVector3D c( ( p1 + p2 + p3 + p4 ) * 0.25 ) ; 
  /// 
  HepVector3D p01( p1 - c );
  HepVector3D p02( p2 - c );
  HepVector3D p03( p3 - c );
  HepVector3D p04( p4 - c );
  ///
  if( 0 != p01.cross(p02).dot(p03) ) { throw SolidException("SolidTrap::addFace - face is not planar!" ) ; } 
  if( 0 != p01.cross(p02).dot(p04) ) { throw SolidException("SolidTrap::addFace - face is not planar!" ) ; } 
  if( 0 != p01.cross(p04).dot(p03) ) { throw SolidException("SolidTrap::addFace - face is not planar!" ) ; } 
  if( 0 != p04.cross(p02).dot(p03) ) { throw SolidException("SolidTrap::addFace - face is not planar!" ) ; } 
  ///
  HepVector3D u;
  HepVector3D v;
  ///
  if      ( p01 != p02 && 0 != p01.mag2() && 0 != p02.mag2() ) { u = p01.unit() ; v = p02.unit() ; } 
  else if ( p01 != p03 && 0 != p01.mag2() && 0 != p03.mag2() ) { u = p01.unit() ; v = p03.unit() ; } 
  else if ( p01 != p04 && 0 != p01.mag2() && 0 != p04.mag2() ) { u = p01.unit() ; v = p04.unit() ; } 
  else if ( p02 != p03 && 0 != p02.mag2() && 0 != p03.mag2() ) { u = p02.unit() ; v = p03.unit() ; } 
  else if ( p02 != p04 && 0 != p02.mag2() && 0 != p04.mag2() ) { u = p02.unit() ; v = p04.unit() ; } 
  else if ( p03 != p04 && 0 != p03.mag2() && 0 != p04.mag2() ) { u = p03.unit() ; v = p04.unit() ; } 
  else          { throw SolidException("SolidTrap::addFace could not find 3 different points!" ) ; } 
  ///
  if(  u == v ) 
    { throw SolidException("SolidTrap::addFace could not find 3 different points!" ) ; } 
  if(  0 == u.cross(v).mag2() )
    { throw SolidException("SolidTrap::addFace could not find 3 different points!" ) ; } 
  ///
  HepVector3D n( u.cross(v).unit() ); 
  /// 
  v = n.cross( u ).unit(); 
  /// 
  u = v.cross( n ).unit(); 
  ///
  /// here (u,v,n) - orthogonal basis 
  ///
  { 
    /// check the point 0 , 0 , 0 
    /// and if the point is outside - inverse normal direction 
    ///
    HepPoint3D tc( 0 , 0 , 0 ) ;
    ///
    FACE tmp; 
    tmp.push_back( c ) ; 
    tmp.push_back( u ) ; 
    tmp.push_back( v ) ; 
    tmp.push_back( n ) ; 
    /// 
    if( !inside( tc , tmp ) ) { v *= -1 ; n *= -1 ; } 
    tmp.clear();
  }
  ///
  /// 
  ///
  FACE face; 
  ///
  face.push_back( c ); 
  face.push_back( u ); 
  face.push_back( v ); 
  face.push_back( n ); 
  ///
  if( 4 != face.size() )   { throw SolidException("SolidTrap::addFace::wrong argument!");}
  ///
  m_trap_faces.push_back( face ) ; 
  ///
}

///
///
///

inline  unsigned int SolidTrap::intersectionTicks ( const HepPoint3D&  Point  ,          // initial point for teh line 
						    const HepVector3D& Vector ,          // vector along the line 
						    ISolid::Ticks   &  ticks  ) const   // output container of "Ticks"
{
  ///
  ticks.clear(); 
  /// 
  if( 0 == Vector.mag2() ) { return 0; } 
  ///
  for( FACES::const_iterator pface = m_trap_faces.begin() ; 
       m_trap_faces.end() != pface ; ++pface )  
    {
      const FACE& face = *pface; 
      const HepVector3D& c =  face[0]; 
      const HepVector3D& n =  face[3]; 
      double vn = Vector*n ; 
      if(  0 == vn ) { continue ; } 
      ISolid::Tick tick = ( c*n - Point*n ) / vn; 
      ticks.push_back( tick );   
    };  
  ///
  return SolidTicks::RemoveAdjancentTicks( ticks , Point , Vector , *this );  
  ///
}; 

/// calculate the intersection points("ticks") with a given line. 
/// Input - line, paramterised by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   
inline  unsigned int SolidTrap::intersectionTicks ( const HepPoint3D  & Point   ,          // initial point for teh line 
						    const HepVector3D & Vector  ,          // vector along the line 
						    const ISolid::Tick& tickMin ,          // minimal value for the Tick 
						    const ISolid::Tick& tickMax ,          // maximal value for the tick 
						    ISolid::Ticks     & ticks   ) const    // output container of "Ticks"
{
  ///
  ticks.clear() ; 
  ///
  for( FACES::const_iterator pface = m_trap_faces.begin() ; 
       m_trap_faces.end() != pface ; ++pface )  
    {
      const FACE& face = *pface; 
      const HepVector3D& c =  face[0]; 
      const HepVector3D& n =  face[3]; 
      double vn = Vector*n ; 
      if(  0 == vn ) { continue ; } 
      ISolid::Tick tick = ( c*n - Point*n ) / vn; 
      ticks.push_back( tick ); 
    };  
  ///
  return SolidTicks::RemoveAdjancentTicks( ticks , Point , Vector , tickMin , tickMax , *this );  
  ///
};

///
///
///














