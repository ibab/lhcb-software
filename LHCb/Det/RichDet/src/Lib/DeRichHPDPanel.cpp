// $Id: DeRichHPDPanel.cpp,v 1.1 2002-07-16 16:02:41 papanest Exp $
#define DERICHHPDPANEL_CPP

// Include files
#include "RichDet/DeRichHPDPanel.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

// CLHEP files
#include "CLHEP/Units/SystemOfUnits.h"

/// Detector description classes
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h" 
#include "DetDesc/SolidTubs.h" 
#include "DetDesc/SolidSphere.h" 



//----------------------------------------------------------------------------
//
// Implementation of class :  DeRichHPDPanel
//
//----------------------------------------------------------------------------

const CLID& CLID_DeRichHPDPanel = 12010;  // User defined

// Standard Constructor
DeRichHPDPanel::DeRichHPDPanel()
{}
  
// Standard Destructor
DeRichHPDPanel::~DeRichHPDPanel() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichHPDPanel::classID() {
  return CLID_DeRichHPDPanel;
}


StatusCode DeRichHPDPanel::initialize() {

  StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "DeRichHPDPanel" );
  log << MSG::DEBUG <<"Starting initialisation of DeRichHPDPanel"<< endreq;
  this->printOut(log);
  
  pixelSize = 0.5*mm;
  HepPoint3D zero(0.0, 0.0, 0.0);
  

  // get the first HPD and follow down to the silicon block
  const IPVolume* pvHPDMaster0 = geometry()->lvolume()->pvolume(0);
  const IPVolume* pvHPDSMaster0 = pvHPDMaster0->lvolume()->pvolume(0);
  const IPVolume* pvSilicon0 = pvHPDSMaster0->lvolume()->pvolume(4);

  const ISolid* siliconSolid = pvSilicon0->lvolume()->solid();
  const SolidBox* siliconBox = dynamic_cast<const SolidBox*>(siliconSolid);

  // assume same size for all silicon detectors
  siliconHalfLengthX = siliconBox->xHalfLength();
  siliconHalfLengthY = siliconBox->yHalfLength();

  // HPD #0 coordinates
  HepPoint3D HPD0centre = pvHPDMaster0->toMother(zero);

  //get the next HPD (same column row above) and the row pitch
  const IPVolume* pvHPDMaster1 = geometry()->lvolume()->pvolume(1);
  HepPoint3D HPD1centre = pvHPDMaster1->toMother(zero);
  rowPitch = HPD1centre.y() - HPD0centre.y();
  
  //get the HPD at next column and the column pitch
  const IPVolume* pvHPDMaster16 = geometry()->lvolume()->pvolume(16);
  HepPoint3D HPD16centre = pvHPDMaster16->toMother(zero);
  columnPitch = HPD16centre.x() - HPD0centre.x();
  
  const ISolid* HPDSolid = pvHPDMaster0->lvolume()->solid();
  
  const SolidTubs* HPDTube=dynamic_cast<const SolidTubs*>(HPDSolid);
  double HPDRadius =  HPDTube->innerRadius();

  //get the leftmost panel point
  panelLeft = - (HPD0centre.x() - HPDRadius);
  
  //get the bottom-most points for the two types of columns
  panelBottomEven = - (HPD0centre.y() - HPDRadius);
  panelBottomOdd = - (HPD16centre.y() - HPDRadius);

  log << MSG::DEBUG << "HPD panel column pitch: " << columnPitch
      <<" row pitch: " << rowPitch << endreq;
  
  // get the pv and the solid for the HPD quartz window
  const IPVolume* pvWindow0 = pvHPDSMaster0->lvolume()->pvolume(2);
  const ISolid* windowSolid0 = pvWindow0->lvolume()->solid();
  const SolidSphere* windowSphere0 = dynamic_cast<const SolidSphere*>
    (windowSolid0);
  double windowRadius = windowSphere0->outerRadius();
  
  // get the coordinate of the centre of the HPD quarz window
  HepPoint3D HPDTop1(0.0, 0.0, windowRadius);
  // convert this to HPD coordinates
  HepPoint3D HPDTop2 = pvWindow0->toMother(HPDTop1);
  // and to silicon
  HPDTop = pvSilicon0->toLocal(HPDTop2);

  log << MSG::DEBUG <<"Finished initialisation for DeRichHPDPanel"<< endreq;

  return sc;
}

// ===========================================================================

