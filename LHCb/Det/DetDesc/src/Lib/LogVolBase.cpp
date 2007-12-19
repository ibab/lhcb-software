// $Id: LogVolBase.cpp,v 1.19 2007-12-19 09:42:40 ibelyaev Exp $
// ============================================================================
// Incldue files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/System.h"
#include "GaudiKernel/IDataProviderSvc.h"
// =============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/DetDesc.h"
#include "DetDesc/LogVolBase.h"
#include "DetDesc/PVolume.h"
#include "DetDesc/VolumeIntersectionIntervals.h"
// ============================================================================
/** @file LogVolBase.cpp
 *
 *  Implementation file for class : LogVolBase
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author Sebastien Ponce
 *  @author Marco Clemencic
 */
// ============================================================================
/** initialize the static counter (number of existing volumes) 
 */
// ============================================================================
unsigned long LogVolBase::s_volumeCounter = 0 ;
// ============================================================================
/*  constructor
 *  @exception LVolumeException wrong paramaters value
 *  @param name name of logical volume 
 *  @param sensitivity  name of sensitive detector object (for simulation)
 *  @param magnetic  nam eof magnetic field object (for simulation)
 */
// ============================================================================
LogVolBase::LogVolBase( const std::string& /*name*/    , 
                        const std::string& sensitivity ,
                        const std::string& magnetic    )
  : ValidDataObject (             )
  , m_pvolumes   (             )
  , m_surfaces   (             ) 
  , m_sdName     ( sensitivity ) 
  , m_mfName     ( magnetic    )
  , m_services   ( 0           )
{
  // get services
  m_services = DetDesc::services();
  /// add volume counter 
  ++s_volumeCounter ;  
}
// ============================================================================
// destructor 
// ============================================================================
LogVolBase::~LogVolBase()
{ 
  /// decrease  volume counter 
  --s_volumeCounter ;
  // release physical volumes
  for (PVolumes::const_iterator ipv = m_pvolumes.begin();
       ipv != m_pvolumes.end() ; ++ipv ) 
  {
    IPVolume* pv = *ipv ;
    if( 0 != pv ) { delete pv ; }
  }
  m_pvolumes.clear();
  // release services
  m_services->release();
}
// ============================================================================
/* 
 *  accessor to data service used for retriving of the material 
 *  @return pointer to data service 
 */
IDataProviderSvc* LogVolBase::dataSvc() const { return m_services->detSvc(); }
// ============================================================================
/*
 *  accessor to message service
 *  @return pointer to message service 
 */
IMessageSvc* LogVolBase::msgSvc() const { return m_services->msgSvc(); }
// ============================================================================
/*  add the reference
 *  @return reference counter 
 */
// ============================================================================
unsigned long LogVolBase::addRef  () { return DataObject::addRef(); }
// ============================================================================
/*  release the interface 
 *  @return reference counter 
 */
// ============================================================================
unsigned long LogVolBase::release () { return DataObject::release(); }
// ============================================================================
/*  query the interface
 *  @param ID unique interface identifier 
 *  @param ppI placeholder for returned interface
 *  @return status code 
 */
