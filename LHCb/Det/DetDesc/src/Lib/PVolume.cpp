
#include <cassert>
#include <iostream>
#include <string> 
#include <vector>

#include "GaudiKernel/IInspector.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/TransientStore.h"
#include "GaudiKernel/Bootstrap.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"

/// local 
#include "PVolume.h"
#include "PVolumeException.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////
PVolume::PVolume           ( const     std::string&     PhysVol_name       ,
			     const     std::string&     LogVol_name        ,
			     const     HepPoint3D&      Position           , // position in Mother Reference Frame!
			     const     HepRotation&     Rotation           ) // rotation with respect to Mother Reference Frame
  : m_pv_name      ( PhysVol_name   ) 
  , m_pv_lvname    ( LogVol_name    ) 
  , m_pv_matrix    (                )
  , m_pv_lvolume   ( 0              )
  , m_pv_dataSvc   ( 0              )
{
  // NB!!! transformaion is given by Translation and then Rotation!!!
  Hep3Vector translation = -1.0 * Position;
  m_pv_matrix = HepRotate3D(Rotation)*HepTranslate3D(translation ) ;
  //m_pv_matrix = HepTranslate3D(translation)*HepRotate3D(Rotation);
  //
  ///
  {
    ISvcLocator* svcLoc = Gaudi::svcLocator(); 
    if( 0 == svcLoc ) { throw PVolumeException(" PVolume()(protected): ISvcLocator* points to NULL!"); } 
    svcLoc->addRef();
    ///
    const std::string tmp("DetectorDataSvc");
    StatusCode sc = svcLoc->service( tmp , m_pv_dataSvc );
    if( sc.isFailure() ) { throw PVolumeException( " LVolume(): could not locate IDataProviderSvc="+tmp ); } 
    if( 0 == dataSvc() ) { throw PVolumeException( " LVolume(): IDataProviderSvc* points to NULL"       ); } 
    dataSvc()->addRef();
  }
  ///
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PVolume::PVolume           ( const     std::string&     PhysVol_name       ,
			     const     std::string&     LogVol_name        ,
			     const     HepTransform3D&  Transform          ) // position in Mother Reference Frame!
  : m_pv_name      ( PhysVol_name   ) 
  , m_pv_lvname    ( LogVol_name    ) 
  , m_pv_matrix    ( Transform      )
  , m_pv_lvolume   ( 0              )
  , m_pv_dataSvc   ( 0              )
{
  ISvcLocator* svcLoc = Gaudi::svcLocator(); 
  if( 0 == svcLoc ) { throw PVolumeException(" PVolume()(protected): ISvcLocator* points to NULL!"); } 
  svcLoc->addRef();
  ///
  const std::string tmp("DetectorDataSvc");
  StatusCode sc = svcLoc->service( tmp , m_pv_dataSvc );
  if( sc.isFailure() ) { throw PVolumeException( " LVolume(): could not locate IDataProviderSvc="+tmp ); } 
  if( 0 == dataSvc() ) { throw PVolumeException( " LVolume(): IDataProviderSvc* points to NULL"       ); } 
  dataSvc()->addRef();
};
// destructor! //////////////////////////////////////////////////////////////////////////////////////////////////////////
PVolume::~PVolume(){};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
ILVolume*         PVolume::findLogical() const 
{
  ILVolume* lv = 0;
  try
    { 
      SmartDataPtr<ILVolume> ptr( dataSvc() , lvolumeName() );
      if( 0 != ptr ) { lv = ptr.operator->(); }
    }
  catch( const GaudiException& Exception ) 
    { Assert( false , " PVolume::findLogical(), exception caught! " , Exception ) ; } 
  catch( const std::exception& Exception ) 
    { Assert( false , " PVolume::findLogical(), std::exception caught! "+std::string(Exception.what())) ; }   
  catch(...)                    
    { Assert( false , " PVolume::findLogical(), unknown exception caught! "     ) ; } 
  ///
  Assert( 0 != lv , " PVolume::findLogical, unable to locate LV="+lvolumeName() );
  ///
  m_pv_lvolume = lv ;
  ///
  return m_pv_lvolume;
};
/// inspection /////////////////////////////////////////////////////////////////////////////////////////////
bool PVolume::acceptInspector( IInspector* pInspector ) 
{
  const IPVolume* pv = this ;
  return pv->acceptInspector( pInspector ); 
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PVolume::acceptInspector( IInspector* pInspector ) const 
{
  ///
  if( 0 == pInspector ) { return false; } 
  ///
  pInspector->inspectByRef( &m_pv_name   , this , "PhysVolumeName"    ) ;
  pInspector->inspectByRef( &m_pv_lvname , this , "LogicalVolumeName" ) ;
  ///
  return true; 
  ///
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////







