

#include <iostream> 
#include <string> 
#include <functional> 
#include <algorithm> 

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/TimePoint.h" 

#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/TransientStore.h" 

#include "GaudiKernel/IDataDirectory.h" 
#include "GaudiKernel/IDataProviderSvc.h" 

#include "GaudiKernel/ISolid.h"
#include "GaudiKernel/ILVolume.h"
#include "GaudiKernel/IPVolume.h"

#include "GaudiKernel/MsgStream.h" 

#include "DetDesc/LVolume.h"
#include "DetDesc/PVolume.h"
#include "DetDesc/VolumeIntersectionIntervals.h"

#include "DetDesc/SolidTicks.h"
#include "DetDesc/Material.h"


#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Geometry/Transform3D.h"


const CLID& CLID_LogicalVolume = 1100;



///
///  not-inlined methods of implemenattion of LVolume class  
///
///  Author: Vanya Belyaev 



///
/// standart (default) coinstructor! It is private!!!
///
LVolume::LVolume( )                    // private!!!
  : DataObject        ( "not defined "  )
  , m_lv_name         ( "not defined "  )
  , m_lv_solid        (        0        )
  , m_lv_materialName ( " not defined " )
  , m_lv_pvolumes     (                 )
  , m_lv_material     (        0        )
  , m_lv_validSince   (        0        ) 
  , m_lv_validTill    (        0        ) 
  , m_lv_dataSvc      (        0        )
  , m_lv_messSvc      (        0        ) 
{
  ///
  ///  Assert( 0 != Solid , " LVolume consructor, pointer to ISolid is NULL " );
  ///
  m_lv_validSince = new TimePoint( time_absolutepast );
  ///
  m_lv_validTill  = new TimePoint( time_absolutefuture ) ;
  ///
}; 



///
/// constructor
///

LVolume::LVolume( const std::string& name        , 
		  ISolid*            Solid       ,
		  const std::string& material    ,
                  const ITime&       validSince  , 
                  const ITime&       validTill   , 
                  IDataProviderSvc*  dataSvc     ,
                  IMessageSvc*       messSvc     )
  : DataObject        ( name     )
  , m_lv_name         ( name     )
  , m_lv_solid        ( Solid    )
  , m_lv_materialName ( material )
  , m_lv_pvolumes     (          )
  , m_lv_material     (    0     )
  , m_lv_validSince   (    0     ) 
  , m_lv_validTill    (    0     ) 
  , m_lv_dataSvc      ( dataSvc  )
  , m_lv_messSvc      ( messSvc  ) 
{
  Assert( 0 != Solid , "LVolume::LVolume(...), ISolid* points to NULL " );
  //
  m_lv_validSince = new TimePoint( validSince ) ;
  //
  m_lv_validTill  = new TimePoint( validTill  ) ;
  //
}; 


///
/// constructor
///

LVolume::LVolume( const std::string& name        , 
		  ISolid*            Solid       ,
		  const std::string& material    ,
                  IDataProviderSvc*  dataSvc     ,
                  IMessageSvc*       messSvc     )
  : DataObject        ( name     )
  , m_lv_name         ( name     )
  , m_lv_solid        ( Solid    )
  , m_lv_materialName ( material )
  , m_lv_pvolumes     (          )
  , m_lv_material     (    0     )
  , m_lv_validSince   (    0     ) 
  , m_lv_validTill    (    0     ) 
  , m_lv_dataSvc      ( dataSvc  )
  , m_lv_messSvc      ( messSvc  ) 
{
  ///
  Assert( 0 != Solid , "LVolume::LVolume(...), ISolid* points to NULL!" );
  ///
  m_lv_validSince = new TimePoint( time_absolutepast   ) ;
  ///
  m_lv_validTill  = new TimePoint( time_absolutefuture ) ;
  ///
}; 




///
/// virtual destructor 
///

LVolume::~LVolume()
{
  ILVolume::PVolumes::iterator pvi = pvBegin(); 
  while( pvEnd() != pvi ) { if( 0 != *pvi ) { delete *pvi++; } }  // NB!!!! DELETE!!!! 
  m_lv_pvolumes.clear();
  if( 0 != m_lv_solid ) { delete m_lv_solid; m_lv_solid = 0 ; } 
};


///
/// access to the matherial 
///

Material* LVolume::findMaterial()
{
  //
  Material* mat = 0; 
  try   
    { 
      SmartDataPtr<Material> material( dataSvc() , materialName() );
      if( 0 != material) { mat = material.operator->();}
    }
  catch ( const GaudiException& Exception ) 
    { Assert( false , "LVolume::findMaterial(), exception caught! ", Exception ); } 
  catch (...)                      
    { Assert( false , "LVolume::findMaterial(), unknown exception caught! "    ); } 
  ///
  Assert( 0 != mat , "LVolumeException:: could not find material="+materialName() ); 
  ///
  m_lv_material = mat;
  ///
  return m_lv_material; 
};


///
/// technical features:
/// get data service 
///  could throw LVolumeExeption!
IDataProviderSvc* LVolume::dataSvc() 
{
  ///
  if( 0 != m_lv_dataSvc ){ return m_lv_dataSvc; } 
  ///
  DataObject* obj       = dynamic_cast<DataObject*>(this);
  Assert ( 0 != obj   , " LVolume::dataSvc() - DataObject*       points to NULL!" );
  IDataDirectory* dir   = obj->directory();
  Assert ( 0 != dir   , " LVolume::dataSvc() - IDataDirectory*   points to NULL!" );
  TransientStore* store = dir->store() ; 
  Assert ( 0 != store , " LVolume::dataSvc() - TransientStore*   points to NULL!" );
  IDataProviderSvc* svc = store->dataSvc(); 
  Assert ( 0 != store , " LVolume::dataSvc() - IDataProviderSvc* points to NULL!" );
  ///
  m_lv_dataSvc = svc; 
  ///
  return m_lv_dataSvc; 
  ///
};


///
///
/// printout to MsgStream 
///
MsgStream&    LVolume::printOut( MsgStream&    os ) const
{
  os << " LVolume:: name=" << fullpath    () 
     << " material="       << materialName() 
     << " solid=("         << solid       () << ")" 
     << " #PVolumes="      << noPVolumes  ()
     << "(" ;
  for( ILVolume::PVolumes::const_iterator ci = pvBegin(); pvEnd() != ci ; ++ci ) 
    {
      const IPVolume* ipv = *ci; 
      if( 0 == ipv ) { continue; } 
      const PVolume*  pv  = 0 ; 
      try{ pv = dynamic_cast<const PVolume*>(ipv); }
      catch(...) { pv = 0 ; }
      if( 0 != pv ) {  os <<  pv << ";"; } 
      else          {  os << ipv << ";"; } 
    } 
  os << ")";
  //
  return os; 
};
///
///
///


///
///
///
#include "DetDesc/LVolume.createPVolume.h" 
#include "DetDesc/LVolume.createMultiPVolume.h" 
#include "DetDesc/LVolume.intersectLine.h" 
#include "DetDesc/LVolume.validity.h" 



