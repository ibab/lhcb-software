// $Id: DeRich1CdfHPDPanel.cpp,v 1.8 2003-09-20 15:02:49 jonrob Exp $
#define DERICH1CDFHPDPANEL_CPP

// Include files
#include "RichDet/DeRich1CdfHPDPanel.h"

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
// Implementation of class :  DeRich1CdfHPDPanel (COLUMN wise panel)
// the numbering in this class increases in columns and odd cloumns have
// one less HPD than even columns
//
//----------------------------------------------------------------------------

const CLID& CLID_DeRich1CdfHPDPanel = 12013;  // User defined

// Standard Constructor
DeRich1CdfHPDPanel::DeRich1CdfHPDPanel() {}

// Standard Destructor
DeRich1CdfHPDPanel::~DeRich1CdfHPDPanel() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRich1CdfHPDPanel::classID() { return CLID_DeRich1CdfHPDPanel; }

StatusCode DeRich1CdfHPDPanel::initialize() {

  StatusCode sc = StatusCode::FAILURE;

  MsgStream log(msgSvc(), "DeRich1CdfHPDPanel" );
  log << MSG::VERBOSE <<"Starting initialisation of DeRich1CdfHPDPanel"
      << endreq;
  this->printOut(log);

  pixelSize = 0.5*mm;
  m_winR = 56*mm;
  m_winRsq = m_winR*m_winR;


  HepPoint3D zero(0.0, 0.0, 0.0);
  // this is the real number of columns
  HPDColumns = userParameterAsInt("PDColumns");
  // this is 2*rows-1 of what you would expect from a normal grid
  HPDRows = userParameterAsInt("PDRows");
  HPDsInBigCol = static_cast<int>(ceil(HPDRows/2.0));
  int halfHPDCols = HPDColumns/2;
  HPDsIn2Cols = 2*HPDsInBigCol - 1;

  if (0 == HPDColumns%2) {
    m_PDMax = halfHPDCols * HPDsIn2Cols;
  } else {
    m_PDMax = halfHPDCols * HPDsIn2Cols + HPDsInBigCol;
  }

  log << MSG::DEBUG <<"HPDsInBigCol:" << HPDsInBigCol
      << " m_PDMax:" << m_PDMax << endreq;

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

  //get the next HPD (same column)
  const IPVolume* pvHPDMaster1 = geometry()->lvolume()->pvolume(1);
  HepPoint3D HPD1Centre = pvHPDMaster1->toMother(zero);
  rowPitch = HPD1Centre.y() - HPD0Centre.y();

  //get the HPD at next row NS == next set
  HepPoint3D HPDNSCentre;
  const IPVolume* pvHPDMasterNS = geometry()->lvolume()->pvolume(HPDsInBigCol);
  HPDNSCentre = pvHPDMasterNS->toMother(zero);
  columnPitch = HPDNSCentre.x() - HPD0Centre.x();

  double halfColumnPitch = 0.5 * columnPitch;
  double halfRowPitch = 0.5 * rowPitch;

  //std::cout << "HPD0Centre:" << HPD0Centre
  //     << " HPDNSCentre:" << HPDNSCentre<< endreq;

  log << MSG::DEBUG <<"columnPitch:"<< columnPitch << " rowPitch:"
      << rowPitch << endreq;

  // get the Horizontal Edge of the HPD cover area. When the HPD centre
  // coordinate is positive the pitch is negative and vise versa
  panelHorizEdge = HPD0Centre.x() - halfColumnPitch;

  //get the Vertical Edge for the two types of columns
  //numbers start at zero (so the first is even)
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
  // get the outside radius of the window
  ISolid::Ticks windowTicks;
  unsigned int windowTicksSize = windowSolid0->
    intersectionTicks(HepPoint3D(0.0, 0.0, 0.0),HepVector3D(0.0, 0.0, 1.0),
                      windowTicks);
  if (windowTicksSize != 2) {
    log << MSG::ERROR << "Problem getting window radius" << endreq;
    return sc;
  }

  // get the coordinate of the centre of the HPD quarz window
  HepPoint3D HPDTop1(0.0, 0.0, windowTicks[1]);
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

  // for point B go to the last HPD of this column.
  //The relative position inside the HPD is the same
  const IPVolume* pvHPDMasterB=geometry()->lvolume()->pvolume(HPDsInBigCol-1);
  HepPoint3D pointBInPanel = pvHPDMasterB->toMother(pointAInHPD);
  HepPoint3D pointB = geometry()->toGlobal(pointBInPanel);

  // now point C on the other end.
  int HPDForC = halfHPDCols * HPDsIn2Cols - (HPDsInBigCol/2);
  const IPVolume* pvHPDMasterC = geometry()->lvolume()->pvolume(HPDForC);
  HepPoint3D pointCInPanel = pvHPDMasterC->toMother(pointAInHPD);
  HepPoint3D pointC = geometry()->toGlobal(pointCInPanel);

  detectionPlane_m = HepPlane3D(pointA, pointB, pointC);
  //std::cout <<"Detection plane:" << detectionPlane_m << std::endl;

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

  log << MSG::DEBUG <<"Finished initialisation for DeRich1CdfHPDPanel"<< endreq;
  return StatusCode::SUCCESS;
}

