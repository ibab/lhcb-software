/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.9  2001/08/10 16:41:29  ibelyaev
/// modifitcations in IDetectorElement and related classes
///
/// Revision 1.8  2001/08/09 16:48:01  ibelyaev
/// update in interfaces and redesign of solids
/// 
/// ===========================================================================
///@{ 
/** STD & STL includes */  
#include <cassert>
#include <iostream>
#include <string> 
#include <vector>
///@}
///@{ 
/** CLHEP includes */
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
///@}
///@{ 
/** GaudiKernel includes */
#include "GaudiKernel/IInspector.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/StreamBuffer.h"
///@}
/// DetDesc 
#include "DetDesc/DetDesc.h"
///@{ 
/** local includes */ 
#include "PVolume.h"
#include "PVolumeException.h"
///@}

// ============================================================================
/** @file PVolume.cpp 
 *  
 * implementation of class PVolume 
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date xx/xx/xxxx
 */
// ============================================================================

unsigned long PVolume::s_count = 0;

// ============================================================================
/** @defgroup Create1 
 *  Constructors and destructor for class PVolume 
 *  @{ 
 */
// ============================================================================

// ============================================================================
/** constructor    
 *  @param PhysVol_name name of phys volume 
 *  @param LogVol_name name of log volume 
 *  @param Position position of physical volume inside mother volume 
 *  @param Rotation  rotation of physical volume with respect to mother 
 */
// ============================================================================
PVolume::PVolume ( const std::string& PhysVol_name  ,
                   const std::string& LogVol_name   ,
                   const HepPoint3D&  Position      ,
                   const HepRotation& Rotation      )
  : m_pv_name      ( PhysVol_name   ) 
  , m_pv_lvname    ( LogVol_name    ) 
  , m_pv_matrix    (                )
  , m_pv_imatrix   ( 0              )
  , m_pv_lvolume   ( 0              )
{
  // NB!!! transformaion is given by Translation and then Rotation!!!
  m_pv_matrix = HepRotate3D(Rotation)*HepTranslate3D(Position) ;
  ///
  addRef();
};

// ============================================================================
/** constructor    
 *  @param PhysVol_name name of phys volume 
 *  @param LogVol_name name of log volume 
 *  @param Transform transformation matrix 
 */
// ============================================================================
PVolume::PVolume ( const std::string&    PhysVol_name ,
                   const std::string&    LogVol_name  ,
                   const HepTransform3D& Transform    )
  : m_pv_name      ( PhysVol_name   ) 
  , m_pv_lvname    ( LogVol_name    ) 
  , m_pv_matrix    ( Transform      )
  , m_pv_imatrix   ( 0              )
  , m_pv_lvolume   ( 0              )
{ addRef(); };

// ============================================================================
// destructor 
// ============================================================================
PVolume::~PVolume() { reset(); release() ; };

// ============================================================================
///@}  end of group Create1 
// ============================================================================

// ============================================================================
/** find logical volume by name 
 *  @return pointet to logical volume 
 */
// ============================================================================
ILVolume*         PVolume::findLogical() const 
{
  m_pv_lvolume = 0 ;
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
  m_pv_lvolume = lv ;
  ///
  return m_pv_lvolume;
};

// ============================================================================
/** inverse the matrix
 *  @return pointer to inverse matrix 
 */
// ============================================================================
HepTransform3D* PVolume::findMatrix() const 
{
  if( 0 != m_pv_imatrix ) { delete m_pv_imatrix ; m_pv_imatrix = 0 ; }
  return m_pv_imatrix = new HepTransform3D( matrix().inverse() ) ;
};


// ============================================================================
// ============================================================================
bool PVolume::acceptInspector( IInspector* pInspector ) 
{
  return 0 == pInspector ? false : true ;
};

// ============================================================================
// ============================================================================
bool PVolume::acceptInspector( IInspector* pInspector ) const 
{
  return 0 == pInspector ? false : true ;
};

