// $Id: LogVolBase.cpp,v 1.3 2001-11-20 15:22:23 sponce Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/11/18 16:08:26  ibelyaev
//  bug fix for Win2K
//
// Revision 1.1  2001/11/18 15:32:45  ibelyaev
//  update for Logical Assemblies
// 
// ============================================================================
// GaudiKernel
#include "GaudiKernel/System.h"
// DetDesc 
#include "DetDesc/DetDesc.h"
#include "DetDesc/LogVolBase.h"
#include "DetDesc/VolumeIntersectionIntervals.h"
// local 
#include "PVolume.h"
#include "SimpleValidity.h"

// ============================================================================
/** @file LogVolBase.cpp
 *
 *  Implementation file for class : LogVolBase
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 17/11/2001 
 */
// ============================================================================

// ============================================================================
/** initialize the static counter (number of existing volumes) 
 */
// ============================================================================
unsigned long LogVolBase::s_volumeCounter = 0 ;

// ============================================================================
/** constructor
 *  @exception LVolumeException wrong paramaters value
 *  @param name name of logical volume 
 *  @param sensitivity  name of sensitive detector object (for simulation)
 *  @param magnetic  nam eof magnetic field object (for simulation)
 */
// ============================================================================
LogVolBase::LogVolBase( const std::string& name        , 
                        const std::string& sensitivity ,
                        const std::string& magnetic    )
  : DataObject (             )
  , m_pvolumes (             )
  , m_surfaces (             ) 
  , m_sdName   ( sensitivity ) 
  , m_mfName   ( magnetic    )
  , m_validity ( 0           )
{
  /// create validity object 
  m_validity = new SimpleValidity();
  /// add volume counter 
  ++s_volumeCounter ;  
};

// ============================================================================
/** constructor
 *  @exception LVolumeException wrong parameters value
 *  @param name name    of logical volume 
 *  @param validSince   begin of validity range 
 *  @param validTill    end of validity range 
 *  @param sensitivity  name of sensitive detector object (for simulation)
 *  @param magnetic     name of magnetic field object (for simulation)
 */
// ============================================================================
LogVolBase::LogVolBase( const std::string& name        , 
                        const ITime&       validSince  , 
                        const ITime&       validTill   , 
                        const std::string& sensitivity ,
                        const std::string& magnetic    )
  : DataObject (             )
  , m_pvolumes (             )
  , m_surfaces (             ) 
  , m_sdName   ( sensitivity ) 
  , m_mfName   ( magnetic    )
  , m_validity ( 0           )
{
  /// create validity object 
  m_validity = new SimpleValidity( validSince , validTill );
  /// add volume counter 
  ++s_volumeCounter ;  
};

// ============================================================================
/** constructor 
 *  @exception LVolumeException wrong parameter value
 *  @param name         name of logical volume 
 *  @param validity     validity object 
 *  @param sensitivity  name of sensitive detector object (for simulation)
 *  @param magnetic     nam eof magnetic field object (for simulation)
 */
// ============================================================================
LogVolBase::LogVolBase( const std::string& name        ,
                        const IValidity  & validity    ,
                        const std::string& sensitivity ,
                        const std::string& magnetic    )
  : DataObject (             )
  , m_pvolumes (             )
  , m_surfaces (             ) 
  , m_sdName   ( sensitivity ) 
  , m_mfName   ( magnetic    )
  , m_validity ( 0           )
{
  /// create validity object
  /// huh! ugly lines??? I think so .. 
  IValidity& val = const_cast<IValidity&> (validity) ;
  m_validity     = new SimpleValidity( val.validSince() , 
                                       val.validTill() );
  /// add volume counter 
  ++s_volumeCounter ;  
};

// ============================================================================
/** destructor 
 */
// ============================================================================
LogVolBase::~LogVolBase()
{ 
  reset();
  /// decrease  volume counter 
  --s_volumeCounter ;
  if( 0 != m_validity ) { delete m_validity ; m_validity = 0 ; }
};

// ============================================================================
/** static accessor to 
 *  data service used for retriving of the material 
 *  @return pointer to data service 
 */
// ============================================================================
IDataProviderSvc* LogVolBase::dataSvc() { return DetDesc::detSvc(); }

// ============================================================================
/** add the reference
 *  @return reference counter 
 */
// ============================================================================
unsigned long LogVolBase::addRef  () { return ++m_refCounter ; }

// ============================================================================
/** release the interface 
 *  @return reference counter 
 */
// ============================================================================
unsigned long LogVolBase::release () 
{ return 0 < --m_refCounter  ? m_refCounter : 0 ; };

// ============================================================================
/** query the interface
 *  @param ID unique interface identifier 
 *  @param ppI placeholder for returned interface
 *  @return status code 
 */
// ============================================================================
StatusCode LogVolBase::queryInterface( const InterfaceID& ID  , 
                                       void**             ppI ) 
{
  if(  0 ==  ppI ) { return StatusCode::FAILURE ; }
  *ppI = 0 ;
  if      ( ID == ILVolume::   interfaceID() ) 
    { *ppI = static_cast<ILVolume*>   ( this ); }
  else if ( ID == ISerialize:: interfaceID() )
    { *ppI = static_cast<ISerialize*> ( this ); }
  else { return StatusCode::FAILURE ; }               ///< RETURN !
  ///
  addRef();
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** traverse the sequence of paths  \n 
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
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** printout to STD/STL stream
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
  if( !sdName   ().empty() ) { os << " sensDet='"  << sdName() << "'"   ; }
  if( !mfName   ().empty() ) { os << " magField='" << mfName() << "'"   ; }
  if( !surfaces ().empty() ) { os << " #surfaces=" << surfaces().size() ; }
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
};

