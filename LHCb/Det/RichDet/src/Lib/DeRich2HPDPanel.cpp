// $Id: DeRich2HPDPanel.cpp,v 1.3 2003-06-16 13:34:31 sponce Exp $
#define DERICH2HPDPANEL_CPP

// Include files
#include "RichDet/DeRich2HPDPanel.h"

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
// Implementation of class :  DeRich2HPDPanel
//
//----------------------------------------------------------------------------

const CLID& CLID_DeRich2HPDPanel = 12012;  // User defined

// Standard Constructor
DeRich2HPDPanel::DeRich2HPDPanel()
{}

// Standard Destructor
DeRich2HPDPanel::~DeRich2HPDPanel() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRich2HPDPanel::classID() {
  return CLID_DeRich2HPDPanel;
}


StatusCode DeRich2HPDPanel::initialize() {

  StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "DeRich2HPDPanel" );
  log << MSG::VERBOSE <<"Starting initialisation of DeRich2HPDPanel" << endreq;
  this->printOut(log);

  pixelSize = 0.5*mm;
  HepPoint3D zero(0.0, 0.0, 0.0);
  HPDRows = userParameterAsInt("HPDRows");
  HPDColumns = userParameterAsInt("HPDColumns");

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
  rowPitch = HPD1Centre.y() - HPD0Centre.y();

  //get the HPD at next row for Rich1, next column for Rich2, NS == next set
  HepPoint3D HPDNSCentre;
  const IPVolume* pvHPDMasterNS = geometry()->lvolume()->pvolume(HPDRows);
  HPDNSCentre = pvHPDMasterNS->toMother(zero);
  columnPitch = HPDNSCentre.x() - HPD0Centre.x();

  double halfColumnPitch = 0.5 * columnPitch;
  double halfRowPitch = 0.5 * rowPitch;

  log << MSG::DEBUG <<"columnPitch:"<< columnPitch << " rowPitch:"
      << rowPitch << endreq;

  // get the Horizontal Edge of the HPD cover area. When the HPD centre
  // coordinate is positive the pitch is negative and vise versa
  panelHorizEdge = HPD0Centre.x() - halfColumnPitch;
  fabs_panelHorizEdge = fabs(panelHorizEdge);

  //get the Vertical Edge for the two types of columns
  //numbers are 0 to 8
  panelVerticalEdgeEven = HPD0Centre.y() - halfRowPitch;
  panelVerticalEdgeOdd = HPDNSCentre.y() - halfRowPitch;

  panelVerticalEdge = fabs(panelVerticalEdgeOdd);
  if (fabs(panelVerticalEdgeEven) < panelVerticalEdge)
    panelVerticalEdge = fabs(panelVerticalEdgeEven);

  log << MSG::DEBUG <<"panelHorizEdge:"<< panelHorizEdge
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
  HepPoint3D pointAInHPD = pvHPDSMaster0->toMother(HPDTop2);
  HepPoint3D pointAInPanel = pvHPDMaster0->toMother(pointAInHPD);
  HepPoint3D pointA = geometry()->toGlobal(pointAInPanel);

  // for second point go to HPD at the end of the column.
  //The relative position inside the HPD is the same
  const IPVolume* pvHPDMasterB = geometry()->lvolume()->pvolume(HPDRows-1);
  HepPoint3D pointBInPanel = pvHPDMasterB->toMother(pointAInHPD);
  HepPoint3D pointB = geometry()->toGlobal(pointBInPanel);

  // now point 3 on the other end.  HPD 91 is OK for both Riches
  int HPDForC = HPDRows*HPDColumns - HPDColumns/2;
  const IPVolume* pvHPDMasterC = geometry()->lvolume()->pvolume(HPDForC);
  HepPoint3D pointCInPanel = pvHPDMasterC->toMother(pointAInHPD);
  HepPoint3D pointC = geometry()->toGlobal(pointCInPanel);

  detectionPlane_m = HepPlane3D(pointA, pointB, pointC);

  // Cache information for HPDWindowPoint method
  m_vectorTransf = geometry()->matrix();
  m_HPDPanelSolid = geometry()->lvolume()->solid();
  for ( int HPD = 0; HPD < HPDColumns*HPDRows; ++HPD ) {
    m_pvHPDMasters.push_back( geometry()->lvolume()->pvolume(HPD) );
    m_pvHPDSMasters.push_back( m_pvHPDMasters[HPD]->lvolume()->pvolume(0) );
    m_pvWindows.push_back( m_pvHPDSMasters[HPD]->lvolume()->pvolume(2) );
    m_windowSolids.push_back( m_pvWindows[HPD]->lvolume()->solid() );
    m_vectorTransfHPD2s.push_back( m_pvHPDMasters[HPD]->matrix() );
  }

  log << MSG::DEBUG <<"Finished initialisation for DeRich2HPDPanel"<< endreq;

  return sc;
}

