// $Id: SolidBase.cpp,v 1.6 2002-05-14 06:59:48 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $  
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/05/13 18:29:54  ibelyaev
//  see $DETDESCROOT/doc/release.notes 13 May 2002
//
// Revision 1.4  2002/05/11 18:25:47  ibelyaev
//  see $DETDESCROOT/doc/release.notes 11 May 2002
//
// ============================================================================
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// GaudiKernel
#include "GaudiKernel/StreamBuffer.h"
/// DetDesc 
#include "DetDesc/DetDesc.h"
#include "DetDesc/SolidBase.h"
#include "DetDesc/SolidTicks.h"
#include "DetDesc/SolidException.h"

// ============================================================================
/** @file SolidBase.cpp  
 * 
 * Implementation file for class : SolidBase
 *
 * @author Vanya  Belyaev
 * @date 02/08/2001 
 */
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @param Name name of teh solid 
 */
// ============================================================================
SolidBase::SolidBase( const std::string& Name )
  : m_name   ( Name     ) 
  , m_count  ( 0        ) 
  , m_cover  ( 0        ) 
  , m_xmin   (  10 * km )
  , m_ymin   (  10 * km ) 
  , m_zmin   (  10 * km )
  , m_xmax   ( -10 * km ) 
  , m_ymax   ( -10 * km ) 
  , m_zmax   ( -10 * km )
  , m_rmax   ( -10 * km ) 
  , m_rhomax ( -10 * km )
{
  addRef(); ///< add the reference 
};

// ============================================================================
/// destructor 
// ============================================================================
SolidBase::~SolidBase() 
{ 
  reset   () ; 
  release () ; 
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
  else if ( ISerialize:: interfaceID() == ID )
    { *ppI = static_cast<ISerialize*> ( this ) ; } 
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
unsigned long SolidBase::addRef  () { return ++m_count ; }

// ============================================================================
/// Release Interface instance
// ============================================================================
unsigned long SolidBase::release () { return 0 < m_count ? --m_count : 0 ; }

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
       << DetDesc::print(   xMin () / mm )  << "/" 
       << DetDesc::print(   xMax () / mm )  << "," 
       << DetDesc::print(   yMin () / mm )  << "/" 
       << DetDesc::print(   yMax () / mm )  << "," 
       << DetDesc::print(   zMin () / mm )  << "/" 
       << DetDesc::print(   zMax () / mm )  << "," 
       << DetDesc::print(   rMax () / mm )  << "," 
       << DetDesc::print( rhoMax () / mm )  << ") " << std::endl ;
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
       << std::endl
       << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
       << DetDesc::print(   xMin () / mm )  << "/" 
       << DetDesc::print(   xMax () / mm )  << "," 
       << DetDesc::print(   yMin () / mm )  << "/" 
       << DetDesc::print(   yMax () / mm )  << "," 
       << DetDesc::print(   zMin () / mm )  << "/" 
       << DetDesc::print(   zMax () / mm )  << "," 
       << DetDesc::print(   rMax () / mm )  << "," 
       << DetDesc::print( rhoMax () / mm )  << ") " << endreq ;
};
// ============================================================================

// ============================================================================
/** serialization for reading
 *  @param sb reference to stream buffer
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidBase::serialize( StreamBuffer& sb )
{
  /// reset the current status before reading
  reset();
  /// read 
  return sb >> m_name ;
};


// ============================================================================
/** serialization for writing
 *  @param sb reference to stream buffer
 *  @return reference to stream buffer
 */
// ============================================================================
StreamBuffer& SolidBase::serialize( StreamBuffer& sb ) const
{
  return sb << typeName() <<  name()  ;
};

// ============================================================================
/** return FALSE if doesn't accept Inspecetor so that diagnostic 
 *  message can be printed. Calls the inspect member function of 
 *  the Inspector on all the members it wants to inspect.
 */
// ============================================================================
bool SolidBase::acceptInspector( IInspector* pI )       
{
  return 0 != pI ? true : false ;
};

// ============================================================================
/** return FALSE if doesn't accept Inspecetor so that diagnostic 
 *  message can be printed. Calls the inspect member function of 
 *  the Inspector on all the members it wants to inspect.
 */