// ============================================================================
/** printout to Gaudi MsgStream stream
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
  if( !sdName   ().empty() ) { os << " sensDet='"  << sdName() << "'"   ; }
  if( !mfName   ().empty() ) { os << " magField='" << mfName() << "'"   ; }
  if( !surfaces ().empty() ) { os << " #surfaces=" << surfaces().size() ; }
  ///
  os << " #physvols" << m_pvolumes.size() ;
  for( ILVolume::PVolumes::const_iterator it = 
         m_pvolumes.begin() ; m_pvolumes.end() != it ; ++it )
    {
      os 
        << "#"   <<  it - m_pvolumes.begin() 
        << " "   << (*it) << endreq ;
    }
  ///
  return os << endreq ;
};

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
StreamBuffer& LogVolBase::serialize( StreamBuffer& s )
{
  /// reset the logical volume  
  reset();
  /// delete volumes 
  while( !pvolumes().empty() ) 
    { delete m_pvolumes.back() ; m_pvolumes.pop_back(); }
  /// Serialize the base class  
  DataObject::serialize( s ) ;
  /// Serialize the members 
  unsigned int npv ;
  s >> npv ;
  while( 0 < npv-- ) 
    { 
      IPVolume* ipv = createPVolume(); 
      s >> *ipv ;
    }
  ///
  return s >> m_surfaces ( this ) 
           >> m_sdName           
           >> m_mfName            ;
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
StreamBuffer& LogVolBase::serialize( StreamBuffer& s )  const
{
  /// serialize the base 
  DataObject::serialize( s ) ;
  /// serialze the members
  s << m_pvolumes.size() ;
  /// serialize pvolumes 
  for( ILVolume::PVolumes::const_iterator it = m_pvolumes.begin() ;
       m_pvolumes.end() != it ; ++it )
    { s << **it ; }
  /// serialise other members 
  return s << m_surfaces ( this ) 
           << m_sdName           
           << m_mfName            ;
};

// ===========================================================================
/** create EMPTY daughter physical volume 
 *  @return pointer to created physical volume  
 */
// ===========================================================================
IPVolume* LogVolBase::createPVolume()
{ return createPVolume("",""); }

// ===========================================================================
/** create daughter physical volume 
 *  @param PVname name of daughter volume 
 *  @param LVnameForPV name of logical volume for the physical volume 
 *  @param position position of logical volume 
 *  @param rotation rotation of logical volume 
 *  @return pointer to created physical volume  
 */
// ===========================================================================
IPVolume* LogVolBase::createPVolume( const std::string&    PVname         , 
                                     const std::string&    LVnameForPV    ,
                                     const HepPoint3D&     Position       ,
                                     const HepRotation&    Rotation       ) 
{
  //
  PVolume* pv = 0; 
  try{ pv = new  PVolume( PVname , LVnameForPV , Position , Rotation ); }
  catch( const GaudiException& Exception ) 
    { Assert( false , "createPVolume() , exception caught! " , Exception ); } 
  catch(...)                        
    { Assert( false , "createPVolume() , unknown exception!"); } 
  /// make
  Assert( 0 != pv , 
          "LVolume::createPVolume, could not create volume " + 
          PVname+"(lv="+LVnameForPV+")" );
  /// 
  m_pvolumes.push_back( pv ); 
  ///  
  return m_pvolumes.back();
};

// ============================================================================
/** create daughter physical volume 
 *  @param PVname      name of daughter volume 
 *  @param LVnameForPV name of logical volume for the physical volume 
 *  @param Transform   tranformation
 *  @return pointer to created physical volume  
 */
// ============================================================================
IPVolume* 
LogVolBase::createPVolume( const std::string&    PVname      , 
                           const std::string&    LVnameForPV ,
                           const HepTransform3D& Transform   )
{
  //
  PVolume* pv = 0; 
  try{ pv = new  PVolume( PVname , LVnameForPV , Transform ); }
  catch( const GaudiException& Exception ) 
    { Assert( false , "createPVolume() , exception caught! " , Exception ); } 
  catch(...)                        
    { Assert( false , "createPVolume() , unknown exception!"); } 
  /// make
  Assert( 0 != pv , 
          "LVolume::createPVolume, could not create volume " + 
          PVname+"(lv="+LVnameForPV+")" );
  /// 
  m_pvolumes.push_back( pv ); 
  ///  
  return m_pvolumes.back();  
};  

// ============================================================================
/** Auxillary method  to calculate intersections with daughters
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
( const HepPoint3D&        Point              , 
  const HepVector3D&       Vector             , 
  ILVolume::Intersections& childIntersections , 
  const ISolid::Tick     & tickMin            , 
  const ISolid::Tick     & tickMax            , 
  const double             Threshold          ) const 
{
  ///
  if( m_pvolumes.empty() ) { return 0; } /// RETURN!!!
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
  std::sort( childIntersections.begin() , 
             childIntersections.end  () , 
             VolumeIntersectionIntervals::CompareIntersections() ); 
  ///
  return childIntersections.size(); 
  ///  
};

// ============================================================================
/** Auxillary method  to calculate intersection with daughters
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
( const HepPoint3D&        Point              , 
  const HepVector3D&       Vector             , 
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
  std::sort( childIntersections.begin() , 
             childIntersections.end  () , 
             VolumeIntersectionIntervals::CompareIntersections() ); 
  ///
  return childIntersections.size(); 
  ///
};

// ============================================================================
// The End 
// ============================================================================
