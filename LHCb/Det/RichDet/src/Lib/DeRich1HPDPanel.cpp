// $Id: DeRich1HPDPanel.cpp,v 1.7 2003-10-22 10:48:28 papanest Exp $
#define DERICH1HPDPANEL_CPP

// Include files
#include "RichDet/DeRich1HPDPanel.h"

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
// Implementation of class :  DeRich1HPDPanel
//
//----------------------------------------------------------------------------

const CLID& CLID_DeRich1HPDPanel = 12011;  // User defined

// Standard Constructor
DeRich1HPDPanel::DeRich1HPDPanel()
{}

// Standard Destructor
DeRich1HPDPanel::~DeRich1HPDPanel() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRich1HPDPanel::classID() {
  return CLID_DeRich1HPDPanel;
}


StatusCode DeRich1HPDPanel::initialize() {

  StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "DeRich1HPDPanel" );
  log << MSG::VERBOSE <<"Starting initialisation of DeRich1HPDPanel" << endreq;
  this->printOut(log);

  pixelSize = 0.5*mm;
  m_winR = 56*mm;
  m_winRsq = m_winR*m_winR;

  HepPoint3D zero(0.0, 0.0, 0.0);
  HPDRows = userParameterAsInt("PDRows");
  HPDColumns = userParameterAsInt("PDColumns");
  m_PDMax = HPDColumns * HPDRows;

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

  //get the next HPD. For Rich1 same row
  const IPVolume* pvHPDMaster1 = geometry()->lvolume()->pvolume(1);
  HepPoint3D HPD1Centre = pvHPDMaster1->toMother(zero);
  columnPitch = HPD1Centre.x() - HPD0Centre.x();

  //get the HPD at next row for Rich1, next column for Rich1, NS == next set
  HepPoint3D HPDNSCentre;
  const IPVolume* pvHPDMasterNS = geometry()->lvolume()->pvolume(HPDColumns);
  HPDNSCentre = pvHPDMasterNS->toMother(zero);
  rowPitch = HPDNSCentre.y() - HPD0Centre.y();

  double halfColumnPitch = 0.5 * columnPitch;
  double halfRowPitch = 0.5 * rowPitch;

  //std::cout << "HPD0Centre:" << HPD0Centre
  //     << " HPDNSCentre:" << HPDNSCentre<< endreq;

  log << MSG::DEBUG <<"columnPitch:"<< columnPitch << " rowPitch:"
      << rowPitch << endreq;

  // get the Horizontal Edge of the HPD cover area. When the HPD centre
  // coordinate is positive the pitch is negative and vise versa
  panelHorizEdgeEven = HPD0Centre.x() - halfColumnPitch;
  panelHorizEdgeOdd = HPDNSCentre.x() - halfColumnPitch;

  panelHorizEdge = fabs(panelHorizEdgeOdd);
  m_detPlaneHorizEdge = fabs(panelHorizEdgeEven);

  if (m_detPlaneHorizEdge < panelHorizEdge) {
    m_detPlaneHorizEdge = panelHorizEdge;
    panelHorizEdge = fabs(panelHorizEdgeEven);
  }
  

  //get the Vertical Edge for the two types of columns
  //numbers are 0 to 8
  panelVerticalEdge = HPD0Centre.y() - halfRowPitch;

  panelVerticalEdge = fabs(panelVerticalEdge);
  m_detPlaneVertEdge = panelVerticalEdge;

  log << MSG::DEBUG <<"panelHorizEdgeEven:"<< panelHorizEdgeEven
      << " panelHorizEdgeOdd:" << panelHorizEdgeOdd
      << " panelVerticalEdge:" << panelVerticalEdge << endreq;

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

  // for point B go to the HPD at the end of the row.
  //The relative position inside the HPD is the same
  const IPVolume* pvHPDMasterB = geometry()->lvolume()->pvolume(HPDColumns-1);
  HepPoint3D pointBInPanel = pvHPDMasterB->toMother(pointAInHPD);
  HepPoint3D pointB = geometry()->toGlobal(pointBInPanel);

  // now point C on the other end.
  int HPDForC = HPDRows*HPDColumns - HPDRows/2;
  const IPVolume* pvHPDMasterC = geometry()->lvolume()->pvolume(HPDForC);
  HepPoint3D pointCInPanel = pvHPDMasterC->toMother(pointAInHPD);
  HepPoint3D pointC = geometry()->toGlobal(pointCInPanel);

  m_detectionPlane = HepPlane3D(pointA, pointB, pointC);
  m_detectionPlane.normalize();
  m_localPlane = HepPlane3D(pointAInPanel, pointBInPanel, pointCInPanel);
  m_localPlaneNormal = m_localPlane.normal();
  //std::cout <<"Detection plane:" << m_detectionPlane << std::endl;

  // Cache information for PDWindowPoint method
  m_vectorTransf  = geometry()->matrix();
  m_HPDPanelSolid = geometry()->lvolume()->solid();
  for ( int HPD = 0; HPD < m_PDMax; ++HPD ) {
    m_pvHPDMasters.push_back( geometry()->lvolume()->pvolume(HPD) );
    m_pvHPDSMasters.push_back( m_pvHPDMasters[HPD]->lvolume()->pvolume(0) );
    m_pvWindows.push_back( m_pvHPDSMasters[HPD]->lvolume()->pvolume(2) );
    m_windowSolids.push_back( m_pvWindows[HPD]->lvolume()->solid() );
    m_vectorTransfHPD2s.push_back( m_pvHPDMasters[HPD]->matrix() );
  }

  log << MSG::DEBUG <<"Finished initialisation for DeRich1HPDPanel"<< endreq;

  return sc;
}