// ============================================================================
/** the static accessor to the data service
 *  @return pointer to data service 
 */
// ============================================================================
IDataProviderSvc* PVolume::dataSvc() { return DetDesc::detSvc(); }

// ============================================================================
/** @defgroup IInterface1 
 *  implementations of vurtual functions from class IInterface 
 *  @see IInterface
 *  @see ILVolume 
 *  @{ 
 */
// ===========================================================================

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
/** add the reference
 *  @return reference counter 
 */
// ============================================================================
unsigned long PVolume::addRef  () { return ++s_count; }

// ============================================================================
/** release the interface 
 *  @return reference counter 
 */
// ============================================================================
unsigned long PVolume::release () { return ( 0 < s_count ) ? --s_count : 0 ; }

// ============================================================================
/** @} end of group IInterface1 */
// ============================================================================

// ============================================================================
/** @defgroup PrintOut1 
 *  implementation of printout methods from class PVolume 
 *  @{ 
 */
// ============================================================================

// ============================================================================
/** printout to STD/STL stream 
 *  @param os reference to STD/STL stream 
 *  @return reference to STD/STL stream 
 */
// ============================================================================
std::ostream& PVolume::printOut( std::ostream& os ) const
{
  return 
  os << " class PVolume (" << s_count       << ")"
     << " [" 
     << " name='"          << name()        << "'" 
     << " logvol='"        << lvolumeName() << "'" << "]";
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
    os << " class PVolume (" << s_count       << ")"
       << " [" 
       << " name='"          << name()        << "'" 
       << " logvol='"        << lvolumeName() << "'" << "]";
};
// ===========================================================================
///@} 
// ============================================================================


// ============================================================================
/** @defgroup Serialize1 
 *  implementation of serialize methods from class PVolume 
 *  @{ 
 */
// ============================================================================

// ===========================================================================
/** serialization for reading 
 *  - implementation of DataObject method
 *  - implementation of ISerialize interface
 *  @see DataObject
 *  @see IPVolume 
 *  @see ISerialize 
 *  @param s reference to stream buffer 
 *  @return reference to stream buffer 
 */ 
// ===========================================================================
StreamBuffer& PVolume::serialize(StreamBuffer& s )
{
  reset();
  s >> m_pv_name  
    >> m_pv_lvname ;
  double     angle ;
  Hep3Vector axis  ; 
  s >> angle 
    >> axis[0] 
    >> axis[1] 
    >> axis[2] ;
  HepRotation rot( HepRotation().rotate( angle , axis ) );
  Hep3Vector pos ;
  s >> pos[0] 
    >> pos[1] 
    >> pos[2] ;
  m_pv_matrix = HepTransform3D( rot , pos ) ;
  return s;
};

// ===========================================================================
/** serialization for writing 
 *  - implementation of DataObject method
 *  - implementation of ISerialize interface
 *  @see DataObject
 *  @see ILVolume 
 *  @see ISerialize 
 *  @param s reference to stream buffer 
 *  @return reference to stream buffer 
 */ 
// ===========================================================================
StreamBuffer& PVolume::serialize(StreamBuffer& s )  const
{
  s << m_pv_name  
    << m_pv_lvname ;
  HepRotation rot = matrix().getRotation    () ;
  double angle   = 0 ;
  Hep3Vector axis;
  rot.getAngleAxis( angle , axis ) ;
  s << angle 
    << axis.x()
    << axis.y()
    << axis.z() ;
  Hep3Vector pos = matrix().getTranslation () ;
  s << pos.x()
    << pos.y() 
    << pos.z() ;
  ///
  return s;
};
// ===========================================================================
///@} 
// ============================================================================


// ============================================================================
/** @defgroup Assertions1 
 *  assertion methods for class PVolume 
 *  @{ 
 */ 
// ============================================================================

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
///@} end of group Assertions1 
// ============================================================================


