// $Id: PVolume.cpp,v 1.22 2006-04-20 14:39:56 ranjard Exp $ 

/// GaudiKernel includes 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h" 
/// STD & STL includes 
#include <cassert>
#include <vector>
#include <algorithm>
/// CLHEP includes 
#include "Kernel/Transform3DTypes.h"
/// DetDesc 
#include "DetDesc/DetDesc.h"
#include "DetDesc/PVolume.h"
#include "DetDesc/PVolumeException.h"

// ============================================================================
/** @file PVolume.cpp 
 *  
 *  implementation of class PVolume 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author Sebastien Ponce
 */
// ============================================================================

unsigned long PVolume::s_volumeCounter = 0;

namespace
{
  std::string ITOA( const int value )
  {
    char buffer[64];
    return std::string( buffer , buffer + sprintf( buffer , "%d" , value ) );
  };
};


// ============================================================================
/** constructor    
 *  @param PhysVol_name name of phys volume 
 *  @param LogVol_name name of log volume 
 *  @param Position position of physical volume inside mother volume 
 *  @param Rotation  rotation of physical volume with respect to mother 
 */
// ============================================================================
PVolume::PVolume 
( const std::string& PhysVol_name  ,
  const std::string& LogVol_name   ,
  //  const size_t       copy_number   ,
  const Gaudi::XYZPoint&   Position      ,
  const Gaudi::Rotation3D& Rotation      )
  : m_name      ( PhysVol_name   ) 
  , m_lvname    ( LogVol_name    ) 
  , m_nominal   (                )
  , m_matrix    (                )
  , m_imatrix   ( 0              )
  , m_lvolume   ( 0              )
  , m_services  ( 0              )
  //, m_copy      ( copy_number    ) 
{
  // NB!!! transformaion is given by Translation and then Rotation!!!
//   m_nominal = Gaudi::Transform3D(Rotation ) *
//     Gaudi::Transform3D(Gaudi::XYZVector(Position) ) ; // MathCore syntax
  m_nominal =  Gaudi::Transform3D(Gaudi::XYZVector(Position), Rotation);
  //  m_nominal = Rotation*Gaudi::TranslationXYZ(Position) ; // CLHEP syntax
  m_matrix  = m_nominal ;
  /// 
  //  { /// ensure the agreement between name and copy number
  //    std::string::iterator sep = 
  //      std::find( m_name.begin() , m_name.end() , ':' ) ;
  //    m_name.erase( sep , m_name.end() ) ;
  //    m_name += ":" + ITOA( copy_number ) ;
  //  }
  ///
  m_services = DetDesc::services();
  ++s_volumeCounter ;
};

// ============================================================================
/** constructor    
 *  @param PhysVol_name name of phys volume 
 *  @param LogVol_name name of log volume 
 *  @param Transform transformation matrix 
 */
// ============================================================================
PVolume::PVolume 
( const std::string&    PhysVol_name ,
  const std::string&    LogVol_name  ,
  //  const size_t          copy_number  ,
  const Gaudi::Transform3D& Transform    )
  : m_name      ( PhysVol_name   ) 
  , m_lvname    ( LogVol_name    ) 
  , m_nominal   ( Transform      )
  , m_matrix    ( Transform      )
  , m_imatrix   ( 0              )
  , m_lvolume   ( 0              )
  , m_services  ( 0              )
  //, m_copy      ( copy_number    ) 
{ 
  //  { /// ensure the agreement between name and copy number 
  //    std::string::iterator sep = 
  //      std::find( m_name.begin() , m_name.end() , ':' ) ;
  //    m_name.erase( sep , m_name.end() ) ;
  //    m_name += ":" + ITOA( copy_number ) ;
  //  }
  ///
  m_services = DetDesc::services();
  ++s_volumeCounter ;
};

// ============================================================================
// destructor 
// ============================================================================
PVolume::~PVolume() 
{
  if( 0 != m_imatrix ) { delete m_imatrix   ; m_imatrix = 0 ; }
  m_services->release();
  --s_volumeCounter;
};

// ============================================================================
/** find logical volume by name 
 *  @return pointet to logical volume 
 */
// ============================================================================
ILVolume* PVolume::findLogical() const 
{
  m_lvolume = 0 ;
  ILVolume* lv = 0 ;
  try
    { 
      SmartDataPtr<ILVolume> ptr( dataSvc() , lvolumeName() );
      if( 0 != ptr ) { lv = ptr.operator->(); }
    }
  catch( const GaudiException& Exception ) 
    { Assert( false , 
              " PVolume::findLogical(), exception caught! " , 
              Exception ) ; } 
  catch( const std::exception& Exception ) 
    { Assert( false , 
              " PVolume::findLogical(), std::exception caught! " + 
              std::string(Exception.what())) ; }   
  catch(...)                    
    { Assert( false , 
              " PVolume::findLogical(), unknown exception caught! ") ; } 
  ///
  Assert( 0 != lv , 
          " PVolume::findLogical, unable to locate LV=" + 
          lvolumeName() );
  ///
  m_lvolume = lv ;
  ///
  return m_lvolume;
};