// ===========================================================================

StatusCode DeRich1HPDPanel::smartID (const HepPoint3D& globalPoint,
                                     RichSmartID& id)
{
  MsgStream log(msgSvc(), "DeRich1HPDPanel" );

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

  HPDRow = (unsigned int) floor((inPanel.y() - panelVerticalEdge) /
                                rowPitch);

  if (HPDRow >= (unsigned int) HPDRows) {
    log << MSG::ERROR << "HPD Row number too high --- "
        << "inPanel.y:" << inPanel.y() <<" panelVerticalEdge:"
        << panelVerticalEdge <<  endreq;
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

StatusCode DeRich1HPDPanel::detectionPoint (const RichSmartID& smartID,
                                            HepPoint3D& windowHitGlobal)
{

  // HPD row and column, pixel row and column as well as HPD panel number
  // and rich id starts at 1 and not 0

  int HPDNumber = smartID.PDRow()*HPDColumns + smartID.PDCol();

  // find the correct HPD and silicon block inside it
  const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
  const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
  //  const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume(4);

  const IPVolume* pvWindow = pvHPDSMaster->lvolume()->pvolume(2);
  //  const ISolid* windowSolid = pvWindow->lvolume()->solid();

  // convert pixel number to silicon coordinates
  double inSiliconX = smartID.pixelCol() * pixelSize + pixelSize/2.0 -
    siliconHalfLengthX;
  double inSiliconY = smartID.pixelRow() * pixelSize + pixelSize/2.0 -
    siliconHalfLengthY;


  double inWindowX = -5.0 * inSiliconX;
  double inWindowY = -5.0 * inSiliconY;
  double inWindowZ = sqrt(m_winRsq-inWindowX*inWindowX-inWindowY*inWindowY);
  HepPoint3D windowHit(inWindowX, inWindowY, inWindowZ);
  //  std::cout << windowHit << std::endl;

  HepPoint3D windowHitInHPDS = pvWindow->toMother(windowHit);
  HepPoint3D windowHitInHPD = pvHPDSMaster->toMother(windowHitInHPDS);
  HepPoint3D windowHitInPanel = pvHPDMaster->toMother(windowHitInHPD);
  windowHitGlobal = geometry()->toGlobal(windowHitInPanel);

  return StatusCode::SUCCESS;

}

//============================================================================

StatusCode DeRich1HPDPanel::PDWindowPoint( const HepVector3D& vGlobal,
                                           const HepPoint3D& pGlobal,
                                           HepPoint3D& windowPointGlobal,
                                           RichSmartID& smartID ) {

  // transform point and vector to the HPDPanel coordsystem.
  HepPoint3D pLocal = geometry()->toLocal(pGlobal);
  HepVector3D vLocal = vGlobal;
  vLocal.transform(m_vectorTransf);

  unsigned int noTicks;
  ISolid::Ticks HPDPanelTicks;
  //  noTicks =m_HPDPanelSolid->intersectionTicks(pLocal,vLocal,HPDPanelTicks);
  //  if ( 0 == noTicks ) return StatusCode::FAILURE;
  //  HepPoint3D panelIntersection = pLocal + HPDPanelTicks[0]*vLocal;

  double scalar1 = vLocal*m_localPlaneNormal;
  double distance = 0.0;

  if ( scalar1 == 0.0 ) return StatusCode::FAILURE;

  distance = -(m_localPlane.d() + pLocal*m_localPlaneNormal) / scalar1;
  HepPoint3D panelIntersection( pLocal + distance*vLocal );

  const IPVolume* pvHPDMaster = 0;
  const IPVolume* pvHPDSMaster = 0;
  const IPVolume* pvWindow = 0;
  const ISolid* windowSolid;

  HepPoint3D pInWindow;
  HepVector3D vInHPDMaster;

  ISolid::Ticks HPDWindowTicks;

  int HPDColumn(-1), HPDRow(-1), HPDNumber(-1);
  bool HPDFound(false);

  if ( (fabs(panelIntersection.x()) <= panelHorizEdge) &&
       (fabs(panelIntersection.y()) <= panelVerticalEdge)) {

    HPDRow = (int) floor((panelIntersection.y()-panelVerticalEdge)
                         /rowPitch);

    if (0 == HPDRow%2) {
      HPDColumn = (int) floor((panelIntersection.x()-panelHorizEdgeEven)
                              /columnPitch);
    } else {
      HPDColumn = (int) floor((panelIntersection.x()-panelHorizEdgeOdd)
                              /columnPitch);
    }
    HPDNumber = HPDRow*HPDColumns + HPDColumn;

    // find the correct HPD and quartz window inside it
    pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);

    // just in case
    if ( !pvHPDMaster ) {
      MsgStream log(msgSvc(), "DeRich1HPDPanel" );
      log << MSG::ERROR << "Inappropriate HPDNumber:" << HPDNumber
          << " from HPDRow:" << HPDRow << " and HPDColumn:" << HPDColumn
          << " please notify Antonis" << std::endl
          << " x:" << panelIntersection.x()
          << " y:" << panelIntersection.y() << " edge1:"
          << panelHorizEdgeEven << " edge2:" <<panelVerticalEdge << endreq;
      return StatusCode::FAILURE;
    }

    pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
    pvWindow = pvHPDSMaster->lvolume()->pvolume(2);
    windowSolid = pvWindow->lvolume()->solid();

    // convert point to local coordinate systems
    pInWindow = pvWindow->toLocal(pvHPDSMaster->
                                  toLocal(pvHPDMaster->toLocal(pLocal)));

    // convert local vector assuming that only the HPD can be rotated
    const HepTransform3D vectorTransfHPD = pvHPDMaster->matrix();
    vInHPDMaster = vLocal;
    vInHPDMaster.transform(vectorTransfHPD);

    noTicks = windowSolid->intersectionTicks( pInWindow, vInHPDMaster,
                                              HPDWindowTicks );
    if ( 0 != noTicks ) HPDFound = true;

  }

  if (!HPDFound) {
    // Not in central PD : Try nearest neighbours

    for ( int HPD=0; HPD<m_PDMax; ++HPD ) {

      // convert point to local coordinate systems
      pInWindow =  m_pvWindows[HPD]->
        toLocal(m_pvHPDSMasters[HPD]->toLocal(m_pvHPDMasters[HPD]->
                                              toLocal(pLocal)));

      // convert local vector assuming that only the HPD can be rotated
      vInHPDMaster = vLocal;
      vInHPDMaster.transform( m_vectorTransfHPD2s[HPD] );

      noTicks = m_windowSolids[HPD]->intersectionTicks( pInWindow,
                                                        vInHPDMaster,
                                                        HPDWindowTicks );
      if ( 2 == noTicks ) {
        HPDFound = true;
        HPDNumber = HPD;
        pvHPDMaster = m_pvHPDMasters[HPD];
        pvHPDSMaster = m_pvHPDSMasters[HPD];
        pvWindow = m_pvWindows[HPD];
        HPDRow = HPDNumber/HPDRows;
        HPDColumn = HPDNumber%HPDRows;
        break;
      }

    }

  }

  if (!HPDFound) return StatusCode::FAILURE;

  HepPoint3D windowPoint = pInWindow + HPDWindowTicks[1]*vInHPDMaster;
  HepPoint3D windowPointInHPDS = pvWindow->toMother(windowPoint);
  HepPoint3D windowPointInHPD = pvHPDSMaster->toMother(windowPointInHPDS);
  HepPoint3D windowPointInPanel = pvHPDMaster->toMother(windowPointInHPD);
  windowPointGlobal = geometry()->toGlobal(windowPointInPanel);

  smartID.setPDRow( HPDRow );
  smartID.setPDCol( HPDColumn );
  // For the moment do not bother with pixel info
  smartID.setPixelRow( 0 );
  smartID.setPixelCol( 0 );

  return StatusCode::SUCCESS;

}


