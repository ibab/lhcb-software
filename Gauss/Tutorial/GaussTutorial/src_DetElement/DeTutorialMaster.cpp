// $Id: $
// Include files 



// local
#include "DeTutorialMaster.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DeTutorialMaster
//
// 2012-02-13 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeTutorialMaster::DeTutorialMaster( const std::string& name ) 
  :DetectorElement     ( name       ) {

}
//=============================================================================
// Destructor
//=============================================================================
DeTutorialMaster::~DeTutorialMaster() {} 

//=============================================================================
StatusCode DeTutorialMaster::initialize()
{
  StatusCode sc = DetectorElement::initialize();
  if(sc.isFailure() ) {return sc;}
  MsgStream msg( msgSvc(), m_TutorialDet );


  return sc;
  
}
void DeTutorialMaster::FindGlobalPoint (const double dx, const double dy, const double dz ){
  
  const IGeometryInfo* geometry = this->geometry() ;
  
  Gaudi::XYZPoint localPoint(dx, dy, dz);
  
  Gaudi::XYZPoint globalPoint = geometry->toGlobal(localPoint);
  
  MsgStream msg1( msgSvc(), name()+"Master info" );
  msg1<<MSG::INFO<<" Tutorial Master localPoint GlobalPoint "<< localPoint<<"  "<< globalPoint<<endreq;  
  
}
