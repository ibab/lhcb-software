
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 


//
//
//

SolidBox::SolidBox( const std::string& name        ,
		    const double       XHalfLength , 
		    const double       YHalfLength , 
		    const double       ZHalfLength )
  : m_box_name       ( name        ) 
  , m_box_xHalfLength( XHalfLength )
  , m_box_yHalfLength( YHalfLength )
  , m_box_zHalfLength( ZHalfLength )
{
  if( 0 >= xHalfLength() ){ throw SolidException("XHalfLength is non positive!", this); }
  if( 0 >= yHalfLength() ){ throw SolidException("YHalfLength is non positive!", this); }
  if( 0 >= zHalfLength() ){ throw SolidException("ZHalfLength is non positive!", this); }
};


///
/// "fictive" default constructor 
///
SolidBox::SolidBox()
  ///
  : m_box_name("unnamed box")
  , m_box_xHalfLength( 100000.0 ) 
  , m_box_yHalfLength( 100000.0 ) 
  , m_box_zHalfLength( 100000.0 ) 
  ///
{}; 
///

///
/// serialization for reading 
///
StreamBuffer& SolidBox::serialize( StreamBuffer& s )       
{
  s >> m_box_name  
    >> m_box_xHalfLength 
    >> m_box_yHalfLength 
    >> m_box_zHalfLength ;
  ///
  if( 0 >= xHalfLength() ){ throw SolidException("XHalfLength is non positive!", this); }
  if( 0 >= yHalfLength() ){ throw SolidException("YHalfLength is non positive!", this); }
  if( 0 >= zHalfLength() ){ throw SolidException("ZHalfLength is non positive!", this); }
  ///
  return s ;
};

/// serialization for writing 
StreamBuffer& SolidBox::serialize( StreamBuffer& s ) const 
{
  return s << typeName() 
	   << name() 
	   << xHalfLength() 
	   << yHalfLength() 
	   << zHalfLength() ; 
}; 


///
///
///
// calculate the intersection points("ticks") with a given line. 
// Input - line, paramterised by (Point + Vector * Tick) 
// "Tick" is just a value of parameter, at which the intercestion occurs 
// Return the number of intersection points (=size of Ticks container)   

inline unsigned int SolidBox::intersectionTicks( const HepPoint3D & point  ,
						 const HepVector3D& vect   ,
						 ISolid::Ticks    & ticks  ) const
{
  ///
  ticks.clear(); 

  /// find intersection ticks with x-planes
  SolidTicks::LineIntersectsTheX( point , vect ,        xHalfLength() , std::back_inserter( ticks ) ); 
  SolidTicks::LineIntersectsTheX( point , vect , -1.0 * xHalfLength() , std::back_inserter( ticks ) ); 
  
  /// find intersection ticks with y-planes
  SolidTicks::LineIntersectsTheY( point , vect ,        yHalfLength() , std::back_inserter( ticks ) ); 
  SolidTicks::LineIntersectsTheY( point , vect , -1.0 * yHalfLength() , std::back_inserter( ticks ) ); 
  
  /// find intersection ticks with z-planes
  SolidTicks::LineIntersectsTheZ( point , vect ,        zHalfLength() , std::back_inserter( ticks ) ); 
  SolidTicks::LineIntersectsTheZ( point , vect , -1.0 * zHalfLength() , std::back_inserter( ticks ) ); 
  
  ///
  ///
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , *this );  
  ///  
};


///
///
///
// calculate the intersection points("ticks") with a given line. 
// Input - line, paramterised by (Point + Vector * Tick) 
// "Tick" is just a value of parameter, at which the intercestion occurs 
// Return the number of intersection points (=size of Ticks container)   

inline unsigned int SolidBox::intersectionTicks( const HepPoint3D   & point   ,
						 const HepVector3D  & vect    ,
                                                 const ISolid::Tick & tickMin , 
                                                 const ISolid::Tick & tickMax , 
						 ISolid::Ticks      & ticks   ) const
{
  ///
  intersectionTicks( point , vect , ticks ); 
  ///
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , tickMin, tickMax , *this );  
  ///
};

///
///
///

