// $Id: SolidBase.cpp,v 1.16 2006-10-11 15:02:58 cattanem Exp $

// Units
#include "GaudiKernel/SystemOfUnits.h"
/// DetDesc 
#include "DetDesc/DetDesc.h"
#include "DetDesc/SolidBase.h"
#include "DetDesc/SolidTicks.h"
#include "DetDesc/SolidException.h"

/** @file SolidBase.cpp  
 * 
 * Implementation file for class : SolidBase
 *
 * @author Vanya  Belyaev
 * @author Sebastien Ponce
 */

// ============================================================================
/** standard constructor 
 *  @param Name name of teh solid 
 */
// ============================================================================
SolidBase::SolidBase( const std::string& Name )
  : m_name   ( Name     ) 
  , m_cover  ( 0        ) 
  , m_xmin   (  10 * Gaudi::Units::km )
  , m_ymin   (  10 * Gaudi::Units::km ) 
  , m_zmin   (  10 * Gaudi::Units::km )
  , m_xmax   ( -10 * Gaudi::Units::km ) 
  , m_ymax   ( -10 * Gaudi::Units::km ) 
  , m_zmax   ( -10 * Gaudi::Units::km )
  , m_rmax   ( -10 * Gaudi::Units::km ) 
  , m_rhomax ( -10 * Gaudi::Units::km )
  , m_services (0)
{
  // get message service
  m_services = DetDesc::services();
};

// ============================================================================
/// destructor 
// ============================================================================
SolidBase::~SolidBase() 
{ 
  if( 0 != m_cover && this != m_cover ) { delete m_cover ; }
  m_cover = 0 ;
  // release message service
  m_services->release();
};

// ============================================================================
/** - reset solid to its inititial state, remove 
 *    all auxillary data fields and pointers. 
 *  - implementation of ISolid abstract interface  
 *  @see ISolid 
 *  @see SolidBase::~SolidBase()
 *  @return  self-reference 
 */
// ============================================================================
ISolid* SolidBase::reset ()
{
  if( this == m_cover ) {                  m_cover = 0 ; }
  if(    0 != m_cover ) { delete m_cover ; m_cover = 0 ; }
  ///
  return this;
};

// ============================================================================
/** query the interface 
 *  @param ID unique interafce identifier 
 *  @param ppI placeholder for returned interface
 *  @return status code 
 */
