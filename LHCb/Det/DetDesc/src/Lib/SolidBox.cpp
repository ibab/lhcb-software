#include "GaudiKernel/IInspector.h"

#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTicks.h" 
#include "DetDesc/SolidException.h" 

//////////////////////////////////////////////////////////////////////////////////////////////////////
SolidBox::SolidBox( const std::string& name        ,
		    const double       XHalfLength , 
		    const double       YHalfLength , 
		    const double       ZHalfLength )
  : m_box_name       ( name        ) 
  , m_box_xHalfLength( XHalfLength )
  , m_box_yHalfLength( YHalfLength )
  , m_box_zHalfLength( ZHalfLength )
{
  if( 0 >= xHalfLength() ){ throw SolidException("SolidBox(): XHalfLength is non positive! "); }
  if( 0 >= yHalfLength() ){ throw SolidException("SolidBox(): YHalfLength is non positive! "); }
  if( 0 >= zHalfLength() ){ throw SolidException("SolidBox(): ZHalfLength is non positive! "); }
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
SolidBox::SolidBox()
  : m_box_name("unnamed box")
  , m_box_xHalfLength( 100000.0 ) 
  , m_box_yHalfLength( 100000.0 ) 
  , m_box_zHalfLength( 100000.0 ) 
{}; 
//////////////////////////////////////////////////////////////////////////////////////////////////////
SolidBox::~SolidBox(){};
//////////////////////////////////////////////////////////////////////////////////////////////////////
StreamBuffer& SolidBox::serialize( StreamBuffer& s )       
{
  s >> m_box_name  
    >> m_box_xHalfLength 
    >> m_box_yHalfLength 
    >> m_box_zHalfLength ;
  ///
  if( 0 >= xHalfLength() ){ throw SolidException("SolidBox::serialize: XHalfLength is non positive!"); }
  if( 0 >= yHalfLength() ){ throw SolidException("SolidBox::serialize: YHalfLength is non positive!"); }
  if( 0 >= zHalfLength() ){ throw SolidException("SolidBox::serialize: ZHalfLength is non positive!"); }
  ///
  return s ;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
StreamBuffer& SolidBox::serialize( StreamBuffer& s ) const 
{
  ///
  s << typeName() ;
  ///
  return s << name() 
	   << xHalfLength() 
	   << yHalfLength() 
	   << zHalfLength() ; 
}; 
/**
   calculate the intersection points("ticks") with a given line. 
   Input - line, paramterised by (Point + Vector * Tick) 
   "Tick" is just a value of parameter, at which the intercestion occurs 
   Return the number of intersection points (=size of Ticks container)   
*/
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
  /// sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , *this );  
  ///  
};
/** 
    calculate the intersection points("ticks") with a given line. 
    Input - line, paramterised by (Point + Vector * Tick) 
    "Tick" is just a value of parameter, at which the intercestion occurs 
    Return the number of intersection points (=size of Ticks container)   
*/
inline unsigned int SolidBox::intersectionTicks( const HepPoint3D   & point   ,
						 const HepVector3D  & vect    ,
                                                 const ISolid::Tick & tickMin , 
                                                 const ISolid::Tick & tickMax , 
						 ISolid::Ticks      & ticks   ) const
{
  intersectionTicks( point , vect , ticks ); 
  return SolidTicks::RemoveAdjancentTicks( ticks , point , vect , tickMin, tickMax , *this );  
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool SolidBox::acceptInspector( IInspector* pInspector ) 
{
  ///
  const ISolid* s = this; 
  return s->acceptInspector( pInspector ) ; 
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool SolidBox::acceptInspector( IInspector* pInspector )  const 
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByRef ( &m_box_name         ,  this , "BoxName"  ) ; 
  pInspector->inspectByRef ( &m_box_xHalfLength  ,  this , "X-size/2" ) ; 
  pInspector->inspectByRef ( &m_box_yHalfLength  ,  this , "Y-size/2" ) ; 
  pInspector->inspectByRef ( &m_box_zHalfLength  ,  this , "Z-size/2" ) ; 
  ///
  return true;
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream& SolidBox::printOut  ( std::ostream&  os ) const
{
  return os << typeName() << " name="             << name()
	    << " (xsize=" << xsize() / millimeter << "[mm],"  
	    <<   "ysize=" << ysize() / millimeter << "[mm],"  
	    <<   "zsize=" << zsize() / millimeter << "[mm])" << std::endl ;   
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
MsgStream&    SolidBox::printOut  ( MsgStream&     os ) const
{
  return os << typeName() << " name="             << name()
	    << " (xsize=" << xsize() / millimeter << "[mm],"  
	    <<   "ysize=" << ysize() / millimeter << "[mm],"  
	    <<   "zsize=" << zsize() / millimeter << "[mm])" << endreq ;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
