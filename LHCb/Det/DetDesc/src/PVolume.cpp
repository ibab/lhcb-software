
#include <cassert>
#include <iostream>
#include <string> 
#include <vector>


//
//
//  Implementation of PVolume class
//
//

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataDirectory.h"


#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"

#include "GaudiKernel/MsgStream.h" 

#include "GaudiKernel/SmartDataPtr.h" 


#include "GaudiKernel/TransientStore.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"

//
// constructor 
//

#include "DetDesc/PVolume.h"


PVolume::PVolume           ( const     std::string&     PhysVol_name       ,
			     const     std::string&     LogVol_name        ,
			     const     HepPoint3D&      Position           , // position in Mother Reference Frame!
			     const     HepRotation&     Rotation           , // rotation with respect to Mother Reference Frame
                             IDataProviderSvc*          dataService        ,
                             IMessageSvc*               msgSvc )
  : m_pv_name      ( PhysVol_name   ) 
  , m_pv_lvname    ( LogVol_name    ) 
  , m_pv_matrix    (                )
  , m_pv_lvolume   ( 0              )
  , m_pv_dataSvc   ( dataService    )
  , m_pv_messageSvc( msgSvc         )
{
  // NB!!! transformaion is given by Translation and then Rotation!!!
  Hep3Vector translation = -1.0 * Position;
  //  m_pv_matrix = HepRotate3D(Rotation)*HepTranslate3D(translation ) ;
  m_pv_matrix = HepTranslate3D(translation)*HepRotate3D(Rotation);
  //
  Assert( 0 != dataService , "PVolume Fatal Error in constructor(1)!" );
  //
};

PVolume::PVolume           ( const     std::string&     PhysVol_name       ,
			     const     std::string&     LogVol_name        ,
			     const     HepTransform3D&  Transform          , // position in Mother Reference Frame!
                             IDataProviderSvc*          dataService        ,
                             IMessageSvc*               msgSvc )
  : m_pv_name      ( PhysVol_name   ) 
  , m_pv_lvname    ( LogVol_name    ) 
  , m_pv_matrix    ( Transform      )
  , m_pv_lvolume   ( 0              )
  , m_pv_dataSvc   ( dataService    )
  , m_pv_messageSvc( msgSvc         )
{
  //
  Assert( 0 != dataService , "PVolume Fatal Error in constructor(2) !" );
  //
};

//
//
// destructor!
PVolume::~PVolume(){};

//
//
//
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

//



