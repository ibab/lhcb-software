// $Id: DeRichHPDPanel.cpp,v 1.3 2002-11-11 13:59:05 papanest Exp $
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
  log << MSG::VERBOSE <<"Starting initialisation of DeRichHPDPanel"<< endreq;
  this->printOut(log);
  
  pixelSize = 0.5*mm;
  HepPoint3D zero(0.0, 0.0, 0.0);
  HPDRows = userParameterAsInt("HPDRows");
  HPDColumns = userParameterAsInt("HPDColumns");  

  const std::string::size_type pos = name().find("Rich2");
  if( std::string::npos != pos ) {
    m_Rich2 = true;
  } else {
    m_Rich2 = false;
  }

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
  HepPoint3D HPD0Centre = pvHPDMaster0->toMother(zero);
  
  //get the next HPD. For Rich1 same row, for Rich2 same column
  const IPVolume* pvHPDMaster1 = geometry()->lvolume()->pvolume(1);
  HepPoint3D HPD1Centre = pvHPDMaster1->toMother(zero);
  if (m_Rich2) {
    rowPitch = HPD1Centre.y() - HPD0Centre.y();
  } else {
    columnPitch = HPD1Centre.x() - HPD0Centre.x();
  }  
  
  //get the HPD at next row for Rich1, next column for Rich2, NS == next set
  HepPoint3D HPDNSCentre;
  if (m_Rich2) {
    const IPVolume* pvHPDMasterNS = geometry()->lvolume()->pvolume(HPDRows);
    HPDNSCentre = pvHPDMasterNS->toMother(zero);
    columnPitch = HPDNSCentre.x() - HPD0Centre.x();
  } else {
    const IPVolume* pvHPDMasterNS = geometry()->lvolume()->pvolume(HPDColumns);
    HPDNSCentre = pvHPDMasterNS->toMother(zero);
    rowPitch = HPDNSCentre.y() - HPD0Centre.y();
  }

  double halfColumnPitch = 0.5 * columnPitch;
  double halfRowPitch = 0.5 * rowPitch;

  //std::cout << "HPD0Centre:" << HPD0Centre 
  //     << " HPDNSCentre:" << HPDNSCentre<< endreq;
  
  log << MSG::DEBUG <<"columnPitch:"<< columnPitch << " rowPitch:"
      << rowPitch << endreq;

  // get the Horizontal Edge of the HPD cover area. When the HPD centre 
  // coordinate is positive the pitch is negative and vise versa
  panelHorizEdgeEven = HPD0Centre.x() - halfColumnPitch;

  if (m_Rich2) {
    panelHorizEdgeOdd = panelHorizEdgeEven;

  } else {
    panelHorizEdgeOdd = HPDNSCentre.x() - halfColumnPitch;
  }  

  panelHorizEdge = fabs(panelHorizEdgeOdd);
  if (fabs(panelHorizEdgeEven) < panelHorizEdge) 
    panelHorizEdge = fabs(panelHorizEdgeEven);
  

  //get the Vertical Edge for the two types of columns
  //numbers are 0 to 8
  panelVerticalEdgeEven = HPD0Centre.y() - halfRowPitch;
  
  if (m_Rich2) {  
    panelVerticalEdgeOdd = HPDNSCentre.y() - halfRowPitch;
  } else {
    panelVerticalEdgeOdd = panelVerticalEdgeEven;
  }
  
  panelVerticalEdge = fabs(panelVerticalEdgeOdd);
  if (fabs(panelVerticalEdgeEven) < fabs(panelVerticalEdge)) 
    panelVerticalEdge = panelVerticalEdgeEven;

  log << MSG::DEBUG <<"panelHorizEdgeEven:"<< panelHorizEdgeEven 
      << " panelHorizEdgeOdd:" << panelHorizEdgeOdd 
      << " panelVerticalEdgeEven:" << panelVerticalEdgeEven
      << " panelVerticalEdgeOdd:" << panelVerticalEdgeOdd << endreq;
  
  // get the pv and the solid for the HPD quartz window
  const IPVolume* pvWindow0 = pvHPDSMaster0->lvolume()->pvolume(2);
  const ISolid* windowSolid0 = pvWindow0->lvolume()->solid();
  const SolidSphere* windowSphere0 = dynamic_cast<const SolidSphere*>
    (windowSolid0);
  double windowRadius = windowSphere0->outerRadius();
  
  // get the coordinate of the centre of the HPD quarz window
  HepPoint3D HPDTop1(0.0, 0.0, windowRadius);
  // convert this to HPDS master  coordinates
  HepPoint3D HPDTop2 = pvWindow0->toMother(HPDTop1);
  // and to silicon
  HPDTop = pvSilicon0->toLocal(HPDTop2);



  // find the top of 3 HPDs to create a detection plane.  We already have the 
  // first in HPDSMaster coordinates.
  // now to HPD coordinates
  HepPoint3D point1InHPD = pvHPDSMaster0->toMother(HPDTop2);
  HepPoint3D point1InPanel = pvHPDMaster0->toMother(point1InHPD);
  HepPoint3D point1 = geometry()->toGlobal(point1InPanel);
  
  // for second point go to HPD 13.  The relative position inside the HPD is
  // the same
  const IPVolume* pvHPDMaster13 = geometry()->lvolume()->pvolume(13);
  HepPoint3D point2InPanel = pvHPDMaster13->toMother(point1InHPD);
  HepPoint3D point2 = geometry()->toGlobal(point2InPanel);

  // now point 3 on the other end.  HPD 91 is OK for both Riches
  const IPVolume* pvHPDMaster91 = geometry()->lvolume()->pvolume(91);
  HepPoint3D point3InPanel = pvHPDMaster91->toMother(point1InHPD);
  HepPoint3D point3 = geometry()->toGlobal(point3InPanel);

  detectionPlane_m = HepPlane3D(point1, point2, point3);

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
  unsigned int HPDColumn, HPDRow;
  int HPDNumber;
  
  if (m_Rich2) {
    HPDColumn = (unsigned int) floor((inPanel.x() - panelHorizEdgeEven) / 
                                      columnPitch);
    //std::cout << inPanel.x() <<"  "<< panelHorizEdgeEven << std::endl;
  

    if (HPDColumn >= (unsigned int) HPDColumns) {
      log << MSG::ERROR << "Rich2 HPD column number too high" <<  endreq;
      id = RichSmartID(0);
      return StatusCode::FAILURE;
    }
  
    if (0 == HPDColumn%2) {
      HPDRow = (unsigned int) floor((inPanel.y() - panelVerticalEdgeEven)/
                                     rowPitch);
    } else {
      HPDRow = (unsigned int) floor((inPanel.y() - panelVerticalEdgeOdd)/
                                     rowPitch);
    }
    //std::cout << inPanel.y() <<"  " << panelVerticalEdgeEven << "  " 
    //     << panelVerticalEdgeOdd << std::endl;
    
    if (HPDRow >= (unsigned int) HPDRows) {
      log << MSG::ERROR << "HPD row number too high" <<  endreq;
      id = RichSmartID(0);
      return StatusCode::FAILURE;
    }
    HPDNumber = HPDColumn * HPDRows + HPDRow;
  } else {
    HPDRow = (unsigned int) floor((inPanel.y() - panelVerticalEdgeEven) /
                                           rowPitch);

    if (HPDRow >= (unsigned int) HPDRows) {
      log << MSG::ERROR << "HPD Row number too high --- " 
          << "inPanel.y:" << inPanel.y() <<" panelVerticalEdgeEven:" 
          << panelVerticalEdgeEven <<  endreq;
      id = RichSmartID(0);
      return StatusCode::FAILURE;
    }
  
    if (0 == HPDRow%2) {
      HPDColumn =  (unsigned int) floor((inPanel.x() - panelHorizEdgeEven)
                                         / columnPitch);
    } else {
      HPDColumn =  (unsigned int) floor((inPanel.x() - panelHorizEdgeOdd) 
                                         / columnPitch);
    }
    //std::cout << inPanel.y() <<"  " << panelVerticalEdgeEven << "  " 
    //     << panelVerticalEdgeOdd << std::endl;
    
    if (HPDColumn >= (unsigned int) HPDColumns) {
      log << MSG::ERROR << "HPD column number too high" <<  endreq;
      id = RichSmartID(0);
      return StatusCode::FAILURE;
    }
    HPDNumber = HPDRow * HPDColumns + HPDColumn;
  }
  
  log << MSG::DEBUG << "HPDColumn: " << HPDColumn <<" HPDRow: "<< HPDRow
      <<" HPDNumber: " << HPDNumber << endreq;

  if (HPDNumber < 0 || HPDNumber >= HPDRows*HPDColumns) {
    log << MSG::ERROR << "Problem with HPDNumner= " << HPDNumber <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }

  const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
  const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
  const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume(4);
  const ISolid* siliconSolid = pvSilicon->lvolume()->solid();
  
  HepPoint3D inHPDMaster = pvHPDMaster->toLocal(inPanel);
  HepPoint3D inHPDSMaster = pvHPDSMaster->toLocal(inHPDMaster);
  HepPoint3D inSilicon = pvSilicon->toLocal(inHPDSMaster);
  
  //std::cout << inSilicon << std::endl;
  
  if (!siliconSolid->isInside(inSilicon)) {
    log << MSG::ERROR << "The point is outside the silicon box "
        << pvHPDMaster->name() <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }

  unsigned int pixelColumn = (unsigned int) ((siliconHalfLengthX + 
                                            inSilicon.x()) / pixelSize);
  unsigned int pixelRow    = (unsigned int) ((siliconHalfLengthY + 
                                            inSilicon.y()) / pixelSize);
  
  //std::cout << pixelColumn << "  " <<pixelRow << std::endl;

  id = RichSmartID(0,0, HPDRow, HPDColumn, pixelRow, pixelColumn);  
  return StatusCode::SUCCESS;
  
}