// ============================================================================
bool SolidBase::acceptInspector( IInspector* pI ) const 
{
  return 0 != pI ? true : false ;
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
unsigned int 
SolidBase::intersectionTicks 
( const HepPoint3D  & Point   ,         
  const HepVector3D & Vector  ,         
  const ISolid::Tick& tickMin ,         
  const ISolid::Tick& tickMax ,         
  ISolid::Ticks     & ticks   ) const 
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
unsigned int 
SolidBase::intersectionTicks 
( const HepPoint3D  & /* Point */  ,         
  const HepVector3D & /* Vector */ ,         
  ISolid::Ticks     &    ticks     ) const 
{
  ///  reset the output container 
  ticks.clear();
  ///  return container size 
  return ticks.size();
};
// ============================================================================

// ============================================================================
/** check bounding parameters 
 *  @return status code
 */
// ============================================================================
StatusCode SolidBase::checkBP() const
{
  const std::string msg("SolidBase("+typeName()+"/"+name()+")::checkBP(): ");
  if     ( xMax()   <= xMin() ) 
    { 
      MsgStream log( DetDesc::msgSvc() , "Solid" );
      log << MSG::FATAL
          << " SolidBase::checkPB "
          << typeName() << "/" << name() 
          << " Wrong Bounding Parameters "            << endreq ;
      log << MSG::ERROR 
          << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
          << DetDesc::print(   xMin () / mm )  << "/" 
          << DetDesc::print(   xMax () / mm )  << "," 
          << DetDesc::print(   yMin () / mm )  << "/" 
          << DetDesc::print(   yMax () / mm )  << "," 
          << DetDesc::print(   zMin () / mm )  << "/" 
          << DetDesc::print(   zMax () / mm )  << "," 
          << DetDesc::print(   rMax () / mm )  << "," 
          << DetDesc::print( rhoMax () / mm )  << ") " << endreq ;
      throw SolidException( msg + "   xMax() <= xMin() " 
                            + DetDesc::print( xMax  () ) + "/"  
                            + DetDesc::print( xMin  () ) ); 
    }
  else if( yMax()   <= yMin() ) 
    { 
      MsgStream log( DetDesc::msgSvc() , "Solid" );
      log << MSG::FATAL
          << " SolidBase::checkPB "
          << typeName() << "/" << name() 
          << " Wrong Bounding Parameters "            << endreq ;
      log << MSG::ERROR 
          << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
          << DetDesc::print(   xMin () / mm )  << "/" 
          << DetDesc::print(   xMax () / mm )  << "," 
          << DetDesc::print(   yMin () / mm )  << "/" 
          << DetDesc::print(   yMax () / mm )  << "," 
          << DetDesc::print(   zMin () / mm )  << "/" 
          << DetDesc::print(   zMax () / mm )  << "," 
          << DetDesc::print(   rMax () / mm )  << "," 
          << DetDesc::print( rhoMax () / mm )  << ") " << endreq ;
      throw SolidException( msg + "   yMax() <= yMin() " 
                            + DetDesc::print( yMax  () ) + "/"  
                            + DetDesc::print( yMin  () ) ) ; 
    }
  else if( zMax()   <= zMin() ) 
    {  
      MsgStream log( DetDesc::msgSvc() , "Solid" );
      log << MSG::FATAL
          << " SolidBase::checkPB "
          << typeName() << "/" << name() 
          << " Wrong Bounding Parameters "            << endreq ;
      log << MSG::ERROR 
          << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
          << DetDesc::print(   xMin () / mm )  << "/" 
          << DetDesc::print(   xMax () / mm )  << "," 
          << DetDesc::print(   yMin () / mm )  << "/" 
          << DetDesc::print(   yMax () / mm )  << "," 
          << DetDesc::print(   zMin () / mm )  << "/" 
          << DetDesc::print(   zMax () / mm )  << "," 
          << DetDesc::print(   rMax () / mm )  << "," 
          << DetDesc::print( rhoMax () / mm )  << ") " << endreq ;
      throw SolidException( msg + "   zMax() <= zMin() " 
                            + DetDesc::print( zMax  () ) + "/"  
                            + DetDesc::print( zMin  () ) ); 
    }
  else if( rMax()   <= 0      ) 
    {
      MsgStream log( DetDesc::msgSvc() , "Solid" );
      log << MSG::FATAL
          << " SolidBase::checkPB "
          << typeName() << "/" << name() 
          << " Wrong Bounding Parameters "            << endreq ;
      log << MSG::ERROR 
          << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
          << DetDesc::print(   xMin () / mm )  << "/" 
          << DetDesc::print(   xMax () / mm )  << "," 
          << DetDesc::print(   yMin () / mm )  << "/" 
          << DetDesc::print(   yMax () / mm )  << "," 
          << DetDesc::print(   zMin () / mm )  << "/" 
          << DetDesc::print(   zMax () / mm )  << "," 
          << DetDesc::print(   rMax () / mm )  << "," 
          << DetDesc::print( rhoMax () / mm )  << ") " << endreq ;
      throw SolidException( msg + "   rMax() <=  0     " 
                            + DetDesc::print( rMax  () ) ); 
    }
  else if( rhoMax() <= 0      ) 
    {  
      MsgStream log( DetDesc::msgSvc() , "Solid" );
      log << MSG::FATAL
          << " SolidBase::checkPB "
          << typeName() << "/" << name() 
          << " Wrong Bounding Parameters "            << endreq ;
      log << MSG::ERROR 
          << " BPs: (x,y,z,r,rho)[Min/Max][mm]=(" 
          << DetDesc::print(   xMin () / mm )  << "/" 
          << DetDesc::print(   xMax () / mm )  << "," 
          << DetDesc::print(   yMin () / mm )  << "/" 
          << DetDesc::print(   yMax () / mm )  << "," 
          << DetDesc::print(   zMin () / mm )  << "/" 
          << DetDesc::print(   zMax () / mm )  << "," 
          << DetDesc::print(   rMax () / mm )  << "," 
          << DetDesc::print( rhoMax () / mm )  << ") " << endreq ;
      throw SolidException( msg + " rhoMax() <=  0     " 
                            + DetDesc::print( rhoMax() ) ); 
    }
  //
  return StatusCode::SUCCESS ;
};
// ============================================================================
  
    

// ============================================================================
// The End 
// ============================================================================

