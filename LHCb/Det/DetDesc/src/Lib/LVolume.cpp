// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.12  2001/08/10 16:41:29  ibelyaev
// modifitcations in IDetectorElement and related classes
//
// Revision 1.11  2001/08/09 16:48:01  ibelyaev
// update in interfaces and redesign of solids
// 
// ===========================================================================
///@{
/** STD & STL includes */ 
#include <iostream> 
#include <string> 
#include <functional> 
#include <algorithm> 
///@} 
///@{
/** CLHEP includes */ 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"
///@} 
///@{ 
/** Gaudi Kernel includes */
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/StreamBuffer.h" 
#include "GaudiKernel/SmartRefVector.h" 
///@} 
///@{
/** DetDesc includes */ 
#include "DetDesc/ISolid.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/LVolume.h"
#include "DetDesc/VolumeIntersectionIntervals.h"
#include "DetDesc/SolidTicks.h"
#include "DetDesc/Material.h"
#include "DetDesc/Solid.h"
#include "DetDesc/Surface.h"
#include "DetDesc/ClhepToStream.h"
#include "DetDesc/DetDesc.h"
///@}
/// local 
#include "PVolume.h"

// ===========================================================================
/** @file LVolume.cpp 
 *  
 *  implementation of class LVolume 
 *
 *  @author: Vanya Belyaev  Ivan.Belyaev@itep.ru 
 */
// ===========================================================================

/// initialize the static reference ocunter 
unsigned long LVolume::s_count = 0;

// ===========================================================================
/** @defgroup Constructors
 *  Constructors and destructor  
 *  @{ 
 */
// ===========================================================================
LVolume::LVolume( const std::string& name        , 
                  ISolid*            Solid       ,
                  const std::string& material    ,
                  const ITime&       validSince  , 
                  const ITime&       validTill   ,
                  const std::string& sdname      ,
                  const std::string& mfname      )
  : DataObject        ( name     )
  , m_lv_solid        ( Solid    )
  , m_lv_pvolumes     (          )
  , m_lv_surfaces     (          ) 
  , m_lv_materialName ( material )
  , m_lv_material     (    0     )
  , m_lv_sdName       ( sdname   )
  , m_lv_mfName       ( mfname   )
  , m_lv_validSince   (    0     ) 
  , m_lv_validTill    (    0     ) 
{
  //  if( 0 == Solid ) 
  //  { throw LVolumeException("LVolume: ISolid* points to NULL ") ; }
  ///
  m_lv_validSince = new TimePoint( validSince ) ;
  m_lv_validTill  = new TimePoint( validTill  ) ;
  ///
  addRef();
}; 

// ===========================================================================
// =========================================================================== 
LVolume::LVolume( const std::string& name        , 
                  ISolid*            Solid       ,
                  const std::string& material    ,
                  const std::string& sdname      ,
                  const std::string& mfname      )
  : DataObject        ( name     )
  , m_lv_solid        ( Solid    )
  , m_lv_pvolumes     (          )
  , m_lv_surfaces     (          )
  , m_lv_materialName ( material )
  , m_lv_material     (    0     )
  , m_lv_sdName       ( sdname   )
  , m_lv_mfName       ( mfname   )
  , m_lv_validSince   (    0     ) 
  , m_lv_validTill    (    0     ) 
{
  // if( 0 == Solid ) 
  //   { throw LVolumeException("LVolume, ISolid* points to NULL ") ; }
  ///
  m_lv_validSince = new TimePoint( time_absolutepast    ) ;
  m_lv_validTill  = new TimePoint( time_absolutefuture  ) ;
  ///
  addRef();
};

// ===========================================================================
// ===========================================================================
LVolume::LVolume( const std::string& Name )
  : DataObject        ( Name            )
  , m_lv_solid        (        0        )
  , m_lv_pvolumes     (                 )
  , m_lv_surfaces     (                 )
  , m_lv_materialName ( " not defined " )
  , m_lv_material     (        0        )
  , m_lv_sdName       (        ""       )
  , m_lv_mfName       (        ""       )
  , m_lv_validSince   (        0        ) 
  , m_lv_validTill    (        0        ) 
{
  m_lv_validSince = new TimePoint( time_absolutepast );
  m_lv_validTill  = new TimePoint( time_absolutefuture ) ;
  /// 
  addRef();
}; 