// ===========================================================================

StatusCode DeRich2HPDPanel::smartID (const HepPoint3D& globalPoint,
                                     RichSmartID& id) {
  MsgStream log(msgSvc(), "DeRich2HPDPanel" );

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

  HPDColumn = (unsigned int) floor((inPanel.x() - panelHorizEdge) /
                                   columnPitch);

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

StatusCode DeRich2HPDPanel::detectionPoint (const RichSmartID& smartID,
                                            HepPoint3D& windowHitGlobal)
{
  //MsgStream log(msgSvc(), "DeRich2HPDPanel" );
  //log << MSG::VERBOSE << "In DeRich2HPDPanel::detectionPoint" << endreq;

  // HPD row and column, pixel row and column as well as HPD panel number
  // and rich id starts at 1 and not 0

  int HPDNumber = smartID.HPDCol()*HPDRows + smartID.HPDRow();

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

  HepPoint3D siliconHitInHPDS = pvSilicon->toMother(siliconHit);
  HepPoint3D siliconHitInWin = pvWindow->toLocal(siliconHitInHPDS);

  ISolid::Ticks myTicks;

  unsigned int noTicks = windowSolid->intersectionTicks(siliconHitInWin,
                                                        toWindow, myTicks);

  if (2 != noTicks) return StatusCode::FAILURE;

  HepPoint3D windowHit = siliconHitInWin + toWindow * myTicks[1];

  HepPoint3D windowHitInHPDS = pvWindow->toMother(windowHit);
  HepPoint3D windowHitInHPD = pvHPDSMaster->toMother(windowHitInHPDS);
  HepPoint3D windowHitInPanel = pvHPDMaster->toMother(windowHitInHPD);
  windowHitGlobal = geometry()->toGlobal(windowHitInPanel);

  return StatusCode::SUCCESS;

}

//============================================================================

StatusCode DeRich2HPDPanel::HPDWindowPoint( const HepVector3D& vGlobal,
                                            const HepPoint3D& pGlobal,
                                            HepPoint3D& windowPointGlobal,
                                            RichSmartID& smartID ) {

  // transform point and vector to the HPDPanel coordsystem.
  HepPoint3D pLocal = geometry()->toLocal(pGlobal);
  HepVector3D vLocal = vGlobal;
  // CRJ
  //const HepTransform3D vectorTransf = geometry()->matrix();
  //vLocal.transform(vectorTransf);
  vLocal.transform(m_vectorTransf);

  //const ISolid* HPDPanelSolid = geometry()->lvolume()->solid();

  unsigned int noTicks;
  ISolid::Ticks HPDPanelTicks;
  noTicks = m_HPDPanelSolid->intersectionTicks( pLocal, vLocal, HPDPanelTicks );
  if ( 0 == noTicks ) return StatusCode::FAILURE;

  HepPoint3D panelIntersection = pLocal + HPDPanelTicks[0]*vLocal;

  const IPVolume* pvHPDMaster = 0;
  const IPVolume* pvHPDSMaster = 0;
  const IPVolume* pvWindow = 0;
  const ISolid* windowSolid;

  HepPoint3D pInHPDMaster, pInHPDSMaster, pInWindow;
  HepVector3D vInHPDMaster;

  ISolid::Ticks HPDWindowTicks;

  int HPDColumn(-1), HPDRow(-1), HPDNumber(-1);
  bool HPDFound1(false), HPDFound2(false);

  if ( (fabs(panelIntersection.x()) <= fabs_panelHorizEdge) &&
       (fabs(panelIntersection.y()) <= panelVerticalEdge)) {
    
    HPDColumn = (int) floor((panelIntersection.x()- panelHorizEdge)
                            /columnPitch);

    if (0 == HPDColumn%2) {
      HPDRow = (int) floor((panelIntersection.y()-panelVerticalEdgeEven)
                           /rowPitch);
    } else {
      HPDRow = (int) floor((panelIntersection.y()-panelVerticalEdgeOdd)
                           /rowPitch);
    }
    HPDNumber = HPDColumn*HPDRows + HPDRow;

    // find the correct HPD and quartz window inside it
    pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
    // just in case
    if ( !pvHPDMaster ) {
      MsgStream log(msgSvc(), "DeRich2HPDPanel" );
      log << MSG::ERROR << "Inappropriate HPDNumber:" << HPDNumber
          << " from HPDRow:" << HPDRow << " and HPDColumn:" << HPDColumn
          << " please notify Antonis" << endreq
          << " x:" << panelIntersection.x()
          << " y:" << panelIntersection.y() << " edge1:"
          << panelHorizEdge << " edge2:" <<panelVerticalEdgeEven << endreq;
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

    noTicks = windowSolid->intersectionTicks(pInWindow, vInHPDMaster,
                                             HPDWindowTicks );
    if ( 0 != noTicks ) HPDFound1 = true;

  }

  if ( !HPDFound1 ) {
    // first attempt to find relevant HPD failed.
    // search all HPDs for intersection
    int HPDMax = HPDColumns * HPDRows;
    for ( int HPD = 0; HPD < HPDMax; ++HPD ) {

      // convert point to local coordinate systems
      pInHPDMaster = m_pvHPDMasters[HPD]->toLocal(pLocal);
      pInHPDSMaster = m_pvHPDSMasters[HPD]->toLocal(pInHPDMaster);
      pInWindow = m_pvWindows[HPD]->toLocal(pInHPDSMaster);

      // convert local vector assuming that only the HPD can be rotated
      vInHPDMaster = vLocal;
      vInHPDMaster.transform( m_vectorTransfHPD2s[HPD] );

      noTicks = m_windowSolids[HPD]->intersectionTicks( pInWindow, 
                                                        vInHPDMaster,
                                                        HPDWindowTicks );
      if ( 2 == noTicks ) {
        HPDFound2 = true;
        HPDNumber = HPD;
        pvHPDMaster = m_pvHPDMasters[HPD];
        pvHPDSMaster = m_pvHPDSMasters[HPD];
        pvWindow = m_pvWindows[HPD];
        break;
      }

    }
  }

  if ( !HPDFound1 && !HPDFound2 ) return StatusCode::FAILURE;

  HepPoint3D windowPoint = pInWindow + HPDWindowTicks[0]*vInHPDMaster;
  HepPoint3D windowPointInHPDS = pvWindow->toMother(windowPoint);
  HepPoint3D windowPointInHPD = pvHPDSMaster->toMother(windowPointInHPDS);
  HepPoint3D windowPointInPanel = pvHPDMaster->toMother(windowPointInHPD);
  windowPointGlobal = geometry()->toGlobal(windowPointInPanel);
  
  if ( HPDFound2 ) {
    HPDRow = HPDNumber%HPDRows;
    HPDColumn = HPDNumber/HPDRows;
  }

  smartID.setHPDRow( HPDRow );
  smartID.setHPDCol( HPDColumn );
  // For the moment do not bother with pixel info
  smartID.setPixelRow( 0 );
  smartID.setPixelCol( 0 );

  return StatusCode::SUCCESS;

}

