
#include "DetDesc/SolidTrd.h" 
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 

//
//
//

SolidTrd::SolidTrd( const std::string& name              ,
		    const double       ZHalfLength       ,
		    const double       XHalfLength1      , 
		    const double       YHalfLength1      , 
		    const double       XHalfLength2      , 
		    const double       YHalfLength2      ) 
  : m_trd_name         ( name         )
  , m_trd_zHalfLength  ( ZHalfLength  )
  , m_trd_xHalfLength1 ( XHalfLength1 )
  , m_trd_xHalfLength2 ( XHalfLength2 )
  , m_trd_yHalfLength1 ( YHalfLength1 )
  , m_trd_yHalfLength2 ( YHalfLength2 )
  //
  , m_trd_cover        (      0       )
  //
{
  //
  if( 0 >= ZHalfLength  )
    { throw SolidException("SolidTrd constructor::ZHalfLength is not positive!"); } 
  if( 0 >  XHalfLength1 )
    { throw SolidException("SolidTrd constructor::XHalfLength1 is negative!"); } 
  if( 0 >  XHalfLength2 )
    { throw SolidException("SolidTrd constructor::XHalfLength2 is negative!"); } 
  if( 0 >  YHalfLength1 )
    { throw SolidException("SolidTrd constructor::YHalfLength1 is negative!"); } 
  if( 0 >  YHalfLength2 )
    { throw SolidException("SolidTrd constructor::YHalfLengt2 is negative!"); } 
  if( 0 >=  XHalfLength1 + XHalfLength2 )
    { throw SolidException("SolidTrd constructor::XSUM is not positive "); } 
  if( 0 >=  YHalfLength1 + YHalfLength2 )
    { throw SolidException("SolidTrd constructor::YSUM is not positive "); } 
  //
};

//
//
//

SolidTrd::~SolidTrd()
{
  if( this == m_trd_cover ){                   m_trd_cover = 0 ; } 
  if(    0 != m_trd_cover ){ delete m_trd_cover; m_trd_cover = 0 ; }
};

//
//
// cover for Trd is "symmetric trd", cover for "symmetric trd" is Box 
const ISolid*           SolidTrd::cover         () const 
{
  if( 0 != m_trd_cover ) { return m_trd_cover; }              // cover is calculated already 
  //
  ISolid* cov = 0 ;
  if(  ( xHalfLength1() != yHalfLength1() )  || ( xHalfLength2() != yHalfLength2() ) ) 
    { cov = new SolidTrd("Cover for " + name() , zHalfLength() , 
				       xHalfLength1() > yHalfLength1() ? xHalfLength1() : yHalfLength1() , 
				       xHalfLength1() > yHalfLength1() ? xHalfLength1() : yHalfLength1() , 
				       xHalfLength2() > yHalfLength2() ? xHalfLength2() : yHalfLength2() , 
				       xHalfLength2() > yHalfLength2() ? xHalfLength2() : yHalfLength2() ) ; }
  else
    { cov = new SolidBox("Cover for " + name() , 
				       xHalfLength1() > xHalfLength2() ? xHalfLength1() : xHalfLength2() , 
				       yHalfLength1() > yHalfLength2() ? yHalfLength1() : yHalfLength2() , 
				       zHalfLength      ()                      ); }
  //
  if( 0 == cov ) { return this ; } 
  //
  m_trd_cover = cov; 
  //
  return m_trd_cover;
};