// ===========================================================================
/// destructor 
// ===========================================================================
LVolume::~LVolume()
{
  /// reset 
  reset();
  /// delete all physical volumes 
  for( ILVolume::PVolumes::iterator it  = m_lv_pvolumes.begin() ;
       m_lv_pvolumes.end() !=   it ; ++it ) 
    { if( 0 != *it ) { delete *it ; } }
  m_lv_pvolumes.clear();
  m_lv_material = 0 ;
  if( 0 != m_lv_solid      ) { delete m_lv_solid      ; m_lv_solid      = 0 ; } 
  if( 0 != m_lv_validSince ) { delete m_lv_validSince ; m_lv_validSince = 0 ; }
  if( 0 != m_lv_validTill  ) { delete m_lv_validTill  ; m_lv_validTill  = 0 ; }
  ///
  release();
};
/** @} end of group Constructors */

///@{
/** implementation of DataObject methods */
const CLID& LVolume::clID   () const { return classID()    ; } 
const CLID& LVolume::classID()       { return CLID_LVolume ; };
///@}

/** @defGroup ILVolume1 
 *  implementation of some functions from ILVolume interface
 *  @{ 
 */

// ============================================================================
/** traverse the sequence of paths  \n 
 *  transform the sequence of replicas to sequence of  physical volumes 
 *  @param replicaPath replica-Path 
 *  @param volumePath  vector of physical volumes 
 *  @return status code 
 */
// ============================================================================
StatusCode 
LVolume::traverse ( ILVolume::ReplicaPath::const_iterator pathBegin,
                    ILVolume::ReplicaPath::const_iterator pathEnd  ,
                    ILVolume::PVolumePath&                pVolumePath )
{
  /**  optimized implementation
   *   using functional IPVolume_fromReplica(); 
   */
  pVolumePath.clear(); ///< clear/reset the output container 
  IPVolume_fromReplica PV ( this ); ///< create transformer  
  std::transform( pathBegin , 
                  pathEnd   , 
                  std::back_inserter( pVolumePath ) , PV ); 
  /// errors?
  if( pVolumePath.end() !=
      std::find( pVolumePath.begin () , 
                 pVolumePath.end   () , 
                 (IPVolume* ) 0     ) )
    { pVolumePath.clear() ; return StatusCode::FAILURE; } 
  return StatusCode::SUCCESS; 
  
  // /// standard implementation 
  // if( pathBegin ==  pathEnd ){ return StatusCode::SUCCESS; }  
  // ILVolume::ReplicaType replica = *pathBegin;
  // if( replica >= noPVolumes()       ) 
  //   { pVolumePath.clear() ; return StatusCode::FAILURE; } 
  // //
  // IPVolume* pv =  *(m_lv_pvolumes.begin()+replica);
  // if( 0 == pv || 0 == pv->lvolume() ) 
  //   { pVolumePath.clear() ; return StatusCode::FAILURE; }
  // //
  // pVolumePath.push_back( pv );  
  // return pv->lvolume()->traverse( ++pathBegin , pathEnd , pVolumePath ); 
};

// ============================================================================
/** calculate the daughter path containing the Point in Local frame , 
 *  can be VERY slow for complex geometry, 
 *  therefore use the appropriate Level for usage 
 *  @param  LocalPoint point in local reference system of logical volume 
 *  @param  level depth level
 *  @param  volumePath  vector of physical volumes
 *  @return status code 
 */
// ============================================================================
StatusCode
LVolume::belongsTo( const HepPoint3D&        LocalPoint ,
                    const int                Level      , 
                    ILVolume::PVolumePath&   pVolumePath )
{    
  /// check the depth 
  if( 0 == Level ) { return StatusCode::SUCCESS; } 
  /// check the point 
  if( !isInside( LocalPoint ) )         
    { pVolumePath.clear() ; return StatusCode::FAILURE; } 
  /// look for daughters 
  ILVolume::PVolumes::const_iterator ppi = 
    std::find_if( m_lv_pvolumes.begin ()  , 
                  m_lv_pvolumes.end   ()  ,
                  IPVolume_isInside( LocalPoint ) ); 
  if( pvEnd() == ppi ) { return StatusCode::SUCCESS; }
  /// check for the volume 
  IPVolume* pv = *ppi;
  if( 0 == pv || 0 == pv->lvolume() ) 
    { pVolumePath.clear() ; return StatusCode::FAILURE; } 
  /// add volume to the path 
  pVolumePath.push_back( pv ) ;
  /// recursion 
  return pv->lvolume()->belongsTo( pv->matrix() * LocalPoint , 
                                   Level - 1                 , 
                                   pVolumePath               );
};

// ============================================================================
/** calculate the daughter path containing the Point in Local frame , 
 *  can be VERY slow for complex geometry, 
 *  therefore use the appropriate Level for usage 
 *  @param  LocalPoint point in local reference system of logical volume 
 *  @param  level depth level
 *  @param  volumePath  vector of physical volumes
 *  @return status code 
 */