StatusCode DeRichHPDPanel::smartID (const HepPoint3D& globalPoint,
                                    RichSmartID& id) 
{  
  MsgStream log(msgSvc(), "DeRichHPDPanel" );

  if (!geometry()->isInside(globalPoint)) {
    log << MSG::ERROR << "Point outside HPD Panel" <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }

  HepPoint3D inPanel = geometry()->toLocal(globalPoint);

  // HPD row and column, pixel row and column as well as HPD panel number
  // and rich id starts at 1 and not 0

  // find the correct HPD inside the panel
  unsigned int HPDColumn = (unsigned int) ((inPanel.x() + panelLeft) /
                                           columnPitch) + 1;

  //HPD row and colums max is hard-coded for the time being
  if (HPDColumn > 9) {
    log << MSG::ERROR << "HPD column number too high" <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }
  
  unsigned int HPDRow;
  if (0 == HPDColumn%2) {
    HPDRow =  (unsigned int) ((inPanel.y() + panelBottomEven) /
                                           rowPitch) + 1;
  } else {
    HPDRow =  (unsigned int) ((inPanel.y() + panelBottomOdd) /
                              rowPitch) + 1;
  }
  
  if (HPDRow > 15) {
    log << MSG::ERROR << "HPD row number too high" <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }
  
  int HPDNumber = (HPDColumn - 1) * 15 + HPDRow - 1 ;

  cout << HPDColumn<<"  "<<HPDRow<<"  "<<HPDNumber << std::endl;

  const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
  const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
  const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume(4);
  const ISolid* siliconSolid = pvSilicon->lvolume()->solid();
  
  HepPoint3D inHPDMaster = pvHPDMaster->toLocal(inPanel);
  HepPoint3D inHPDSMaster = pvHPDSMaster->toLocal(inHPDMaster);
  HepPoint3D inSilicon = pvSilicon->toLocal(inHPDSMaster);

  cout << inSilicon << std::endl;
  
  if (!siliconSolid->isInside(inSilicon)) {
    log << MSG::ERROR << "The point is outside the silicon box" <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }

  unsigned int pixelColumn = (unsigned int) ((siliconHalfLengthX + 
                                            inSilicon.x()) / pixelSize) + 1;
  unsigned int pixelRow    = (unsigned int) ((siliconHalfLengthY + 
                                            inSilicon.y()) / pixelSize) + 1;
  
  cout << pixelColumn << "  " <<pixelRow << std::endl;

  id = RichSmartID(0,0, HPDRow, HPDColumn, pixelRow, pixelColumn);  
  return StatusCode::SUCCESS;
  
}

//============================================================================

StatusCode DeRichHPDPanel::detectionPoint (const RichSmartID& smartID,
                                           HepPoint3D& windowHitGlobal)
{
  // HPD row and column, pixel row and column as well as HPD panel number
  // and rich id starts at 1 and not 0

  int HPDNumber = (smartID.HPDCol()-1)*15 + smartID.HPDRow()-1;

  // find the correct HPD and silicon block inside it
  const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
  const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
  const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume(4);

  const IPVolume* pvWindow = pvHPDSMaster->lvolume()->pvolume(2);
  const ISolid* windowSolid = pvWindow->lvolume()->solid();

  // convert pixel number to silicon coordinates
  double inSiliconX = (smartID.pixelCol()-1) * pixelSize + pixelSize/2.0 - 
    siliconHalfLengthX;
  double inSiliconY = (smartID.pixelRow()-1)  * pixelSize + pixelSize/2.0 - 
    siliconHalfLengthY;

  HepPoint3D siliconHit( inSiliconX, inSiliconY, 0.0 );
  HepPoint3D topInSilicon(-inSiliconX*5.0, -inSiliconY*5.0, HPDTop.z());
  
  HepVector3D toWindow = (HepVector3D(topInSilicon) - 
                          HepVector3D(siliconHit)).unit();

  cout << siliconHit << topInSilicon << toWindow << std::endl;

  HepPoint3D siliconHitInHPDS = pvSilicon->toMother(siliconHit);
  HepPoint3D siliconHitInWin = pvWindow->toLocal(siliconHitInHPDS);

  
  cout << siliconHitInWin <<toWindow << std::endl;

  ISolid::Ticks myTicks;  

  unsigned int noTicks = windowSolid->intersectionTicks(siliconHitInWin, 
                                                        toWindow, myTicks);
  
  if (2 != noTicks)
    return StatusCode::FAILURE;
  
  HepPoint3D windowHit = siliconHitInWin + toWindow * myTicks[1];

  HepPoint3D windowHitInHPDS = pvWindow->toMother(windowHit);
  HepPoint3D windowHitInHPD = pvHPDSMaster->toMother(windowHitInHPDS);
  HepPoint3D windowHitInPanel = pvHPDMaster->toMother(windowHitInHPD);
  windowHitGlobal = geometry()->toGlobal(windowHitInPanel);

  cout << noTicks << "   "<< myTicks[1] << windowHitGlobal << std::endl;

  return StatusCode::SUCCESS;
  
}

