// $Id: DeRichMaPMTPanel.cpp,v 1.4 2003-10-22 10:48:28 papanest Exp $
#define DERICHMAPMTPANEL_CPP

// Include files
#include "RichDet/DeRichMaPMTPanel.h"

// CLHEP files
#include "CLHEP/Units/SystemOfUnits.h"
#include "Kernel/CLHEPStreams.h"

/// Detector description classes
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

//----------------------------------------------------------------------------
//
// Implementation of class :  DeRichMaPMTPanel
//
//----------------------------------------------------------------------------

const CLID& CLID_DeRichMaPMTPanel = 12015;  // User defined

// Standard Constructor
DeRichMaPMTPanel::DeRichMaPMTPanel()
{}

// Standard Destructor
DeRichMaPMTPanel::~DeRichMaPMTPanel() {}


// Retrieve Pointer to class defininition structure
const CLID& DeRichMaPMTPanel::classID() {
  return CLID_DeRichMaPMTPanel;
}

//=============================================================================

StatusCode DeRichMaPMTPanel::initialize() {

  MsgStream log(msgSvc(), "DeRichMaPMTPanel" );
  log << MSG::DEBUG <<"Starting initialisation of DeRichMaPMTPanel"<< endreq;
  this->printOut(log);

  // get the detector service and get some MaPMT parameters from Rich1
  IDataProviderSvc* detSvc =  dataSvc();
  SmartDataPtr<DetectorElement> deRich1(detSvc, "/dd/Structure/LHCb/Rich1");

  m_pixelRows = static_cast<unsigned int>
    (deRich1->userParameterAsInt("RichMapmtNumPixelRow"));
  m_pixelCols = static_cast<unsigned int>
    (deRich1->userParameterAsInt("RichMapmtNumPixelCol"));
  m_cdfPixelSize = deRich1->userParameterAsDouble("RhCdfMaPMTPixelSize");
  m_cdfPixelPitch = deRich1->userParameterAsDouble("RhCdfMaPMTPixelPitch");
  // make sure pixel size < pixel pitch
  if (m_cdfPixelSize >= m_cdfPixelPitch) {
    log << MSG::ERROR << "Pixel size >= to pixel pitch" << endreq;
    return StatusCode::FAILURE;
  }

  HepPoint3D zero(0.0, 0.0, 0.0);
  m_PDRows = static_cast<unsigned int>(userParameterAsInt("PDRows"));
  m_PDColumns = static_cast<unsigned int>(userParameterAsInt("PDColumns"));
  m_PDMax = m_PDColumns * m_PDRows;

  log << MSG::DEBUG << "Center of Panel " << geometry()->toGlobal(zero) 
      << endreq;

  // get the first MaPMT and find the quartz window
  const IPVolume* pvMaPMTMaster0 = geometry()->lvolume()->pvolume(0);
  const IPVolume* pvMaPMTSMaster0 = pvMaPMTMaster0->lvolume()->pvolume(0);
  const IPVolume* pvQuartzWin0 = pvMaPMTSMaster0->lvolume()->
    pvolume("pvRichMapmtQW");

  // window is a box, find its size
  const ISolid* quartzWinSolid = pvQuartzWin0->lvolume()->solid();
  log << MSG::DEBUG << "About to do a dynamic_cast" << endreq;
  const SolidBox* quartzWinBox = dynamic_cast<const SolidBox*>(quartzWinSolid);
  // assume same size for all MaPMT windows
  m_quartzWinHalfLengthX = quartzWinBox->xHalfLength();
  m_quartzWinHalfLengthY = quartzWinBox->yHalfLength();
  m_quartzWinHalfLengthZ = quartzWinBox->zHalfLength();


  // MaPMT #0 coordinates
  HepPoint3D MaPMT0Centre( pvMaPMTMaster0->toMother(zero) );
  // MaPMT #1 coordinates
  const IPVolume* pvMaPMTMaster1 = geometry()->lvolume()->
    pvolume(1);
  HepPoint3D MaPMT1Centre( pvMaPMTMaster1->toMother(zero) );
  m_columnPitch = MaPMT1Centre.x() - MaPMT0Centre.x();

  // MaPMT next row
  const IPVolume* pvMaPMTMasterNR = geometry()->lvolume()->
    pvolume(m_PDColumns);
  HepPoint3D MaPMTNRCentre( pvMaPMTMasterNR->toMother(zero) );
  m_rowPitch = MaPMTNRCentre.y() - MaPMT0Centre.y();

  //define the edges of the panel
  m_panelHorizEdge = MaPMT0Centre.x() - 0.5*m_columnPitch;
  m_panelVerticalEdge = MaPMT0Centre.y() - 0.5*m_rowPitch;

  m_abs_panelHorizEdge = fabs(m_panelHorizEdge);
  m_abs_panelVerticalEdge = fabs(m_panelVerticalEdge);

  m_detPlaneHorizEdge = m_abs_panelHorizEdge;
  m_detPlaneVertEdge = m_abs_panelVerticalEdge;
  
  // get the anode pixel pitch
  const IPVolume* pvPixel0_0 = pvMaPMTSMaster0->lvolume()->
    pvolume("pvRichMapmtPixel:0");
  HepPoint3D Pixel0Centre( pvPixel0_0->toMother(zero) );
  const IPVolume* pvPixel0_1 = pvMaPMTSMaster0->lvolume()->
    pvolume("pvRichMapmtPixel:1");
  HepPoint3D Pixel1Centre( pvPixel0_1->toMother(zero) );
  m_pixelPitch = Pixel1Centre.x() - Pixel0Centre.x();

  // get the "edges" of the anode array
  m_pixelHorizEdge = Pixel0Centre.x() - 0.5*m_pixelPitch;
  m_pixelVerticalEdge = Pixel0Centre.y() - 0.5*m_pixelPitch;


  log << MSG::DEBUG << "PDRows:" << m_PDRows << " PDColumns:"
      << m_PDColumns << " PDMax:" <<  m_PDMax << " rowPitch:" << m_rowPitch
      << " columnPitch:" << m_columnPitch << " pixelPitch:" << m_pixelPitch
      << " panelHorizEdge:" <<  m_panelHorizEdge
      << " panelVerticalEdge:" << m_panelVerticalEdge
      << " pixelHorizEdge:" <<  m_pixelHorizEdge
      << " pixelVerticalEdge:" << m_pixelVerticalEdge
      << endreq;

  // find the detection plane getting a point of the inside of the MaPMT
  // window for 3 different PMTs
  // Point A: MaPMT 0
  HepPoint3D pointAInSMaster( pvQuartzWin0->
                              toMother(HepPoint3D(0.0, 0.0, 
                                                  -m_quartzWinHalfLengthZ)) );
  HepPoint3D pointAInMaster( pvMaPMTSMaster0->toMother(pointAInSMaster) );
  HepPoint3D pointAInPanel( pvMaPMTMaster0->toMother(pointAInMaster) );
  HepPoint3D pointA( geometry()->toGlobal(pointAInPanel) );
  // Point B: MaPMT at the end of the row. Use the same point inside the MaPMT
  const IPVolume* pvMaPMTMasterB=geometry()->lvolume()->pvolume(m_PDColumns-1);
  HepPoint3D pointBInPanel( pvMaPMTMasterB->toMother(pointAInMaster) );
  HepPoint3D pointB( geometry()->toGlobal(pointBInPanel) );
  // Point C
  int MaPMTForPointC = m_PDColumns*(m_PDRows-1);
  const IPVolume* pvMaPMTMasterC=geometry()->lvolume()->
    pvolume(MaPMTForPointC);
  HepPoint3D pointCInPanel( pvMaPMTMasterC->toMother(pointAInMaster) );
  HepPoint3D pointC( geometry()->toGlobal(pointCInPanel) );

  m_detectionPlane = HepPlane3D(pointA, pointB, pointC);
  m_detectionPlane.normalize();
  m_localPlane = HepPlane3D(pointAInPanel, pointBInPanel, pointCInPanel);
  m_localPlane.normalize();
  m_localPlaneNormal = m_localPlane.normal();

  // Cache information for PDWindowPoint method
  m_vectorTransf = geometry()->matrix();
  m_MaPMTPanelSolid = geometry()->lvolume()->solid();
  for ( unsigned int MaPMT = 0; MaPMT < m_PDMax; ++MaPMT ) {
    m_pvMaPMTMasters.push_back( geometry()->lvolume()->pvolume(MaPMT) );
    m_pvMaPMTSMasters.push_back(m_pvMaPMTMasters[MaPMT]->lvolume()
                                ->pvolume(0));
    m_pvWindows.push_back( m_pvMaPMTSMasters[MaPMT]->lvolume()->
                           pvolume("pvRichMapmtQW") );
    m_windowSolids.push_back( m_pvWindows[MaPMT]->lvolume()->solid() );
    m_vectorTransfMaPMT2s.push_back( m_pvMaPMTMasters[MaPMT]->matrix() );
  }

  // save cdf information
  m_cdfPixelHorizEdge = m_pixelCols*m_cdfPixelPitch/2;
  m_cdfPixelVerticalEdge = m_pixelRows*m_cdfPixelPitch/2;
  if (m_cdfPixelHorizEdge > m_quartzWinHalfLengthX ||
      m_cdfPixelVerticalEdge > m_quartzWinHalfLengthY) {
    log << MSG::ERROR << "Cdf Pixel pitch:" <<  m_cdfPixelPitch
        << " is too big for the window half size:" <<  m_quartzWinHalfLengthX
        << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode DeRichMaPMTPanel::smartID (const HepPoint3D& globalPoint,
                                      RichSmartID& id) {

  StatusCode sc = StatusCode::FAILURE;

  if (!geometry()->isInside(globalPoint)) {
    MsgStream log(msgSvc(), "DeRichMaPMTPanel" );
    log << MSG::ERROR << "Point outside MaPMT Panel" <<  endreq;
    id = RichSmartID(0);
    return sc;
  }

  HepPoint3D inPanel( geometry()->toLocal(globalPoint) );

  if ( fabs(inPanel.x()) >= m_abs_panelHorizEdge &&
       fabs(inPanel.y()) >= m_abs_panelVerticalEdge ) {
    id = RichSmartID(0);
    return sc;
  }

  unsigned int MaPMTRow, MaPMTColumn;

  MaPMTRow = (unsigned int) floor((inPanel.y() - m_panelVerticalEdge)/
                                  m_rowPitch);
  MaPMTColumn = (unsigned int) floor((inPanel.x() - m_panelHorizEdge)/
                                     m_columnPitch);

  unsigned int MaPMTNum = MaPMTRow*m_PDColumns + MaPMTColumn;
  if (MaPMTNum >= m_PDMax) {
    MsgStream log(msgSvc(), "DeRichMaPMTPanel" );
    log << MSG::ERROR << "Problem with MaPMT number" <<  endreq;
    id = RichSmartID(0);
    return sc;
  }

  HepPoint3D pInMaPMT( m_pvMaPMTMasters[MaPMTNum]->toLocal(inPanel) );
  unsigned int pixelRow = (unsigned int)
    floor((pInMaPMT.y() - m_pixelVerticalEdge) / m_pixelPitch);
  unsigned int pixelColumn = (unsigned int)
    floor((pInMaPMT.x() - m_pixelHorizEdge) / m_pixelPitch);

  id = RichSmartID(0, 0, MaPMTRow, MaPMTColumn, pixelRow, pixelColumn);

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode DeRichMaPMTPanel::detectionPoint (const RichSmartID& smartID,
                                             HepPoint3D& windowHitGlobal) {

  unsigned int MaPMTNum = smartID.PDRow()*m_PDColumns + smartID.PDCol();
  if ( MaPMTNum >= m_PDMax ) {
    MsgStream log(msgSvc(), "DeRichMaPMTPanel" );
    log << MSG::ERROR << "Problem with MaPMT number" <<  endreq;
    windowHitGlobal = HepPoint3D(0.0, 0.0, 0.0);
    return StatusCode::FAILURE;
  }

  // here it is assumed that the x and y coordinates of the hit at the anode
  // are the same in the quartz window
  double x = (smartID.pixelCol()+0.5)*m_pixelPitch - m_pixelHorizEdge;
  double y = (smartID.pixelRow()+0.5)*m_pixelPitch - m_pixelVerticalEdge;
  double z = -m_quartzWinHalfLengthZ;

  HepPoint3D pointInPanel
    (m_pvMaPMTMasters[MaPMTNum]->toMother
     (m_pvMaPMTSMasters[MaPMTNum]->toMother
      (m_pvWindows[MaPMTNum]->toMother(HepPoint3D(x,y,z)))));

  windowHitGlobal = geometry()->toGlobal(pointInPanel);

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode DeRichMaPMTPanel::PDWindowPoint( const HepVector3D& vGlobal,
                                            const HepPoint3D& pGlobal,
                                            HepPoint3D& windowPointGlobal,
                                            RichSmartID& smartID ) {

  // transform point and vector to the MaPMT Panel coordsystem.
  HepPoint3D pLocal( geometry()->toLocal(pGlobal) );
  HepVector3D vLocal( vGlobal );
  vLocal.transform( m_vectorTransf );


  double scalar1 = vLocal*m_localPlaneNormal;
  double distance = 0.0;
  if ( scalar1 == 0.0 ) return false;

  distance = -(m_localPlane.d() + pLocal*m_localPlaneNormal) / scalar1;
  HepPoint3D panelIntersection( pLocal + distance*vLocal );

  if ( fabs(panelIntersection.x()) >= m_abs_panelHorizEdge ||
       fabs(panelIntersection.y()) >= m_abs_panelVerticalEdge ) {
    smartID = RichSmartID(0);
    return StatusCode::FAILURE;
  }

  unsigned int MaPMTRow, MaPMTColumn;

  MaPMTRow = (unsigned int) floor((panelIntersection.y() -
                                   m_panelVerticalEdge)/ m_rowPitch);
  MaPMTColumn = (unsigned int) floor((panelIntersection.x() -
                                      m_panelHorizEdge) / m_columnPitch);

  unsigned int MaPMTNum = MaPMTRow*m_PDColumns + MaPMTColumn;
  if ( MaPMTNum >= m_PDMax ) {
    MsgStream log(msgSvc(), "DeRichMaPMTPanel" );
    log << MSG::ERROR << "Problem with MaPMT number" <<  endreq;
    smartID = RichSmartID(0);
    return StatusCode::FAILURE;
  }

  // convert the point to the window coordinates (of the relevant MaPMT)
  HepPoint3D pInWindow (m_pvWindows[MaPMTNum]->toLocal
                        (m_pvMaPMTSMasters[MaPMTNum]->toLocal
                         (m_pvMaPMTMasters[MaPMTNum]->toLocal(pLocal))));
  // conver vector to MaPMT coordinates
  HepVector3D vInMaPMTMaster = vLocal;
  vInMaPMTMaster.transform( m_vectorTransfMaPMT2s[MaPMTNum] );


  // See if the point is in the central PD
  unsigned int noTicks;
  ISolid::Ticks windowTicks;
  noTicks = m_windowSolids[MaPMTNum]->intersectionTicks(pInWindow,
                                                        vInMaPMTMaster,
                                                        windowTicks );
  if ( 0 == noTicks ) return StatusCode::FAILURE;
 
  HepPoint3D windowPoint = pInWindow + windowTicks[1]*vInMaPMTMaster;

  HepPoint3D windowPointInPanel
    (m_pvMaPMTMasters[MaPMTNum]->toMother
     (m_pvMaPMTSMasters[MaPMTNum]->toMother
      (m_pvWindows[MaPMTNum]->toMother(windowPoint))));

  windowPointGlobal = geometry()->toGlobal(windowPointInPanel);

  smartID.setPDRow( MaPMTRow );
  smartID.setPDCol( MaPMTColumn );
  // For the moment do not bother with pixel info
  smartID.setPixelRow( 0 );
  smartID.setPixelCol( 0 );

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode DeRichMaPMTPanel::readoutChannelList(std::vector<RichSmartID>&
                                                readoutChannels) {

  for (unsigned int PDRow = 0; PDRow < m_PDRows; ++PDRow)
    for (unsigned int PDCol = 0; PDCol < m_PDColumns; ++PDCol)
      for (unsigned int pixelRow = 0; pixelRow < m_pixelRows; ++pixelRow)
        for (unsigned int pixelCol = 0; pixelCol < m_pixelCols; ++pixelCol) {
          readoutChannels.push_back( RichSmartID(0,0,PDRow,PDCol,pixelRow,
                                                 pixelCol) );
        }


  return StatusCode::SUCCESS;
}

//=============================================================================

bool DeRichMaPMTPanel::validateCdfHit(const HepPoint3D& pGlobal,
                                      RichSmartID& smartID ) {

  // transform point and vector to the MaPMT Panel coordsystem.
  HepPoint3D pInPanel( geometry()->toLocal(pGlobal) );

  if ( fabs(pInPanel.x()) >= m_abs_panelHorizEdge ||
       fabs(pInPanel.y()) >= m_abs_panelVerticalEdge ) {
    //MsgStream log(msgSvc(), "DeRichMaPMTPanel" );
    //log << MSG::WARNING << "Failed panel global->local position" << endreq;
    smartID = RichSmartID(0);
    return false;
  }

  unsigned int MaPMTRow, MaPMTColumn;
  MaPMTRow = (unsigned int) floor((pInPanel.y() -
                                   m_panelVerticalEdge)/ m_rowPitch);
  MaPMTColumn = (unsigned int) floor((pInPanel.x() -
                                      m_panelHorizEdge) / m_columnPitch);

  unsigned int MaPMTNum = MaPMTRow*m_PDColumns + MaPMTColumn;
  if (MaPMTNum >= m_PDMax) {
    MsgStream log(msgSvc(), "DeRichMaPMTPanel" );
    log << MSG::ERROR << "Problem with MaPMT number" <<  endreq;
    smartID = RichSmartID(0);
    return false;
  }

  // convert the point to the hit MaPMT coordinates
  HepPoint3D pInMaPMT (m_pvMaPMTMasters[MaPMTNum]->toLocal(pInPanel));

  // check if point is withing the quartz window dimensions
  if (fabs(pInMaPMT.x()) >= m_cdfPixelHorizEdge ||
      fabs(pInMaPMT.y()) >= m_cdfPixelVerticalEdge ) {
    smartID = RichSmartID(0);
    return false;
  }

  unsigned int pixelRow = (unsigned int)
    floor((m_cdfPixelVerticalEdge + pInMaPMT.y()) / m_cdfPixelPitch);
  unsigned int pixelColumn = (unsigned int)
    floor((m_cdfPixelHorizEdge + pInMaPMT.x()) / m_cdfPixelPitch);

  double x = (pixelColumn+0.5)*m_cdfPixelPitch - m_cdfPixelHorizEdge;
  double y = (pixelRow+0.5)*m_cdfPixelPitch - m_cdfPixelVerticalEdge;

  if ( fabs(pInMaPMT.x() - x) > m_cdfPixelSize/2.0 ||
       fabs(pInMaPMT.y() - y) > m_cdfPixelSize/2.0 ) {
    smartID = RichSmartID(0);
    return false;
  }

  smartID = RichSmartID(0, 0, MaPMTRow, MaPMTColumn, pixelRow, pixelColumn);
  return true;
}

//=============================================================================
bool DeRichMaPMTPanel::cdfDetectionPoint (const RichSmartID& smartID,
                                          HepPoint3D& windowHitGlobal) {

  unsigned int MaPMTNum = smartID.PDRow()*m_PDColumns + smartID.PDCol();
  if (MaPMTNum >= m_PDMax) {
    MsgStream log(msgSvc(), "DeRichMaPMTPanel" );
    log << MSG::ERROR << "Problem with MaPMT number" <<  endreq;
    windowHitGlobal = HepPoint3D(0.0, 0.0, 0.0);
    return false;
  }

  // here it is assumed that the x and y coordinates of the hit at the anode
  // are the same in the quartz window
  double x = (smartID.pixelCol()+0.5)*m_cdfPixelPitch - m_cdfPixelHorizEdge;
  double y = (smartID.pixelRow()+0.5)*m_cdfPixelPitch - m_cdfPixelVerticalEdge;
  double z = -m_quartzWinHalfLengthZ;

  HepPoint3D pointInPanel
    (m_pvMaPMTMasters[MaPMTNum]->toMother
     (m_pvMaPMTSMasters[MaPMTNum]->toMother
      (m_pvWindows[MaPMTNum]->toMother(HepPoint3D(x,y,z)))));

  windowHitGlobal = geometry()->toGlobal(pointInPanel);

  return true;
}