/// calculate the intersection points("ticks") with a given line. 
/// Input - line, paramterised by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   
inline  unsigned int SolidTrd::intersectionTicks ( const HepPoint3D & point  ,          // initial point for teh line 
						   const HepVector3D& vect   ,          // vector along the line 
						   ISolid::Ticks    & ticks  ) const    // output container of "Ticks"
{
  // clear container 
  ticks.clear();
  
  // line with null direction vection is not able to intersect any solid!
  if(  vect.mag2() <= 0 ) { return 0; }                     // RETURN!!!
  
  // try to intersect with z-planes
  SolidTicks::LineIntersectsTheZ( point , vect , -1.0 * zHalfLength() , std::back_inserter( ticks ) ) ;
  SolidTicks::LineIntersectsTheZ( point , vect ,        zHalfLength() , std::back_inserter( ticks ) ) ;
  
  // try to intersect the line  with other planes
  
  {
    double dx   = xHalfLength2()-xHalfLength1();
    double dxdz = 0.5 * dx / zHalfLength() ; 
    {  
      // x-z plane 
      // 1) line x=x1+(z-z1)*(x2-x1)/(z2-z1) = x1 + (z-z1)*dxdz =dxdz*z + x1-z1*dxdz = a*z+b
      //         z=z 
      //    line x = px + vx * t 
      //         z = pz + vz * t 
      // ------------------------------------
      //    a*z + b = px + vx * t 
      //      z     = pz + vz * t 
      // -----------------------------------
      //    t*vx - a*z = b - px 
      //    t*vz -   z =   - pz
      double a = dxdz;
      double b = xHalfLength1()+zHalfLength()*dxdz;
      double d = -1.0 * vect.x() + a * vect.z() ;
      if( 0 != d ) { ticks.push_back( (ISolid::Tick) ( -1.0*(b-point.x())-point.z()*a ) / d ); }
    }
    {
      // 2) line x=-x1-(z-z1)*(x2-x1)/(z2-z1) =-x1-(z-z1)*dxdz =-dxdz*z -x1+z1*dxdz = a*z+b
      //         z=z 
      //    line x = px + vx * t 
      //         z = pz + vz * t 
      // ------------------------------------
      //    a*z + b = px + vx * t 
      //      z     = pz + vz * t 
      // -----------------------------------
      //    t*vx - a*z = b - px 
      //    t*vz -   z =   - pz
      double a = -dxdz;
      double b = -xHalfLength1()-zHalfLength()*dxdz;
      double d = -1.0 * vect.x() + a * vect.z() ;
      if( 0 != d ) { ticks.push_back( (ISolid::Tick) ( -1.0*(b-point.x())-point.z()*a ) / d ); }
    }
  }
  {
    double dy   = yHalfLength2()-yHalfLength1();
    double dydz = 0.5 * dy / zHalfLength() ; 
    {
      // y-z plane 
      // 1) line y=y1+(z-z1)*(y2-y1)/(z2-z1) = y1 + (z-z1)*dydz =dydz*z + y1-z1*dydz = a*z+b
      //         z=z 
      //    line y = py + vy * t 
      //         z = pz + vz * t 
      // ------------------------------------
      //    a*z + b = py + vy * t 
      //      z     = pz + vz * t 
      // -----------------------------------
      //    t*vy - a*z = b - py 
      //    t*vz -   z =   - pz
      double a = dydz;
      double b = yHalfLength1()+zHalfLength()*dydz;
      double d = -1.0 * vect.y() + a * vect.z() ;
      if( 0 != d ) { ticks.push_back( (ISolid::Tick) ( -1.0*(b-point.y())-point.z()*a ) / d ); }
    }
    {
      // 2) line y=-y1-(z-z1)*(y2-y1)/(z2-z1) =-y1-(z-z1)*dydz =-dydz*z -y1+z1*dydz = a*z+b
      //         z=z 
      //    line y = py + vy * t 
      //         z = pz + vz * t 
      // ------------------------------------
      //    a*z + b = py + vy * t 
      //      z     = pz + vz * t 
      // -----------------------------------
      //    t*vy - a*z = b - py 
      //    t*vz -   z =   - pz
      double a = -dydz;
      double b = -yHalfLength1()-zHalfLength()*dydz;
      double d = -1.0 * vect.y() + a * vect.z() ;
      if( 0 != d ) { ticks.push_back( (ISolid::Tick) ( -1.0*(b-point.y())-point.z()*a ) / d ); }
    }
  }
  
  ///
  ///
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , *this );  
  

};

///
///
///

/// calculate the intersection points("ticks") with a given line. 
/// Input - line, paramterised by (Point + Vector * Tick) 
/// "Tick" is just a value of parameter, at which the intercestion occurs 
/// Return the number of intersection points (=size of Ticks container)   
inline  unsigned int SolidTrd::intersectionTicks ( const HepPoint3D  & point   ,          // initial point for teh line 
						   const HepVector3D & vect    ,          // vector along the line 
                                                   const ISolid::Tick& tickMin ,          // minimal value for the Tick 
                                                   const ISolid::Tick& tickMax ,          // maximal value for the tick 
						   ISolid::Ticks     & ticks   ) const    // output container of "Ticks"
{
  ///
  intersectionTicks( point , vect , ticks ) ; 
  ///
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , tickMin, tickMax , *this );  
  ///
};
///
///
///