// ============================================================================
StatusCode 
LVolume::belongsTo( const HepPoint3D&        LocalPoint ,
                    const int                Level      , 
                    ILVolume::ReplicaPath&   replicaPath )
{
  /// check for level 
  if( 0 == Level ) { return StatusCode::SUCCESS; } 
  /// check for points  
  if( !isInside( LocalPoint ) )
    { replicaPath.clear() ; return StatusCode::FAILURE; } 
  /// look fopr daughters 
  ILVolume::PVolumes::const_iterator ppi = 
    std::find_if( m_lv_pvolumes.begin() ,
                  m_lv_pvolumes.end  () , 
                  IPVolume_isInside( LocalPoint ) );
  /// the last level?  
  if( pvEnd() == ppi ) { return StatusCode::SUCCESS; } 
  /// check for the volume 
  IPVolume* pv = *ppi;
  if( 0 == pv || 0 == pv->lvolume() ) 
    { replicaPath.clear() ; return StatusCode::FAILURE; } 
  /// get the replica number 
  ILVolume::ReplicaType replica = ppi - pvBegin();
  /// add replica to the path 
  replicaPath.push_back( replica ) ;
  /// recursion 
  return pv->lvolume()->belongsTo( pv->matrix() * LocalPoint , 
                                   Level - 1                 , 
                                   replicaPath               );
};  
// ============================================================================
/** @} end of group ILVolume1 */
// ============================================================================


// ============================================================================
/** defgroup IInstectable1 
 *  Implementation of IInstectable interface
 *  @{ 
 */
// ============================================================================

// ============================================================================
/** Accept the inspector 
 *  @param pI pointer to Inspector 
 *  @return true if inspector is accepted 
 */ 
// ============================================================================
bool LVolume::acceptInspector( IInspector* pI )
{ return 0 != pI ? true : false ; };

// ============================================================================
/** Accept the inspector  (const version) 
 *  @param pI pointer to Inspector 
 *  @return true if inspector is accepted 
 */ 
