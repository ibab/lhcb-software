

#include <iostream> 
#include <string> 
#include <functional> 
#include <algorithm> 

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/IInspector.h" 

#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/TransientStore.h" 

#include "GaudiKernel/IDataDirectory.h" 
#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/IDataProviderSvc.h" 

#include "GaudiKernel/ISolid.h"
#include "GaudiKernel/ILVolume.h"
#include "GaudiKernel/IPVolume.h"

#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/StreamBuffer.h" 
#include "GaudiKernel/Bootstrap.h" 


#include "DetDesc/LVolume.h"
#include "DetDesc/VolumeIntersectionIntervals.h"
#include "DetDesc/SolidTicks.h"
#include "DetDesc/Material.h"
#include "DetDesc/ISolidFromStream.h"
#include "DetDesc/ClhepToStream.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"


/// local 
#include "PVolume.h"

///
///  not-inlined methods of implemenattion of LVolume class  
///
///  Author: Vanya Belyaev 
///


///
/// standart (default) coinstructor! It is private!!!
///
LVolume::LVolume( )                    // private!!!
  : DataObject        ( "not defined "  )
  , m_lv_solid        (        0        )
  , m_lv_materialName ( " not defined " )
  , m_lv_pvolumes     (                 )
  , m_lv_material     (        0        )
  , m_lv_sdName       (        ""       )
  , m_lv_mfName       (        ""       )
  , m_lv_validSince   (        0        ) 
  , m_lv_validTill    (        0        ) 
  , m_lv_dataSvc      (        0        )
  , m_lv_msgSvc       (        0        )
  , m_lv_svcLoc       (        0        )
{
  ///  Assert( 0 != Solid , " LVolume consructor, pointer to ISolid is NULL " );
  {
    m_lv_validSince = new TimePoint( time_absolutepast );
    m_lv_validTill  = new TimePoint( time_absolutefuture ) ;
  }
  ///
  {
    m_lv_svcLoc = Gaudi::svcLocator(); 
    if( 0 == svcLoc()) { throw LVolumeException(" LVolume()(protected): ISvcLocator* points to NULL!"); } 
    svcLoc()->addRef();
  }
  ///
  {
    const std::string tmp("DetectorDataSvc");
    StatusCode sc = svcLoc()->service( tmp , m_lv_dataSvc );
    if( sc.isFailure() ) { throw LVolumeException( " LVolume()(protected): could not locate IDataProviderSvc="+tmp , 0 , sc ); } 
    if( 0 == dataSvc() ) { throw LVolumeException( " LVolume()(protected): IDataProviderSvc* points to NULL"                ); } 
    dataSvc()->addRef();
  }
  ///
  {
    const std::string tmp("MessageSvc");
    StatusCode sc = svcLoc()->service( tmp , m_lv_msgSvc ); 
    if( sc.isFailure() ) { throw LVolumeException(" LVolume()(protected): could not locate IMessageSvc="+tmp , 0 , sc ); }
    if( 0 == msgSvc()  ) { throw LVolumeException(" LVolume()(protected): IMessageSvc* points to NULL"                ); } 
    msgSvc()->addRef();
  }
  ///
}; 
/// constructor
LVolume::LVolume( const std::string& name        , 
                  ISolid*            Solid       ,
                  const std::string& material    ,
                  const ITime&       validSince  , 
                  const ITime&       validTill   ,
                  const std::string& sdname      ,
                  const std::string& mfname      )
  : DataObject        ( name     )
  , m_lv_solid        ( Solid    )
  , m_lv_materialName ( material )
  , m_lv_pvolumes     (          )
  , m_lv_material     (    0     )
  , m_lv_sdName       ( sdname   )
  , m_lv_mfName       ( mfname   )
  , m_lv_validSince   (    0     ) 
  , m_lv_validTill    (    0     ) 
  , m_lv_dataSvc      (    0     )
  , m_lv_msgSvc       (    0     ) 
  , m_lv_svcLoc       (    0     )
{
  if( 0 == Solid ) { throw LVolumeException("LVolume::LVolume(1), ISolid* points to NULL ") ; }
  ///
  {
    m_lv_validSince = new TimePoint( validSince ) ;
    m_lv_validTill  = new TimePoint( validTill  ) ;
  }
  ///
  {
    m_lv_svcLoc = Gaudi::svcLocator(); 
    if( 0 == svcLoc()) { throw LVolumeException(" LVolume()(1): ISvcLocator* points to NULL!"); } 
    svcLoc()->addRef();
  }
  ///
  {
    const std::string tmp("DetectorDataSvc");
    StatusCode sc = svcLoc()->service( tmp , m_lv_dataSvc );
    if( sc.isFailure() ) { throw LVolumeException( " LVolume()(1): could not locate IDataProviderSvc="+tmp , 0 , sc ); } 
    if( 0 == dataSvc() ) { throw LVolumeException( " LVolume()(1): IDataProviderSvc* points to NULL"       ); } 
    dataSvc()->addRef();
  }
  ///
  {
    const std::string tmp("MessageSvc");
    StatusCode sc = svcLoc()->service( tmp , m_lv_msgSvc ); 
    if( sc.isFailure() ) { throw LVolumeException(" LVolume()(1): could not locate IMessageSvc="+tmp , 0 , sc ); }
    if( 0 == msgSvc()  ) { throw LVolumeException(" LVolume()(1): IMessageSvc* points to NULL"       ); } 
    msgSvc()->addRef();
  }
  ///
}; 
/// 
LVolume::LVolume( const std::string& name        , 
                  ISolid*            Solid       ,
                  const std::string& material    ,
                  const std::string& sdname      ,
                  const std::string& mfname      )
  : DataObject        ( name     )
  , m_lv_solid        ( Solid    )
  , m_lv_materialName ( material )
  , m_lv_pvolumes     (          )
  , m_lv_material     (    0     )
  , m_lv_sdName       ( sdname   )
  , m_lv_mfName       ( mfname   )
  , m_lv_validSince   (    0     ) 
  , m_lv_validTill    (    0     ) 
  , m_lv_dataSvc      (    0     )
  , m_lv_msgSvc       (    0     ) 
  , m_lv_svcLoc       (    0     )
{
  if( 0 == Solid ) { throw LVolumeException("LVolume::LVolume(2), ISolid* points to NULL ") ; }
  ///
  {
    m_lv_validSince = new TimePoint( time_absolutepast    ) ;
    m_lv_validTill  = new TimePoint( time_absolutefuture  ) ;
  }
  ///
  {
    m_lv_svcLoc = Gaudi::svcLocator(); 
    if( 0 == svcLoc()) { throw LVolumeException(" LVolume()(2): ISvcLocator* points to NULL!"); } 
    svcLoc()->addRef();
  }
  ///
  {
    const std::string tmp("DetectorDataSvc");
    StatusCode sc = svcLoc()->service( tmp , m_lv_dataSvc );
    if( sc.isFailure() ) { throw LVolumeException( " LVolume()(2): could not locate IDataProviderSvc="+tmp , 0 , sc  ); } 
    if( 0 == dataSvc() ) { throw LVolumeException( " LVolume()(2): IDataProviderSvc* points to NULL"                 ); } 
    dataSvc()->addRef();
  }
  ///
  {
    const std::string tmp("MessageSvc");
    StatusCode sc = svcLoc()->service( tmp , m_lv_msgSvc ); 
    if( sc.isFailure() ) { throw LVolumeException(" LVolume()(2): could not locate IMessageSvc="+tmp , 0 , sc ); }
    if( 0 == msgSvc()  ) { throw LVolumeException(" LVolume()(2): IMessageSvc* points to NULL"                ); } 
    msgSvc()->addRef();
  }
  ///
}; 
///
bool LVolume::acceptInspector( IInspector* pInspector )
{
  const ILVolume* lv = this; 
  return lv->acceptInspector( pInspector ); 
} 
///
bool LVolume::acceptInspector( IInspector* pInspector ) const 
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  DataObject::acceptInspector( pInspector );  // inspect base class 
  ///
  pInspector->inspectByValue   ( name()             , this , "Name"            ) ;
  pInspector->inspectByRef     ( &m_lv_materialName , this , "MaterialName"    ) ; 
  pInspector->inspectByRef     ( &m_lv_sdName       , this , "Sensitivity"     ) ; 
  pInspector->inspectByRef     ( &m_lv_mfName       , this , "MagProperties"   ) ; 
  pInspector->inspectByRef     (  m_lv_solid        , this , "Solid"           ) ; 
  pInspector->inspectContByRef ( &m_lv_pvolumes     , this , "PhysicalVolumes" ) ;
  ///
  return true; 
  ///
};
/// virtual destructor 
LVolume::~LVolume()
{
  ///
  ILVolume::PVolumes::iterator pvi = pvBegin(); 
  while( pvEnd() != pvi ) { if( 0 != *pvi ) { delete *pvi++; } }  // NB!!!! DELETE!!!! 
  m_lv_pvolumes.clear();
  if( 0 != m_lv_solid      ) { delete m_lv_solid      ; m_lv_solid      = 0 ; } 
  if( 0 != m_lv_validSince ) { delete m_lv_validSince ; m_lv_validSince = 0 ; }
  if( 0 != m_lv_validTill  ) { delete m_lv_validTill  ; m_lv_validTill  = 0 ; }
  ///
};
/// access to the matherial 
Material* LVolume::findMaterial() const 
{
  Material* mat = 0; 
  SmartDataPtr<Material> material( dataSvc() , materialName() );
  if( 0 != material) { mat = material ; }
  Assert( 0 != mat , "LVolumeException:: could not locate material="+materialName() ); 
  m_lv_material = mat;
  return m_lv_material; 
};
/// Serialize the object for reading 
StreamBuffer& LVolume::serialize( StreamBuffer& s )
{
  /// prepare for reading (reset)
  for( ILVolume::PVolumes::iterator pvi = pvBegin() ; pvEnd() != pvi ; ++pvi ) { delete *pvi; } ; 
  m_lv_pvolumes.clear();
  if( 0 != m_lv_solid ) { delete m_lv_solid ; m_lv_solid = 0 ; } 
  ///
  DataObject::serialize( s ); 
  ///
  ISolidFromStream    cnstr ; 
  m_lv_solid        = cnstr( s ) ; // make solid from the stream 
  /// 
  Assert( 0 != m_lv_solid , " Could not read/create ISolid* from StreamBuffer " );   
  ///
  s >> m_lv_materialName ; 
  ///
  s >> m_lv_sdName       ; 
  /// 
  s >> m_lv_mfName       ; 
  /// 
  unsigned long nPV;
  s >> nPV ;  // number of physical volumes 
  ///
  for( unsigned long PV = 0 ; PV < nPV ; ++PV )  
    {
      ///
      std::string      PVname , LVname ;
      HepTransform3D   Mtrx            ;   
      ///
      s >> PVname >> LVname  >> Mtrx   ; 
      ///
      createPVolume( PVname , LVname , Mtrx ) ; 
      ///
    }
  ///
  return s >> m_surfaces(this) ; 
};