// ===========================================================================

StatusCode DeRich1CdfHPDPanel::smartID ( const HepPoint3D& globalPoint,
                                         RichSmartID& id ) {

  MsgStream log(msgSvc(), "DeRich1CdfHPDPanel" );

  if (!geometry()->isInside(globalPoint)) {
    log << MSG::ERROR << "Point outside HPD Panel" <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }

  HepPoint3D inPanel = geometry()->toLocal(globalPoint);

  // HPD row and column, pixel row and column as well as HPD panel number
  // and rich id starts at 1 and not 0

  // find the correct HPD inside the panel
  int HPDColumn(-1), HPDRow(-1);
  int HPDNumber(-1);

  HPDColumn = static_cast<int>(floor((inPanel.x() - panelHorizEdge)
                                     / columnPitch));

  if (HPDColumn >= HPDColumns) {
    log << MSG::ERROR << "Rich2 HPD column number too high" <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }

  if (0 == HPDColumn%2) {
    HPDRow = static_cast<int>(floor((inPanel.y() - panelVerticalEdgeEven)
                                    / rowPitch));
  } else {
    HPDRow = static_cast<int>(floor((inPanel.y() - panelVerticalEdgeOdd)
                                    / rowPitch));
  }

  int HPDColDiv2 = HPDColumn/2;
  if (0 == HPDColumn%2) {
    HPDNumber = HPDColDiv2 * HPDsIn2Cols + HPDRow;
  } else {
    HPDNumber = HPDColDiv2 * HPDsIn2Cols + HPDRow + HPDsInBigCol;
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
  //  const ISolid* siliconSolid = pvSilicon->lvolume()->solid();

  HepPoint3D inHPDMaster = pvHPDMaster->toLocal(inPanel);
  HepPoint3D inHPDSMaster = pvHPDSMaster->toLocal(inHPDMaster);
  HepPoint3D inSilicon = pvSilicon->toLocal(inHPDSMaster);

  //std::cout << inSilicon << std::endl;

  //  if (!siliconSolid->isInside(inSilicon)) {
  //    log << MSG::ERROR << "The point is outside the silicon box "
  //        << pvHPDMaster->name() <<  endreq;
  //    id = RichSmartID(0);
  //    return StatusCode::FAILURE;
  //  }

  if ( (fabs(inSilicon.x()) > siliconHalfLengthX) ||
       (fabs(inSilicon.y()) > siliconHalfLengthY)    ) {
    log << MSG::ERROR << "The point is outside the silicon box "
        << pvHPDMaster->name() <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }


  unsigned int pixelColumn = static_cast<unsigned int>
    ((siliconHalfLengthX + inSilicon.x()) / pixelSize);
  unsigned int pixelRow    = static_cast<unsigned int>
    ((siliconHalfLengthY + inSilicon.y()) / pixelSize);

  //std::cout << pixelColumn << "  " <<pixelRow << std::endl;

  id = RichSmartID(0,0,
                   static_cast<unsigned int>(HPDRow),
                   static_cast<unsigned int>(HPDColumn),
                   pixelRow,
                   pixelColumn);

  return StatusCode::SUCCESS;

}

//============================================================================

StatusCode DeRich1CdfHPDPanel::detectionPoint ( const RichSmartID& smartID,
                                                HepPoint3D& windowHitGlobal )
{
  //MsgStream log(msgSvc(), "DeRich1CdfHPDPanel" );
  //log << MSG::VERBOSE << "In DeRich1CdfHPDPanel::detectionPoint" << endreq;

  // HPD row and column, pixel row and column as well as HPD panel number
  // and rich id starts at 1 and not 0

  int HPDNumber;
  int HPDColDiv2 = smartID.PDCol()/2;
  if (0 == smartID.PDCol()%2) {
    HPDNumber = HPDColDiv2 * HPDsIn2Cols + smartID.PDRow();
  } else {
    HPDNumber = HPDColDiv2 * HPDsIn2Cols + smartID.PDRow() + HPDsInBigCol;
  }

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

StatusCode DeRich1CdfHPDPanel::PDWindowPoint( const HepVector3D& vGlobal,
                                              const HepPoint3D& pGlobal,
                                              HepPoint3D& windowPointGlobal,
                                              RichSmartID& smartID )
{

  // transform point and vector to the HPDPanel coordsystem.
  HepPoint3D pLocal = geometry()->toLocal(pGlobal);
  HepVector3D vLocal = vGlobal;
  vLocal.transform(m_vectorTransf);

  unsigned int noTicks;
  ISolid::Ticks HPDPanelTicks;
  noTicks = m_HPDPanelSolid->intersectionTicks( pLocal, vLocal, HPDPanelTicks );
  if ( 0 == noTicks ) return StatusCode::FAILURE;

  HepPoint3D panelIntersection = pLocal + HPDPanelTicks[0]*vLocal;

  const IPVolume* pvHPDMaster = 0;
  const IPVolume* pvHPDSMaster = 0;
  const IPVolume* pvWindow = 0;
  const ISolid* windowSolid;

  HepPoint3D pInWindow;
  HepVector3D vInHPDMaster;

  ISolid::Ticks HPDWindowTicks;

  int HPDColumn(-1), HPDRow(-1), HPDNumber(-1);
  bool HPDFound1 (false);
  bool HPDFound2 (false);

  if ( (fabs(panelIntersection.x()) <= panelHorizEdge) &&
       (fabs(panelIntersection.y()) <= panelVerticalEdge)) {

    HPDColumn = (int) floor((panelIntersection.x()- panelHorizEdge)
                            /columnPitch);

    double vEdge;
    if (0 == HPDColumn%2) {
      HPDRow = (int) floor((panelIntersection.y()-panelVerticalEdgeEven)
                           /rowPitch);
      vEdge = panelVerticalEdgeEven;
    } else {
      HPDRow = (int) floor((panelIntersection.y()-panelVerticalEdgeOdd)
                           /rowPitch);
      vEdge = panelVerticalEdgeOdd;
    }

    int HPDColDiv2 = HPDColumn/2;
    if (0 == HPDColumn%2) {
      HPDNumber = HPDColDiv2 * HPDsIn2Cols + HPDRow;
    } else {
      HPDNumber = HPDColDiv2 * HPDsIn2Cols + HPDRow + HPDsInBigCol;
    }

    // find the correct HPD and quartz window inside it
    pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);

    // just in case
    if (!pvHPDMaster) {
      MsgStream log(msgSvc(), "DeRich1CdfHPDPanel" );
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
    pInWindow = pvWindow->toLocal(pvHPDSMaster->
                                  toLocal(pvHPDMaster->toLocal(pLocal)));

    // convert local vector assuming that only the HPD can be rotated
    const HepTransform3D vectorTransfHPD = pvHPDMaster->matrix();
    vInHPDMaster = vLocal;
    vInHPDMaster.transform(vectorTransfHPD);

    noTicks = windowSolid->intersectionTicks(pInWindow, vInHPDMaster,
                                             HPDWindowTicks );
    if ( 0 != noTicks ) HPDFound1 = true;

  }

  if ( !HPDFound1 ) {
    // Not in central PD : Try all others

    for ( int HPD=0; HPD<m_PDMax; ++HPD ) {

      // convert point to local coordinate systems
      pInWindow = m_pvWindows[HPD]->toLocal(m_pvHPDSMasters[HPD]->
                                            toLocal(m_pvHPDMasters[HPD]->toLocal(pLocal)));

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

  if (!HPDFound1 && !HPDFound2) return StatusCode::FAILURE;

  HepPoint3D windowPoint = pInWindow + HPDWindowTicks[1]*vInHPDMaster;
  HepPoint3D windowPointInHPDS = pvWindow->toMother(windowPoint);
  HepPoint3D windowPointInHPD = pvHPDSMaster->toMother(windowPointInHPDS);
  HepPoint3D windowPointInPanel = pvHPDMaster->toMother(windowPointInHPD);
  windowPointGlobal = geometry()->toGlobal(windowPointInPanel);

  if ( !HPDFound1 ) {
    HPDColumn = 2*(HPDNumber/HPDsIn2Cols);
    int HPDsLeft = HPDNumber%HPDsIn2Cols;
    if (HPDsLeft >= HPDsInBigCol) {
      ++HPDColumn;
      HPDRow = HPDsLeft - HPDsInBigCol;
    } else {
      HPDRow = HPDsLeft;
    }
  }

  smartID.setPDRow( HPDRow );
  smartID.setPDCol( HPDColumn );
  // For the moment do not bother with pixel info
  smartID.setPixelRow( 0 );
  smartID.setPixelCol( 0 );

  return StatusCode::SUCCESS;

}