// ============================================================================
StatusCode SolidBase::queryInterface( const InterfaceID& ID , void** ppI )
{  
  if ( 0 == ppI ) { return StatusCode::FAILURE; }
  *ppI = 0 ;
  if      ( ISolid::     interfaceID() == ID ) 
    { *ppI = static_cast<ISolid*>     ( this ) ; } 
  else if ( IInterface:: interfaceID() == ID ) 
    { *ppI = static_cast<IInterface*> ( this ) ; } 
  else                                                  
    { return StatusCode::FAILURE               ; }
  /// add the reference 
  addRef();
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/// Increment the reference count of Interface instance
// ============================================================================
unsigned long SolidBase::addRef  () { return 0; }

// ============================================================================
/// Release Interface instance
// ============================================================================
unsigned long SolidBase::release () { return 0; }

// ============================================================================
/** retrieve the pointer to "the most simplified cover"
 *  probably, something like "gabarite box"
 *  @return pointer to the most simplified cover
 */
// ============================================================================
const ISolid* SolidBase::coverTop () const 
{
  const ISolid* cov = this; 
  while( 0 != cov && cov != cov->cover() ){ cov = cov->cover(); }
  if( 0 == cov ) { cov = this ; }
  return cov ; 
};

// ============================================================================
/** printout to STD/STL stream
 *  @param st STD/STL stream
 *  @return reference to the stream
 */
// ============================================================================
std::ostream& SolidBase::printOut ( std::ostream& st ) const 
{
  return 
    st << " SolidType='"     << typeName () << "'" 
       << " \tname='"        << name     () << "'" 
       << std::endl
       << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
       << DetDesc::print(   xMin () / Gaudi::Units::mm )  << "/" 
       << DetDesc::print(   xMax () / Gaudi::Units::mm )  << "," 
       << DetDesc::print(   yMin () / Gaudi::Units::mm )  << "/" 
       << DetDesc::print(   yMax () / Gaudi::Units::mm )  << "," 
       << DetDesc::print(   zMin () / Gaudi::Units::mm )  << "/" 
       << DetDesc::print(   zMax () / Gaudi::Units::mm )  << "," 
       << DetDesc::print(   rMax () / Gaudi::Units::mm )  << "," 
       << DetDesc::print( rhoMax () / Gaudi::Units::mm )  << ") " << std::endl ;
};
// ============================================================================

// ============================================================================
/** printout to Gaudi  stream
 *  @param st Gaudi stream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream&    SolidBase::printOut ( MsgStream&    st ) const 
{
  return 
    st << " SolidType='"     << typeName () << "'" 
       << " \tname='"        << name     () << "'" 
       << endreq 
       << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
       << DetDesc::print(   xMin () / Gaudi::Units::mm )  << "/" 
       << DetDesc::print(   xMax () / Gaudi::Units::mm )  << "," 
       << DetDesc::print(   yMin () / Gaudi::Units::mm )  << "/" 
       << DetDesc::print(   yMax () / Gaudi::Units::mm )  << "," 
       << DetDesc::print(   zMin () / Gaudi::Units::mm )  << "/" 
       << DetDesc::print(   zMax () / Gaudi::Units::mm )  << "," 
       << DetDesc::print(   rMax () / Gaudi::Units::mm )  << "," 
       << DetDesc::print( rhoMax () / Gaudi::Units::mm )  << ") " << endmsg ;
};

// ============================================================================
/** - calculate the intersection points("ticks") of the solid objects 
 *    with given line. 
 *  -# Line is parametrized with parameter \a t : 
 *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
 *      - \f$ \vec{p} \f$ is a point on the line 
 *      - \f$ \vec{v} \f$ is a vector along the line  
 *  -# \a tick is just a value of parameter \a t, at which the
 *    intersection of the solid and the line occurs
 *  -# both  \a Point  (\f$\vec{p}\f$) and \a Vector  
 *    (\f$\vec{v}\f$) are defined in local reference system 
 *   of the solid 
 *  -# Only intersection ticks within the range 
 *     \a tickMin and \a tickMax are taken into account. 
 *  - implementation of ISolid abstract interface  
 *  @see ISolid 
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param tickMin minimum value of Tick 
 *  @param tickMax maximu value of Tick 
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points
 */
// ============================================================================
unsigned int SolidBase::intersectionTicks( const Gaudi::XYZPoint  & Point,
                                           const Gaudi::XYZVector & Vector,
                                           const ISolid::Tick& tickMin,
                                           const ISolid::Tick& tickMax,
                                           ISolid::Ticks     & ticks) const 
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
unsigned int SolidBase::intersectionTicks( const Gaudi::Polar3DPoint  & Point,
                                           const Gaudi::Polar3DVector & Vector,
                                           const ISolid::Tick& tickMin,
                                           const ISolid::Tick& tickMax,
                                           ISolid::Ticks     & ticks) const 
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
unsigned int SolidBase::intersectionTicks( const Gaudi::RhoZPhiPoint  & Point,
                                           const Gaudi::RhoZPhiVector & Vector,
                                           const ISolid::Tick& tickMin,
                                           const ISolid::Tick& tickMax,
                                           ISolid::Ticks     & ticks) const 
{
  return intersectionTicksImpl(Point, Vector, tickMin, tickMax, ticks);
}
// ============================================================================
template<class aPoint, class aVector>
unsigned int SolidBase::intersectionTicksImpl( const aPoint  & Point,
                                               const aVector & Vector,
                                               const ISolid::Tick& tickMin,
                                               const ISolid::Tick& tickMax,
                                               ISolid::Ticks&  ticks) const 
{
  // check for bounding box 
  if( isOutBBox( Point , Vector , tickMin , tickMax ) ) { return 0; }
  //
  intersectionTicks( Point , Vector , ticks ); 
  // sort and remove adjancent and some EXTRA ticks and return 
  return SolidTicks::RemoveAdjancentTicks( ticks   , 
                                           Point   , 
                                           Vector  , 
                                           tickMin , 
                                           tickMax , 
                                           *this   );
};

// ============================================================================
/** - calculate the intersection points("ticks") of the solid objects 
 *    with given line. 
 *  -# Line is parametrized with parameter \a t : 
 *     \f$ \vec{x}(t) = \vec{p} + t \times \vec{v} \f$ 
 *      - \f$ \vec{p} \f$ is a point on the line 
 *      - \f$ \vec{v} \f$ is a vector along the line  
 *  -# \a tick is just a value of parameter \a t, at which the
 *    intersection of the solid and the line occurs
 *  -# both  \a Point  (\f$\vec{p}\f$) and \a Vector  
 *    (\f$\vec{v}\f$) are defined in local reference system 
 *   of the solid 
 *  - implementation of ISolid abstract interface  
 *  @see ISolid 
 *  @param Point initial point for the line
 *  @param Vector vector along the line
 *  @param tickMin minimum value of Tick 
 *  @param tickMax maximu value of Tick 
 *  @param ticks output container of "Ticks"
 *  @return the number of intersection points
 */
// ============================================================================
unsigned int SolidBase::intersectionTicks( const Gaudi::XYZPoint&  ,         
                                           const Gaudi::XYZVector&,         
                                           ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(ticks);
};
// ============================================================================
unsigned int SolidBase::intersectionTicks( const Gaudi::Polar3DPoint&,
                                           const Gaudi::Polar3DVector&,
                                           ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(ticks);
};
// ============================================================================
unsigned int SolidBase::intersectionTicks( const Gaudi::RhoZPhiPoint&,
                                           const Gaudi::RhoZPhiVector&, 
                                           ISolid::Ticks&    ticks ) const 
{
  return intersectionTicksImpl(ticks);
};
// ============================================================================

unsigned int SolidBase::intersectionTicksImpl( ISolid::Ticks&    ticks ) const 
{
  ///  reset the output container 
  ticks.clear();
  ///  return container size 
  return ticks.size();
};
// ============================================================================

// ============================================================================

// ============================================================================
/** check bounding parameters 
 *  @return status code
 */
// ============================================================================
StatusCode SolidBase::checkBP() const
{
  const std::string msg("SolidBase("+m_name+")::checkBP(): ");
  if     ( xMax()   <= xMin() ) 
    { 
      MsgStream log( msgSvc() , "Solid" );
      log << MSG::FATAL
          << " SolidBase::checkPB "
          << m_name 
          << " Wrong Bounding Parameters "            << endreq ;
      log << MSG::ERROR 
          << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
          << DetDesc::print(   xMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   xMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   yMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   yMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   zMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   zMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   rMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print( rhoMax () / Gaudi::Units::mm )  << ") " << endmsg ;
      throw SolidException( msg + "   xMax() <= xMin() " 
                            + DetDesc::print( xMax  () ) + "/"  
                            + DetDesc::print( xMin  () ) ); 
    }
  else if( yMax()   <= yMin() ) 
    { 
      MsgStream log( msgSvc() , "Solid" );
      log << MSG::FATAL
          << " SolidBase::checkPB "
          << m_name  
          << " Wrong Bounding Parameters "            << endreq ;
      log << MSG::ERROR 
          << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
          << DetDesc::print(   xMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   xMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   yMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   yMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   zMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   zMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   rMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print( rhoMax () / Gaudi::Units::mm )  << ") " << endmsg ;
      throw SolidException( msg + "   yMax() <= yMin() " 
                            + DetDesc::print( yMax  () ) + "/"  
                            + DetDesc::print( yMin  () ) ) ; 
    }
  else if( zMax()   <= zMin() ) 
    {  
      MsgStream log( msgSvc() , "Solid" );
      log << MSG::FATAL
          << " SolidBase::checkPB "
          << m_name 
          << " Wrong Bounding Parameters "            << endreq ;
      log << MSG::ERROR 
          << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
          << DetDesc::print(   xMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   xMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   yMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   yMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   zMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   zMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   rMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print( rhoMax () / Gaudi::Units::mm )  << ") " << endmsg ;
      throw SolidException( msg + "   zMax() <= zMin() " 
                            + DetDesc::print( zMax  () ) + "/"  
                            + DetDesc::print( zMin  () ) ); 
    }
  else if( rMax()   <= 0      ) 
    {
      MsgStream log( msgSvc() , "Solid" );
      log << MSG::FATAL
          << " SolidBase::checkPB "
          << m_name 
          << " Wrong Bounding Parameters "            << endreq ;
      log << MSG::ERROR 
          << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
          << DetDesc::print(   xMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   xMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   yMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   yMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   zMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   zMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   rMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print( rhoMax () / Gaudi::Units::mm )  << ") " << endmsg ;
      throw SolidException( msg + "   rMax() <=  0     " 
                            + DetDesc::print( rMax  () ) ); 
    }
  else if( rhoMax() <= 0      ) 
    {  
      MsgStream log( msgSvc() , "Solid" );
      log << MSG::FATAL
          << " SolidBase::checkPB "
          << m_name
          << " Wrong Bounding Parameters "            << endreq ;
      log << MSG::ERROR 
          << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
          << DetDesc::print(   xMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   xMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   yMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   yMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   zMin () / Gaudi::Units::mm )  << "/" 
          << DetDesc::print(   zMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print(   rMax () / Gaudi::Units::mm )  << "," 
          << DetDesc::print( rhoMax () / Gaudi::Units::mm )  << ") " << endmsg ;
      throw SolidException( msg + " rhoMax() <=  0     " 
                            + DetDesc::print( rhoMax() ) ); 
    }
  //
  return StatusCode::SUCCESS ;
};
// ============================================================================
  
/** 
 *  accessor to message service
 *  @return pointer to message service 
 */
IMessageSvc* SolidBase::msgSvc() const { return m_services->msgSvc(); }


// ============================================================================
// The End 
// ============================================================================