/// Serialize the object for writing
StreamBuffer& LVolume::serialize( StreamBuffer& s )  const
{
  DataObject::serialize( s );
  s << *solid        ()   << materialName () << sdName() << mfName() << noPVolumes   (); 
  ///
  for( ILVolume::PVolumes::const_iterator pvi = pvBegin() ; pvEnd() != pvi ; ++pvi )
    { s <<  (*pvi)->name()  <<  (*pvi)->lvolumeName()  << (*pvi)->matrix() ; }  
  ///
  return s << m_surfaces(this);
}
/// pritout to std::ostream /////////////////////////////////////////////////////////////////// 
std::ostream& LVolume::printOut( std::ostream& os ) const
{
  os << " LVolume:: name=" << name() 
     << "\tmaterial="       << materialName() 
     << "\t"                << solid()   
     << "\t#PVolumes="      << noPVolumes  ()
     << std::endl ;
  ///  
  for( ILVolume::PVolumes::const_iterator ci = pvBegin(); pvEnd() != ci ; ++ci ) 
    {
      const IPVolume* ipv = *ci; 
      if( 0 == ipv ) { continue; } 
      const PVolume*  pv  = 0 ; 
      try{ pv = dynamic_cast<const PVolume*>(ipv); }
      catch(...) { pv = 0 ; }
      os << " pv#" << std::setw(3) << ci - pvBegin() << "\t"
         <<  ( pv ? ( os << pv ) : ( os << ipv ) ) 
         << std::endl; 
    } 
  ///
  if( !sdName().empty() ) { os <<  "\tSensitivity=" << sdName() ; } 
  if( !mfName().empty() ) { os <<  "\tMagnetic="    << mfName() ; } 
  ///
  /// surfaces:
  if( !m_surfaces.empty() )
    { 
      os << "\t#Surfaces=" << std::setw(2) << m_surfaces.size() 
         << std::endl;
      for( Surfaces::const_iterator ci = m_surfaces.begin() ; m_surfaces.end() != ci ; ++ci )
        {
          os << "\tsurface# " << std::setw(2) << ci - m_surfaces.begin() 
             << "\t "  << *ci ;
        } 
    }
  ///
  return os;
};
/// pritout to MsgStream ////////////////////////////////////////////////////////////////////// 
MsgStream&    LVolume::printOut( MsgStream&    os ) const
{
  os << " LVolume:: name="  << name() 
     << "\tmaterial="       << materialName() 
     << "\t"                << solid()   
     << "\t#PVolumes="      << noPVolumes  ()
     << endreq    ;
  ///  
  for( ILVolume::PVolumes::const_iterator ci = pvBegin(); pvEnd() != ci ; ++ci ) 
    {
      const IPVolume* ipv = *ci; 
      if( 0 == ipv ) { continue; } 
      const PVolume*  pv  = 0 ; 
      try{ pv = dynamic_cast<const PVolume*>(ipv); }
      catch(...) { pv = 0 ; }
      os << " pv#" << std::setw(3) << ci - pvBegin() << "\t" ;  
      if( 0 != pv ) {  os <<  pv ; } 
      else          {  os << ipv ; }
      os << endreq   ; 
    } 
  ///
  if( !sdName().empty() ) { os <<  "\tSensitivity=" << sdName() ; } 
  if( !mfName().empty() ) { os <<  "\tMagnetic="    << mfName() ; } 
  ///
  /// surfaces:
  if( !m_surfaces.empty() )
    { 
      os << "\t#Surfaces=" << std::setw(2) << m_surfaces.size() 
         << endreq   ;
      for( Surfaces::const_iterator ci = m_surfaces.begin() ; m_surfaces.end() != ci ; ++ci )
        {
          os << "\tsurface# " << std::setw(2) << ci - m_surfaces.begin() 
             << "\t "  << *ci ;
        } 
    }
  ///
  return os;
};
///////////////////////////////////////////////////////////////////////////////////////