// ============================================================================
StatusCode 
LogVolBase::queryInterface
( const InterfaceID& ID  , 
  void**             ppI ) 
{
  if(  0 ==  ppI ) { return StatusCode::FAILURE ; }
  *ppI = 0 ;
  if      ( ID == ILVolume::   interfaceID() ) 
  { *ppI = static_cast<ILVolume*>   ( this ) ; }
  else if ( ID == IInterface:: interfaceID() )
  { *ppI = static_cast<IInterface*> ( this ) ; }
  else { return StatusCode::FAILURE ; }               ///< RETURN !
  ///
  addRef();
  ///
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  traverse the sequence of paths  \n 
 *  transform the sequence of replicas to sequence of  physical volumes 
 *  @see ILVolume 
 *  @param replicaPath replica-Path 
 *  @param volumePath  vector of physical volumes 
 *  @return status code 
 */
// ============================================================================
StatusCode LogVolBase::traverse 
( ILVolume::ReplicaPath::const_iterator pathBegin   ,
  ILVolume::ReplicaPath::const_iterator pathEnd     ,
  ILVolume::PVolumePath&                pVolumePath ) const 
{
  /*   optimized implementation
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
  ///
  return StatusCode::SUCCESS;
}
// ============================================================================
/*  printout to STD/STL stream
 *  @param os STD/STL stream
 *  @return reference to the stream
 */
// ============================================================================
std::ostream& LogVolBase::printOut( std::ostream & os ) const
{
  os << " " 
     << System::typeinfoName( typeid(*this) )
     << " ("        << s_volumeCounter << ") "
     << " name = '" << name()          << "' " ;
  if ( !sdName   ().empty() ) { os << " sensDet='"  << sdName() << "'"   ; }
  if ( !mfName   ().empty() ) { os << " magField='" << mfName() << "'"   ; }
  if ( !surfaces ().empty() ) { os << " #surfaces=" << surfaces().size() ; }
  ///
  os << " #physvols" << m_pvolumes.size() ;
  for( ILVolume::PVolumes::const_iterator it = 
         m_pvolumes.begin() ; m_pvolumes.end() != it ; ++it )
  {
    os 
      << "#"   <<  it - m_pvolumes.begin() 
      << " "   << (*it) << std::endl ;
  }
  ///
  return os << std::endl ;
}
// ============================================================================
/*  printout to Gaudi MsgStream stream
 *  @param os Gausi MsgStreamstream
 *  @return reference to the stream
 */
// ============================================================================
MsgStream& LogVolBase::printOut( MsgStream & os ) const
{
  os << " " 
     << System::typeinfoName( typeid(*this) )
     << " ("        << s_volumeCounter << ") "
     << " name = '" << name()          << "' " ;
  if ( !sdName   ().empty() ) { os << " sensDet='"  << sdName() << "'"   ; }
  if ( !mfName   ().empty() ) { os << " magField='" << mfName() << "'"   ; }
  if ( !surfaces ().empty() ) { os << " #surfaces=" << surfaces().size() ; }
  ///
  os << " #physvols" << m_pvolumes.size() ;
  for ( ILVolume::PVolumes::const_iterator it = 
          m_pvolumes.begin() ; m_pvolumes.end() != it ; ++it )
  {
    os 
      << "#"   <<  it - m_pvolumes.begin() 
      << " "   << (*it) << endreq ;
  }
  ///
  return os << endreq ;
}
// ===========================================================================
/*  create EMPTY daughter physical volume 
 *  @return pointer to created physical volume  
 */
// ===========================================================================
IPVolume* LogVolBase::createPVolume()
{ return createPVolume("",""); }
// ===========================================================================
//bool LogVolBase::copyExist  ( const size_t copy   ) const 
//{
//  for( PVolumes::const_iterator ipv = m_pvolumes.begin() ; 
//       m_pvolumes.end() != ipv ; ++ipv ) 
//    {
//      const IPVolume* pv = *ipv ;
//      if( 0 == pv ) { continue ; }
//      if( copy == pv->copy() ) { return true ; }     // RETURN 
//    }
//  
//  return false ;
//};
// ===========================================================================

// ===========================================================================
/// deduce the copy number of physical volume from its name 
// ===========================================================================
//size_t LogVolBase::copyNumber( const std::string& pvName ) const
//{
//
//  const size_t copy_number = m_pvolumes.size() ;

// forced copy_number ?
//  const std::string::size_type pos = pvName.find(':') ;
//  if( std::string::npos == pos ) { return copy_number ; }      // RETURN

//  const std::string tmp( pvName  , pos + 1 , std::string::npos );
//  size_t copy_read = atoi( tmp.c_str() );

/// check the validity of the string 
//  Assert( 0 != copy_read  , 
//          "LogVolBase::copyNumber: illegal copy_number '" + tmp + 
//          "' for '" + pvName + "' ( lv='" + name() + "') " ) ;

//  if ( copy_read  < 100000 ) 
//    {
//========== hack to set the output level =================
//      PropertyMgr* pmgr = new PropertyMgr();
//      int outputLevel = -1;
//      pmgr->declareProperty( "OutputLevel", outputLevel );
//      IJobOptionsSvc* jobSvc;
//      m_services->svcLocator()->service("JobOptionsSvc", jobSvc );
//      jobSvc->setMyProperties( "LOGVOL", pmgr );
//   if( -1 != outputLevel ) msgSvc()->setOutputLevel( "LOGVOL", outputLevel );
//      delete pmgr;
//=========================================================
//      MsgStream log( msgSvc() , "LOGVOL" ) ;
//      log << MSG::WARNING
//          << " LogVolBase: copy number ' " 
//          << copy_read << "' deduced from '" 
//          << tmp << "' is ignored! (use forced numbers > 100000) "
//          << " PV='" << pvName << "'" << endreq ;
//      return copy_number ;    
//    }

//  return copy_read ;
//};
// ===========================================================================

// ===========================================================================
/*  create daughter physical volume 
 *  @param PVname name of daughter volume 
 *  @param LVnameForPV name of logical volume for the physical volume 
 *  @param position position of logical volume 
 *  @param rotation rotation of logical volume 
 *  @return pointer to created physical volume  
 */
// ===========================================================================
IPVolume* LogVolBase::createPVolume 
( const std::string&    PVname         , 
  const std::string&    LVnameForPV    ,
  const Gaudi::XYZPoint&     Position  ,
  const Gaudi::Rotation3D&   Rotation    ) 
{
  PVolume* pv = 0; 
  try{ pv = new  PVolume ( PVname      , 
                           LVnameForPV , 
                           //                           copy_number , 
                           Position    , 
                           Rotation    ) ; }
  catch( const GaudiException& Exception ) 
  { Assert( false , "createPVolume() , exception caught! " , Exception ); } 
  catch(...)                        
  { Assert( false , "createPVolume() , unknown exception!"); } 

  if( 0 == pv ) {
    throw LogVolumeException( "LVolume::createPVolume, could not create volume "+
                              PVname+"(lv="+LVnameForPV+")", 
                              this, StatusCode::FAILURE );
  }
  
  m_pvolumes.push_back( pv ); 
  
  return m_pvolumes.back();
}
// ============================================================================
/*  create daughter physical volume 
 *  @param PVname      name of daughter volume 
 *  @param LVnameForPV name of logical volume for the physical volume 
 *  @param Transform   tranformation
 *  @return pointer to created physical volume  
 */
// ============================================================================
IPVolume* 
LogVolBase::createPVolume
( const std::string&    PVname      , 
  const std::string&    LVnameForPV ,
  const Gaudi::Transform3D& Transform   )
{
  PVolume* pv = 0; 
  try{ pv = new  PVolume ( PVname      , 
                           LVnameForPV , 
                           //                         copy_number , 
                           Transform   ) ; }
  catch( const GaudiException& Exception ) 
  { Assert( false , "createPVolume() , exception caught! " , Exception ); } 
  catch(...)                        
  { Assert( false , "createPVolume() , unknown exception!"); } 
  
  if( 0 == pv ) {
    throw LogVolumeException( "LVolume::createPVolume, could not create volume "+
                              PVname+"(lv="+LVnameForPV+")", 
                              this, StatusCode::FAILURE );
  }
  m_pvolumes.push_back( pv ); 
  return m_pvolumes.back();  
}
// ============================================================================
/*  Auxillary method  to calculate intersections with daughters
 *  @exception LVolumeException wrong parameters or geometry error
 *  @param Point initial point at the line
 *  @param Vector direction vector of the line
 *  @param intersections output container
 *  @param tickMin minimum value of possible Tick
 *  @param tickMax maximum value of possible Tick
 *  @param threshold threshold value 
 *  @return number of intersections  
 */
// ============================================================================
unsigned int  
LogVolBase::intersectDaughters
( const Gaudi::XYZPoint&        Point              , 
  const Gaudi::XYZVector&       Vector             , 
  ILVolume::Intersections& childIntersections , 
  const ISolid::Tick     & tickMin            , 
  const ISolid::Tick     & tickMax            , 
  const double             Threshold          ) const 
{
  ///
  if ( m_pvolumes.empty() ) { return 0; } /// RETURN!!!
  ///
  ILVolume::Intersections child; 
  for( ILVolume::PVolumes::const_iterator ipv = m_pvolumes.begin() ; 
       m_pvolumes.end() != ipv ; ++ipv ) 
  { 
    IPVolume* pv = *ipv; 
    /// construct the intersections container for each daughter volumes 
    if( 0 != pv ) 
    { pv->intersectLine( Point     , 
                         Vector    , 
                         child     , 
                         tickMin   , 
                         tickMax   , 
                         Threshold ) ; }
    else 
    { Assert( false , 
              "LVolume::intersect line, IPVolume==NULL for " + name() ); }
    
    /// merge individidual containers and clear child container 
    std::copy( child.begin() , 
               child.end  () , 
               std::back_inserter( childIntersections ) ) ; 
    child.clear();
  }
  ///
  std::stable_sort
    ( childIntersections.begin() , 
      childIntersections.end  () , 
      VolumeIntersectionIntervals::CompareIntersections() ); 
  
  /// V.B.: try to correct 
  VolumeIntersectionIntervals::correct 
    ( this , childIntersections , Vector.R() ) ;

  return childIntersections.size(); 
  ///  
}
// ============================================================================
/*  Auxillary method  to calculate intersection with daughters
 *  @exception LVolumeException wrong parameters or geometry error
 *  @param Point initial point at the line
 *  @param Vector direction vector of the line
 *  @param intersections output container
 *  @param threshold threshold value 
 *  @return number of intersections  
 */
// ============================================================================
unsigned int  
LogVolBase::intersectDaughters
( const Gaudi::XYZPoint&        Point              , 
  const Gaudi::XYZVector&       Vector             , 
  ILVolume::Intersections& childIntersections , 
  const double             Threshold          ) const
{
  ///
  if( m_pvolumes.empty() ) { return 0; } /// RETURN!!!
  ///
  ILVolume::Intersections child; 
  for( ILVolume::PVolumes::const_iterator ipv = m_pvolumes.begin() ; 
       m_pvolumes.end() != ipv ; ++ipv  ) 
  { 
    const IPVolume* pv = *ipv; 
    /// construct the intersections container for each daughter volumes 
    if( 0 != pv ) 
    { pv->intersectLine( Point     , 
                         Vector    , 
                         child     , 
                         Threshold ) ;}
    else 
    { Assert( false , 
              "LogVolBase::intersectDaug, IPVolume==NULL for " +
              name() ); }
    /// merge individidual containers and clear child container 
    std::copy( child.begin() , 
               child.end  () , 
               std::back_inserter( childIntersections ) ) ; 
    child.clear();
  }
  ///
  std::stable_sort
    ( childIntersections.begin() , 
      childIntersections.end  () , 
      VolumeIntersectionIntervals::CompareIntersections() ); 
  ///

  /// V.B.: try to correct 
  VolumeIntersectionIntervals::correct 
    ( this , childIntersections , Vector.R() ) ;

  return childIntersections.size(); 
  ///
}
// ============================================================================
// The End 
// ============================================================================