// ============================================================================
bool LVolume::acceptInspector( IInspector* pI ) const
{ return 0 != pI ? true : false ; };
// ============================================================================
/** @} end of group IInspectable1 */
// ============================================================================

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
LVolume::queryInterface( const InterfaceID& ID , void** ppI ) 
{
  if ( 0 == ppI ) { return StatusCode::FAILURE; }
  *ppI = 0 ;
  if      ( ILVolume::   interfaceID() == ID ) 
    { *ppI = static_cast<ILVolume*>     ( this ) ; } 
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
unsigned long LVolume::addRef  () { return ++s_count; }

// ============================================================================
/** release the interface 
 *  @return reference counter 
 */
// ============================================================================
unsigned long LVolume::release () { return ( 0 < s_count ) ? --s_count : 0 ; }

// ============================================================================
/** @} end of group IInterface1 */
// ============================================================================


// ============================================================================
/** locate material
 *  @exception LVolumeException
 *  @return pointer to material
 */
// ============================================================================
Material* LVolume::findMaterial() const 
{
  Material* mat = 0; 
  SmartDataPtr<Material> material( dataSvc() , materialName() );
  if( 0 != material) { mat = material ; }
  Assert( 0 != mat , "Could not locate material=" + materialName() ); 
  m_lv_material = mat;
  return m_lv_material; 
};

// ============================================================================
/** accessor to 
 *  data service used for retriving of the material 
 *  @return pointer to data service 
 */
// ============================================================================
IDataProviderSvc* LVolume::dataSvc() { return DetDesc::detSvc(); };

// ============================================================================
/** set solid for logical volume
 *  @param solid pointer to ISolid object
 *  @return pointer to ISolid object 
 */
// ============================================================================
const ISolid* LVolume::setSolid( ISolid* solid )
{ 
  if( 0 != m_lv_solid ) { delete m_lv_solid ; m_lv_solid = 0 ; }
  return m_lv_solid = solid ; 
};

// ============================================================================
/** @defgroup PrintOut1 
 *  implementation of printout methods for class ILVolume 
 *  @{ 
 */
// ============================================================================

// ============================================================================
/** printout to STD/STL stream
 *  @param os STD/STL stream
 *  @return reference to the stream
 */
// ============================================================================
std::ostream& LVolume::printOut( std::ostream & os ) const
{
  os << " class LVolume (" << s_count         << ")"  
     << " name='"          << name         () << "'"  
     << " material='"      << materialName () << "'" ;
  if( !sdName().empty() ) { os << " sensDet='"  << sdName() << "'" ; }
  if( !mfName().empty() ) { os << " magField='" << mfName() << "'" ; }
  ///
  os << " #physvols" << m_lv_pvolumes.size() ;
  ///
  os << std::endl ;
  os << " solid={"   << solid() << "}";
  os << std::endl ;
  ///
  for( ILVolume::PVolumes::const_iterator it = 
         m_lv_pvolumes.begin() ; m_lv_pvolumes.end() != it ; ++it )
    {
      os 
        << "#"   <<  it - m_lv_pvolumes.begin() 
        << " "   << (*it) << std::endl ;
    }
  ///
  return os ;}
;

// ============================================================================
/** printout to Gaudi MsgStream stream
 *  @param os Gausi MsgStreamstream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream& LVolume::printOut( MsgStream & os ) const
{
  os << " class LVolume (" << s_count         << ")"  
     << " name='"          << name         () << "'"  
     << " material='"      << materialName () << "'" ;
  if( !sdName().empty() ) { os << " sensDet='"  << sdName() << "'" ; }
  if( !mfName().empty() ) { os << " magField='" << mfName() << "'" ; }
  ///
  os << " #physvols=" << m_lv_pvolumes.size() ;
  ///
  os << endreq ;
  os << " solid={" << solid() << "}";
  os << endreq ;
  ///
  for( ILVolume::PVolumes::const_iterator it = 
         m_lv_pvolumes.begin() ; m_lv_pvolumes.end() != it ; ++it )
    {
      os 
        << "#"   <<  it - m_lv_pvolumes.begin() 
        << " "   << (*it) << endreq ;
    }
  ///
  return os ;

};
// ============================================================================
/** @} end of group PrintOut1 */
// ============================================================================


// ============================================================================
/** @defgroup Serialize1
 *  implementation of serialize methods for class ILVolume 
 *  @{ 
 */
// ============================================================================

// ============================================================================
/** serialization for reading 
 *  - implementation of DataObject method
 *  - implementation of ISerialize interface
 *  @see DataObject
 *  @see ILVolume 
 *  @see ISerialize 
 *  @param s reference to stream buffer 
 *  @return reference to stream buffer 
 */ 
// ============================================================================
StreamBuffer& LVolume::serialize(StreamBuffer& s )
{
  /// reset the logical volume  
  reset();
  /// Serialize the base class  
  DataObject::serialize( s ) ;
  /// Serialize the members 
  /// surfaces 
  s >> m_lv_surfaces( this );
  /// material 
  s >> m_lv_materialName  ;
  m_lv_material = 0 ;
  /// sensitive detector 
  s >> m_lv_sdName ;
  /// magnetic field  
  s >> m_lv_mfName ;
  {
    /// solid 
    if( 0 != m_lv_solid ) { delete m_lv_solid ; m_lv_solid = 0 ; }
    std::string solidType;
    s >> solidType ;
    m_lv_solid = Solid::createSolid( solidType );
    if( 0 == solid() )
      { throw LVolumeException( "Could not instantiate SolidType='"
                                + solidType + "'" ); }
    /// serialize the solid 
    m_lv_solid->serialize( s ) ;
  }
  {
    /// physical volumes
    for( ILVolume::PVolumes::iterator it  = m_lv_pvolumes.begin() ;
         m_lv_pvolumes.end() !=   it ; ++it ) 
      { if( 0 != *it ) { delete *it ; } }
    m_lv_pvolumes.clear();
    unsigned int nPV;
    s >> nPV ;
    while( 0 < nPV-- ) 
      {
        IPVolume* pv = createPVolume();
        pv->serialize( s );
        m_lv_pvolumes.push_back( pv );
      }
  }
  /// return 
  return s ;
};

// ============================================================================
/** serialization for writing 
 *  - implementation of DataObject method
 *  - implementation of ISerialize interface
 *  @see DataObject
 *  @see ILVolume 
 *  @see ISerialize 
 *  @param s reference to stream buffer 
 *  @return reference to stream buffer 
 */ 
// ============================================================================
StreamBuffer& LVolume::serialize( StreamBuffer& s )  const
{
  /// Serialize the base class  
  DataObject::serialize( s ) ;
  /// Serialize the members 
  /// surfaces 
  s << m_lv_surfaces( this );
  /// material 
  s << m_lv_materialName  ;
  /// sensitive detector 
  s << m_lv_sdName ;
  /// magnetic field  
  s << m_lv_mfName ;
  /// solid 
  s << solid()->typeName() ;
  solid()->serialize( s ) ;
  /// physical volumes
  s << m_lv_pvolumes.size() ;
  for( ILVolume::PVolumes::const_iterator it = m_lv_pvolumes.begin() ;
       m_lv_pvolumes.end() !=   it ; ++it ) 
    { (*it)->serialize( s ) ; }
  /// return 
  return s ;  
};

// ============================================================================
/** @} end of group PrintOut1 */
// ============================================================================