// ============================================================================
/** inverse the matrix
 *  @return pointer to inverse matrix 
 */
// ============================================================================
Gaudi::Transform3D* PVolume::findMatrix() const 
{
  if( 0 != m_imatrix ) { delete m_imatrix ; m_imatrix = 0 ; }
  return m_imatrix = new Gaudi::Transform3D( matrix().Inverse() ) ;
};

// ============================================================================
/** the static accessor to the data service
 *  @return pointer to data service 
 */
// ============================================================================
IDataProviderSvc* PVolume::dataSvc() const { return m_services->detSvc(); }

// ============================================================================
/** query the interface
 *  @param ID unique interface identifier 
 *  @param ppI placeholder for returned interface
 *  @return status code 
 */
// ============================================================================
StatusCode 
PVolume::queryInterface( const InterfaceID& ID , void** ppI ) 
{
  if ( 0 == ppI ) { return StatusCode::FAILURE; }
  *ppI = 0 ;
  if      ( IPVolume::   interfaceID() == ID ) 
    { *ppI = static_cast<IPVolume*>     ( this ) ; } 
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
/** add the reference
 *  @return reference counter 
 */
// ============================================================================
unsigned long PVolume::addRef  () { return 1; }

// ============================================================================
/** release the interface 
 *  @return reference counter 
 */
// ============================================================================
unsigned long PVolume::release () { return 1;}

// ============================================================================
/** printout to STD/STL stream 
 *  @param os reference to STD/STL stream 
 *  @return reference to STD/STL stream 
 */
// ============================================================================
std::ostream& PVolume::printOut( std::ostream& os ) const
{
  return 
    os << " class PVolume (" << s_volumeCounter << ")"
       << " [" 
       << " name='"          << name()          << "'" 
       << " logvol='"        << lvolumeName()   << "'" << "]";
};

// ============================================================================
/** printout to Gaudi MsgStream stream 
 *  @param os reference to MsgStream stream 
 *  @return reference to MsgStream stream 
 */
// ============================================================================
MsgStream& PVolume::printOut( MsgStream& os ) const
{
  return 
    os << " class PVolume (" << s_volumeCounter << ")"
       << " [" 
       << " name='"          << name()          << "'" 
       << " logvol='"        << lvolumeName()   << "'" << "]";
};

// ============================================================================
/** Assertion 
 *  @exception PVolumeException for wrong condition 
 *  @param assertion condition 
 *  @param name      exception message
 */
// ============================================================================
void PVolume::Assert( bool               assertion , 
                      const std::string& name      ) const
{ 
  if( !assertion ) 
    { throw PVolumeException( name, this ); } 
};

// ============================================================================
/** Assertion 
 *  @exception PVolumeException for wrong condition 
 *  @param assertion condition 
 *  @param name       exception message
 *  @param Exception  previous exception 
 */  
// ============================================================================
void PVolume::Assert( bool                  assertion , 
                      const std::string&    name      ,
                      const GaudiException& Exception ) const
{ 
  if( !assertion ) 
    { throw PVolumeException( name, Exception , this ); } 
};

// ============================================================================
/**  retrieve  the C++ pointer to Logical Volume 
 *  @return pointer to Logical Volume 
 */
// ============================================================================
const ILVolume* PVolume::lvolume () const
{ return 0 != m_lvolume  ? m_lvolume : m_lvolume = findLogical() ; };
// ============================================================================

// ============================================================================
/** get the inverse transformation matrix
 *  @return reference to inverse transformationmatrix 
 */
// ============================================================================
const Gaudi::Transform3D&  PVolume::matrixInv  () const 
{
  if( 0 == m_imatrix ) { m_imatrix = findMatrix() ; }
  return *m_imatrix ;
};
// ============================================================================

// ============================================================================
/** transform point from  Mother Reference System  to the Local one
 *  @param PointInMother point in Mother Reference System 
 *  @return point in local reference system 
 */ 
// ============================================================================
Gaudi::XYZPoint PVolume::toLocal 
( const Gaudi::XYZPoint& PointInMother ) const 
{ return m_matrix * PointInMother ; }
// ============================================================================

// ============================================================================
/** transform point in Local Reference System to the Mother Reference System
 *  @param PointInLocal point in Local Referency System
 *  @return point in mother reference system 
 */
// ============================================================================
Gaudi::XYZPoint PVolume::toMother ( const Gaudi::XYZPoint& PointInLocal  ) const 
{
  if( 0 == m_imatrix ) { m_imatrix = findMatrix() ; }  
  return (*m_imatrix) * PointInLocal ;
};
// ============================================================================

// ============================================================================
/** check for 3D-point
 *  @param PointInMother pointin Mother Referency System 
 *  @return true if point is inside physical volume 
 */
// ============================================================================
bool PVolume::isInside   
( const Gaudi::XYZPoint& PointInMother ) const 
{
  if( 0 == m_lvolume ) { m_lvolume = findLogical() ; }
  return m_lvolume->isInside( toLocal( PointInMother ) ) ;
};
// ============================================================================

// ============================================================================
/** reset to the initial state 
 *  @return self-reference
 */
// ============================================================================
IPVolume* PVolume::reset () 
{
  if( 0 != m_lvolume ) { m_lvolume->reset() ; m_lvolume = 0 ; }
  if( 0 != m_imatrix ) { delete m_imatrix   ; m_imatrix = 0 ; }
  return this;
};
// ============================================================================


// ============================================================================
/** Intersection of the physical volume with with line.
 *  The line is parametrized in the local reference system of the mother
 *  logical volume ("Mother Reference System")  
 *  with initial Point and direction Vector: 
 *   - @f$ \vec{x}(t) = \vec{p} + t \times \vec{v} @f$ @n 
 *  
 * Method returns the number of intersection points("ticks") and 
 * the container of pairs - ticks and pointer to the corresponding 
 * material. @n 
 * The simplification is determined by value of threshold
 * (in units of radiation length) 
 *  
 *  @see ILVolume
 *  @see ISolid 
 *
 *  @exception PVolumeException wrong environment 
 *  @param Point initial point at the line
 *  @param Vector direction vector of the line
 *  @param intersections output container 
 *  @param threshold threshold value 
 */
// ============================================================================
unsigned int PVolume::intersectLine
( const Gaudi::XYZPoint        & Point         ,
  const Gaudi::XYZVector       & Vector        , 
  ILVolume::Intersections & intersections ,
  const double              threshold     ) const 
{
  const ILVolume* lv = 
    0 != m_lvolume  ? m_lvolume : m_lvolume = findLogical() ;
  return lv->intersectLine ( m_matrix * Point  , 
                             m_matrix * Vector , 
                             intersections     , 
                             threshold         ); 
};
// ============================================================================

// ============================================================================
/** Intersection of the physical volume with with line.
 *  The line is parametrized in the local reference system of the mother
 *  logical volume ("Mother Reference System")  
 *  with initial Point and direction Vector: 
 *   - @f$ \vec{x}(t) = \vec{p} + t \times \vec{v} @f$ @n 
 *  
 * Method returns the number of intersection points("ticks") and 
 * the container of pairs - ticks and pointer to the corresponding 
 * material. @n 
 * The simplification is determined by value of threshold
 * (in units of radiation length) 
 *  
 *  @see ILVolume
 *  @see ISolid 
 *
 *  @exception PVolumeException wrong environment 
 *  @param Point initial point at the line
 *  @param Vector direction vector of the line
 *  @param intersections output container 
 *  @param threshold threshold value 
 */
// ============================================================================
unsigned int PVolume::intersectLine
( const Gaudi::XYZPoint        & Point ,
  const Gaudi::XYZVector       & Vector        ,       
  ILVolume::Intersections & intersections ,      
  const ISolid::Tick        tickMin       ,
  const ISolid::Tick        tickMax       ,
  const double              threshold     ) const
{ 
  const ILVolume* lv = 
    0 != m_lvolume  ? m_lvolume : m_lvolume = findLogical() ;
  return lv->intersectLine( m_matrix * Point    , 
                            m_matrix * Vector   , 
                            intersections       , 
                            tickMin             , 
                            tickMax             ,
                            threshold           );
};
// ============================================================================


// ============================================================================
/** apply the  misalignemnt to the transformation matrix 
 *  @param ms misalignment matrix (assumed to be small!!!)
 *  @return the resulting transformation matrix
 */
// ============================================================================
const Gaudi::Transform3D& 
PVolume::applyMisAlignment ( const Gaudi::Transform3D& ma ) 
{
  // apply the MisAlingment atop of existing matrix 
  m_matrix = ma * m_matrix ;
  // reset the inverse matrix 
  reset() ;
  // return the resulting matrix  
  return matrix();
};
// ============================================================================

// ============================================================================
/** reset the  misalignemnt 
 *  @return the "nominal" transformation matrix
 */
// ============================================================================
const Gaudi::Transform3D& 
PVolume::resetMisAlignment (                          ) 
{
  // reset *ALL* misalignements 
  m_matrix = m_nominal ;
  // reset the inverse matrix 
  reset         () ;
  // return the resulting matrix  
  return matrix () ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================