//============================================================================

StatusCode DeRichHPDPanel::detectionPoint (const RichSmartID& smartID,
                                           HepPoint3D& windowHitGlobal)
{
  MsgStream log(msgSvc(), "DeRichHPDPanel" );
  log << MSG::VERBOSE << "In DeRichHPDPanel::detectionPoint" << endreq;

  // HPD row and column, pixel row and column as well as HPD panel number
  // and rich id starts at 1 and not 0

  int HPDNumber;
  if (m_Rich2) {
    HPDNumber = smartID.HPDCol()*HPDRows + smartID.HPDRow();
  } else {
    HPDNumber = smartID.HPDRow()*HPDColumns + smartID.HPDCol();
  }  

  // find the correct HPD and silicon block inside it
  const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
  const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
  const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume(4);

  const IPVolume* pvWindow = pvHPDSMaster->lvolume()->pvolume(2);
  const ISolid* windowSolid = pvWindow->lvolume()->solid();

  // convert pixel number to silicon coordinates
  double inSiliconX = smartID.pixelCol() * pixelSize + pixelSize/2.0 - 
    siliconHalfLengthX;
  double inSiliconY = smartID.pixelRow() * pixelSize + pixelSize/2.0 - 
    siliconHalfLengthY;

  HepPoint3D siliconHit( inSiliconX, inSiliconY, 0.0 );
  // hit at the top of the tube in silicon coordinates
  HepPoint3D topInSilicon(-inSiliconX*5.0, -inSiliconY*5.0, HPDTop.z());
  
  HepVector3D toWindow = (HepVector3D(topInSilicon) - 
                          HepVector3D(siliconHit)).unit();

  //std::cout << siliconHit << topInSilicon << toWindow << std::endl;

  HepPoint3D siliconHitInHPDS = pvSilicon->toMother(siliconHit);
  HepPoint3D siliconHitInWin = pvWindow->toLocal(siliconHitInHPDS);

  
  //std::cout << siliconHitInWin <<toWindow << std::endl;

  ISolid::Ticks myTicks;  

  unsigned int noTicks = windowSolid->intersectionTicks(siliconHitInWin, 
                                                        toWindow, myTicks);
  
  if (2 != noTicks) {
    log << MSG::DEBUG << "No intersection with HPD window " 
        << pvHPDMaster->name() << endreq;    
    return StatusCode::FAILURE;
  }
  
  HepPoint3D windowHit = siliconHitInWin + toWindow * myTicks[1];

  HepPoint3D windowHitInHPDS = pvWindow->toMother(windowHit);
  HepPoint3D windowHitInHPD = pvHPDSMaster->toMother(windowHitInHPDS);
  HepPoint3D windowHitInPanel = pvHPDMaster->toMother(windowHitInHPD);
  windowHitGlobal = geometry()->toGlobal(windowHitInPanel);

  //std::cout << noTicks << "   "<< myTicks[1] << windowHitGlobal << std::endl;

  return StatusCode::SUCCESS;
  
}

