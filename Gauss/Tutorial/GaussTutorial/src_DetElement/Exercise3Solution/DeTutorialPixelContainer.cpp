// $Id: $
// Include files 



// local
#include "DeTutorialPixelContainer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DeTutorialPixelContainer
//
// 2012-02-13 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeTutorialPixelContainer::DeTutorialPixelContainer(const std::string& name   ) 
  :DetectorElement (name)   {

}
//=============================================================================
// Destructor
//=============================================================================
DeTutorialPixelContainer::~DeTutorialPixelContainer() {} 

//=============================================================================
StatusCode DeTutorialPixelContainer::initialize() 
{
  
  StatusCode sc = DetectorElement::initialize();
  if(sc.isFailure() ) {return sc;}
  MsgStream msg( msgSvc(), m_TutorialDetPC );


  return sc;
  
}

void DeTutorialPixelContainer::FindGlobalPointFromPixelContainer (const double dx, const double dy, const double dz ){
  
 const IGeometryInfo* geometry = this->geometry() ;
 
  Gaudi::XYZPoint localPoint(dx, dy, dz);

  Gaudi::XYZPoint globalPoint = geometry->toGlobal(localPoint);
  MsgStream msg1( msgSvc(), name()+"Master info" );
  msg1<<MSG::INFO<<" Tutorial Pixel Container localPoint GlobalPoint "<< localPoint<<"  "<< globalPoint<<endreq;
  
}