//============================================================================

StatusCode DeRichHPDPanel::HPDWindowPoint(const HepVector3D& vGlobal, 
                                          const HepPoint3D& pGlobal,
                                          HepPoint3D& windowPointGlobal,
                                          RichSmartID& smartID)
{
  MsgStream log(msgSvc(), "DeRichHPDPanel" );
  log << MSG::VERBOSE << "Entering HPDWindowPoint" << endreq;
  

  // transform point and vector to the HPDPanel coordsystem.
  HepPoint3D pLocal = geometry()->toLocal(pGlobal);
  const HepTransform3D vectorTransf = geometry()->matrix();
  HepVector3D vLocal = vGlobal;
  vLocal.transform(vectorTransf);

  const ISolid* HPDPanelSolid = geometry()->lvolume()->solid();

  unsigned int noTicks;
  
  ISolid::Ticks HPDPanelTicks;
  noTicks = HPDPanelSolid->intersectionTicks(pLocal,vLocal,HPDPanelTicks );
  if (0 == noTicks) {
    log << MSG::DEBUG << "Direction did not intersect with HPDPanel " 
        << name() << endreq;
    return StatusCode::FAILURE;
  }
  
  HepPoint3D panelIntersection = pLocal + HPDPanelTicks[0]*vLocal;
  
  const IPVolume* pvHPDMaster;
  const IPVolume* pvHPDSMaster;
  const IPVolume* pvWindow;
  const ISolid* windowSolid;

  HepPoint3D pInHPDMaster, pInHPDSMaster, pInWindow;
  HepVector3D vInHPDMaster;
  
  ISolid::Ticks HPDWindowTicks;

  int HPDColumn, HPDRow, HPDNumber;
  bool HPDFound (false);  

  if ( fabs(panelIntersection.x()) <= panelHorizEdge && 
       fabs(panelIntersection.y()) <= panelVerticalEdge ) {

    if (m_Rich2) {
      HPDColumn = (int) floor((panelIntersection.x()- panelHorizEdgeEven)
                               /columnPitch);

      if (0 == HPDColumn%2) {
        HPDRow = (int) floor((panelIntersection.y()-panelVerticalEdgeEven)
                             /rowPitch);
      } else {
        HPDRow = (int) floor((panelIntersection.y()-panelVerticalEdgeOdd)
                             /rowPitch);
      }

      HPDNumber = HPDColumn*HPDRows + HPDRow;

    } else {

      HPDRow = (int) floor((panelIntersection.y()-panelVerticalEdgeEven)
                            /rowPitch);

      if (0 == HPDRow%2) {
        HPDColumn = (int) floor((panelIntersection.x()-panelHorizEdgeEven)
                             /columnPitch);
      } else {
        HPDColumn = (int) floor((panelIntersection.x()-panelHorizEdgeOdd)
                             /columnPitch);
      }
      HPDNumber = HPDRow*HPDColumns + HPDColumn;
    }

    log << MSG::DEBUG << "HPDRow:" << HPDRow << " HPDColumn:" << HPDColumn
        << endreq;
    
    // find the correct HPD and quartz window inside it
    pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
    // just in case
    if (!pvHPDMaster) {
      log << MSG::ERROR << "Inappropriate HPDNumber:" << HPDNumber
          << " from HPDRow:" << HPDRow << " and HPDColumn:" << HPDColumn
          << " please notify Antonis" << std::endl
          << " m_Rich2:" <<  m_Rich2 << " x:" << panelIntersection.x()
          << " y:" << panelIntersection.y() << " edge1:" 
          << panelHorizEdgeEven << " edge2:" <<panelVerticalEdgeEven << endreq;
      return StatusCode::FAILURE;
    }
    
    pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
    pvWindow = pvHPDSMaster->lvolume()->pvolume(2);
    windowSolid = pvWindow->lvolume()->solid();

    // convert point to local coordinate systems
    pInHPDMaster = pvHPDMaster->toLocal(pLocal);
    pInHPDSMaster = pvHPDSMaster->toLocal(pInHPDMaster);
    pInWindow = pvWindow->toLocal(pInHPDSMaster);
  
    // convert local vector assuming that only the HPD can be rotated
    const HepTransform3D vectorTransfHPD = pvHPDMaster->matrix();
    vInHPDMaster = vLocal;
    vInHPDMaster.transform(vectorTransfHPD);

    //std::cout << pInWindow << vInHPDMaster << std::endl;  

    noTicks = windowSolid->intersectionTicks(pInWindow, vInHPDMaster, 
                                             HPDWindowTicks );
    if (0 == noTicks) {
      log << MSG::DEBUG << "Direction did not intersect with expected HPD " 
          << pvHPDMaster->name() << endreq;
    } else {
      HPDFound = true;
      log << MSG::DEBUG << "Intersect with EXPECTED HPD " 
          << pvHPDMaster->name() << endreq;
    }
    
  }
  
  if (!HPDFound) {
    // first attempt to find relevant HPD failed.
    // search all HPDs for intersection
    int HPDMax = HPDColumns * HPDRows;
    
    for (int HPD=0; HPD<HPDMax; ++HPD) {
      pvHPDMaster = geometry()->lvolume()->pvolume(HPD);
      pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
      pvWindow = pvHPDSMaster->lvolume()->pvolume(2);
      windowSolid = pvWindow->lvolume()->solid();

      // convert point to local coordinate systems
      pInHPDMaster = pvHPDMaster->toLocal(pLocal);
      pInHPDSMaster = pvHPDSMaster->toLocal(pInHPDMaster);
      pInWindow = pvWindow->toLocal(pInHPDSMaster);
  
      // convert local vector assuming that only the HPD can be rotated
      const HepTransform3D vectorTransfHPD2 = pvHPDMaster->matrix();
      vInHPDMaster = vLocal;
      vInHPDMaster.transform(vectorTransfHPD2);

      noTicks = windowSolid->intersectionTicks(pInWindow, vInHPDMaster, 
                                           HPDWindowTicks );
      if (2 == noTicks) {
        log << MSG::DEBUG << "Intersection with HPD " << HPD << endreq;
        HPDFound = true;
        HPDNumber = HPD;
        break;
      }
      
    }
  }

  if (!HPDFound) {
    log << MSG::DEBUG << "NO intersection with HPD" << endreq;
    return StatusCode::FAILURE;
  }

  HepPoint3D windowPoint = pInWindow + HPDWindowTicks[0]*vInHPDMaster;


  HepPoint3D windowPointInHPDS = pvWindow->toMother(windowPoint);
  HepPoint3D windowPointInHPD = pvHPDSMaster->toMother(windowPointInHPDS);
  HepPoint3D windowPointInPanel = pvHPDMaster->toMother(windowPointInHPD);
  windowPointGlobal = geometry()->toGlobal(windowPointInPanel);

  if (m_Rich2) {
    HPDRow = HPDNumber%HPDRows;
    HPDColumn = HPDNumber/HPDRows;
  } else {
    HPDRow = HPDNumber/HPDRows;
    HPDColumn = HPDNumber%HPDRows;
  }

  smartID.setHPDRow( HPDRow );
  smartID.setHPDCol( HPDColumn );
  // For the moment do not bother with pixel info
  smartID.setPixelRow( 0 );
  smartID.setPixelCol( 0 );

  return StatusCode::SUCCESS;
  
}

