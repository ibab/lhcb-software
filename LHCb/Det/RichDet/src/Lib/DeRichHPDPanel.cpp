// $Id: DeRichHPDPanel.cpp,v 1.7 2003-11-21 17:23:26 papanest Exp $
#define DERICHHPDPANEL_CPP

// Include files
#include "RichDet/DeRichHPDPanel.h"

// CLHEP files
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/TabulatedProperty.h"

//----------------------------------------------------------------------------
//
// Implementation of class :  DeRichHPDPanel
//
//----------------------------------------------------------------------------

// Standard Constructor
DeRichHPDPanel::DeRichHPDPanel()
{}

// Standard Destructor
DeRichHPDPanel::~DeRichHPDPanel() {
  /*  
  double rate = 0;
  std::cout << "DeRichHPDPanel Stats" << std::endl;

  std::cout << "Tries           = " << m_nTries << std::endl;

  rate = 100*( m_nTries>0 ? (double)m_failPanel/(double)m_nTries : 0 );
  std::cout << "Failed Panel    = " << m_failPanel << " " << rate << " % " 
            << std::endl;

  rate = 100*( m_nTries>0 ? (double)m_nTryClever/(double)m_nTries : 0 );
  std::cout << "Try clever      = " << m_nTryClever << " " << rate << " % " 
            << std::endl;

  rate = 100*( m_nTries>0 ? (double)m_nFoundByClever/(double)m_nTries : 0 );
  std::cout << "Found by clever = " << m_nFoundByClever << " " << rate 
            << " % " << std::endl;

  rate = 100*( m_nTries>0 ? (double)m_nTrySimple/(double)m_nTries : 0 );
  std::cout << "Try simple      = " << m_nTrySimple << " " << rate << " % " 
            << std::endl;

  rate = 100*( m_nTries>0 ? (double)m_nFoundBySimple/(double)m_nTries : 0 );
  std::cout << "Found by simple = " << m_nFoundBySimple << " " << rate 
            << " % " << std::endl;

  rate = 100*( m_nTries>0 ? (double)m_nSuccess/(double)m_nTries : 0 );
  std::cout << "Success         = " << m_nSuccess << " " << rate << " % " 
            << std::endl;
  */  
}

//=========================================================================
//  Initialize
//=========================================================================
StatusCode DeRichHPDPanel::initialize() {

  StatusCode fail = StatusCode::FAILURE;
  MsgStream log(msgSvc(), "DeRichHPDPanel" );

  // store the name of the panel, without the /dd/Structure part
  const std::string::size_type pos = name().find("Rich");
  if( std::string::npos != pos ) {
    m_name = name().substr(pos);
  }
  else
    m_name = "NO_NAME";

  log << MSG::DEBUG <<"Initializing " << m_name << endreq;

  SmartDataPtr<DetectorElement> deRich1(dataSvc(), "/dd/Structure/LHCb/Rich1");
  m_pixelSize = deRich1->userParameterAsDouble("RichHpdPixelXsize");
  m_subPixelSize = m_pixelSize/8;
  m_activeRadius = deRich1->userParameterAsDouble("RichHpdActiveInpRad");

  m_pixelColumns = deRich1->userParameterAsInt("RichHpdNumPixelCol");
  m_pixelRows = deRich1->userParameterAsInt("RichHpdNumPixelRow");
  log << MSG::DEBUG << "pixelRows:" << m_pixelRows << " pixelColumns:"
      << m_pixelColumns << endreq;
  
  m_HPDRows = userParameterAsInt("PDRows");
  m_HPDColumns = userParameterAsInt("PDColumns");
  log << MSG::DEBUG << "RichHpdPixelsize:" << m_pixelSize << " ActiveRadius:"
      << m_activeRadius << " PDRows:" << m_HPDRows << " PDColumns:"
      << m_HPDColumns << endreq;

  /// the next two variables are used in cdf type panel
  m_HPDsInBigCol = static_cast<int>(ceil(m_HPDRows/2.0));
  m_HPDsIn2Cols = 2*m_HPDsInBigCol - 1;

  SmartDataPtr<TabulatedProperty> 
    HPDdeMag(dataSvc(),
             "/dd/Materials/RichMaterialTabProperties/HpdDemagnification");
  TabulatedProperty::Table DeMagTable = HPDdeMag->table();
  m_deMagFactor[0] = DeMagTable[0].second;
  m_deMagFactor[1] = DeMagTable[1].second;
  log << MSG::DEBUG << "DeMagFactor(0):" << m_deMagFactor[0] 
      << " DeMagFactor(1):" << m_deMagFactor[1] << endreq;

  HepPoint3D zero(0.0, 0.0, 0.0);

  // get the first HPD and follow down to the silicon block
  const IPVolume* pvHPDMaster0 = geometry()->lvolume()->pvolume(0);
  const IPVolume* pvHPDSMaster0 = pvHPDMaster0->lvolume()->pvolume(0);
  const IPVolume* pvSilicon0 = pvHPDSMaster0->lvolume()->pvolume(4);

  const ISolid* siliconSolid = pvSilicon0->lvolume()->solid();
  log << MSG::DEBUG << "About to do a dynamic cast SolidBox" << endreq;
  const SolidBox* siliconBox = dynamic_cast<const SolidBox*>(siliconSolid);

  // assume same size for all silicon detectors
  m_siliconHalfLengthX = siliconBox->xHalfLength();
  m_siliconHalfLengthY = siliconBox->yHalfLength();

  // HPD #0 coordinates
  m_HPD0Centre = pvHPDMaster0->toMother(zero);
  log << MSG::DEBUG << "Centre of HPDPanel:" << geometry()->toGlobal(zero)
      <<endreq;
  log << MSG::DEBUG<< "Centre of HPD#0:" << geometry()->toGlobal(m_HPD0Centre)
      <<endreq;
  
  //get the next HPD. For Rich1 same row, for Rich2 same column
  const IPVolume* pvHPDMaster1 = geometry()->lvolume()->pvolume(1);
  m_HPD1Centre = pvHPDMaster1->toMother(zero);

  //get the HPD at next row for Rich1, next column for Rich2, NS == next set
  const IPVolume* pvHPDMasterNS = geometry()->lvolume()->pvolume(HPDForNS());
  m_HPDNSCentre = pvHPDMasterNS->toMother(zero);

  // get the pv and the solid for the HPD quartz window
  const IPVolume* pvWindow0 = pvHPDSMaster0->lvolume()->pvolume(2);
  const ISolid* windowSolid0 = pvWindow0->lvolume()->solid();
  // get the inside radius of the window
  ISolid::Ticks windowTicks;
  unsigned int windowTicksSize = windowSolid0->
    intersectionTicks(HepPoint3D(0.0, 0.0, 0.0),HepVector3D(0.0, 0.0, 1.0),
                      windowTicks);
  if (windowTicksSize != 2) {
    log << MSG::FATAL << "Problem getting window radius" << endreq;
    return fail;
  }
  m_winR = windowTicks[0];
  m_winRsq = m_winR*m_winR;
  
  // get the coordinate of the centre of the HPD quarz window
  HepPoint3D HPDTop1(0.0, 0.0, m_winR);
  // convert this to HPDS master  coordinates
  HepPoint3D HPDTop2 = pvWindow0->toMother(HPDTop1);
  // and to silicon
  m_HPDTop = pvSilicon0->toLocal(HPDTop2);

  // find the top of 3 HPDs to create a detection plane.  We already have the
  // first in HPDSMaster coordinates.
  // now to HPD coordinates
  HepPoint3D pointAInHPD = pvHPDSMaster0->toMother(HPDTop2);
  HepPoint3D pointAInPanel = pvHPDMaster0->toMother(pointAInHPD);
  HepPoint3D pointA = geometry()->toGlobal(pointAInPanel);

  // for second point go to HPD at the end of the column.
  //The relative position inside the HPD is the same
  const IPVolume* pvHPDMasterB = geometry()->lvolume()->pvolume(HPDForB());
  HepPoint3D pointBInPanel = pvHPDMasterB->toMother(pointAInHPD);
  HepPoint3D pointB = geometry()->toGlobal(pointBInPanel);

  // now point C at the other end.
  const IPVolume* pvHPDMasterC = geometry()->lvolume()->pvolume(HPDForC());
  HepPoint3D pointCInPanel = pvHPDMasterC->toMother(pointAInHPD);
  HepPoint3D pointC = geometry()->toGlobal(pointCInPanel);

  m_detectionPlane = HepPlane3D(pointA, pointB, pointC);
  m_detectionPlane.normalize();
  m_localPlane = HepPlane3D(pointAInPanel, pointBInPanel, pointCInPanel);
  m_localPlane.normalize();
  m_localPlaneNormal = m_localPlane.normal();
  //  std::cout << "detectionPlane " <<  m_detectionPlane << std::endl;
  //  std::cout << "localDetectionPlane " <<  m_localPlane << std::endl;

  // localPlane2 is used when trying to locate the HPD row/column from 
  // a point in the panel.
  m_localPlane2 = m_localPlane;
  double movePlaneBack = m_winR - sqrt(m_winRsq - 
                                       m_activeRadius*m_activeRadius);
  m_localPlane2.transform(HepTranslateZ3D(movePlaneBack));
  m_localPlaneNormal2 = m_localPlane2.normal();
  //  std::cout << "m_localPlane2" << m_localPlane2 << std::endl;
  
  // Cache information for PDWindowPoint method
  m_vectorTransf = geometry()->matrix();
  m_HPDPanelSolid = geometry()->lvolume()->solid();

  /*
  for ( unsigned int HPD = 0; HPD < PDMax(); ++HPD ) {
    m_pvHPDMasters.push_back( geometry()->lvolume()->pvolume(HPD) );
    m_pvHPDSMasters.push_back( m_pvHPDMasters[HPD]->lvolume()->pvolume(0));
    m_pvWindows.push_back( m_pvHPDSMasters[HPD]->lvolume()->pvolume(2) );
    m_windowSolids.push_back( m_pvWindows[HPD]->lvolume()->solid() );
    m_vectorTransfHPD2s.push_back( m_pvHPDMasters[HPD]->matrix() );
  }
  */
  
  for ( unsigned int HPD = 0; HPD < PDMax(); ++HPD ) {
    const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPD);
    m_HPDCentres.push_back( pvHPDMaster->toMother(zero) );
  }
  
  //  m_nTries = 0;
  //  m_nSuccess = 0;
  //  m_failPanel = 0;
  //  m_nFoundBySimple = 0;
  //  m_nFoundByClever = 0;
  //  m_nTryClever = 0;
  //  m_nTrySimple = 0;

  log << MSG::DEBUG << "Finished initialization" << endreq;
  return StatusCode::SUCCESS;
}


//=========================================================================
// convert a point on the silicon sensor to smartID 
//=========================================================================
StatusCode DeRichHPDPanel::smartID (const HepPoint3D& globalPoint,
                                    RichSmartID& id) {


  HepPoint3D inPanel = geometry()->toLocal(globalPoint);

  // find the HPD row/col of this point
  if ( !findHPDRowCol(inPanel, id) ) return StatusCode::FAILURE;

  unsigned int HPDNumber = HPDRowColToNum(id.PDRow(), id.PDCol());
  
  const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
  const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
  const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume(4);
  //  const ISolid* siliconSolid = pvSilicon->lvolume()->solid();

  HepPoint3D inHPDMaster = pvHPDMaster->toLocal(inPanel);
  HepPoint3D inHPDSMaster = pvHPDSMaster->toLocal(inHPDMaster);
  HepPoint3D inSilicon = pvSilicon->toLocal(inHPDSMaster);

  if ( (fabs(inSilicon.x()) > m_siliconHalfLengthX) ||
       (fabs(inSilicon.y()) > m_siliconHalfLengthY)    ) {
    MsgStream log(msgSvc(), m_name );
    log << MSG::ERROR << "The point is outside the silicon box "
        << pvHPDMaster->name() <<  endreq;
    std::cout << inSilicon << std::endl;
    return StatusCode::FAILURE;
  }

  unsigned int pixelColumn = static_cast<unsigned int>
    ((m_siliconHalfLengthX + inSilicon.x()) / m_pixelSize);
  unsigned int pixelRow    = static_cast<unsigned int>
    ((m_siliconHalfLengthY + inSilicon.y()) / m_pixelSize);

  //std::cout << pixelColumn << "  " <<pixelRow << std::endl;

  id.setPixelRow(pixelRow);
  id.setPixelCol(pixelColumn);

  unsigned int subPixel = static_cast<unsigned int>
    ((m_siliconHalfLengthX+inSilicon.x()-pixelColumn*m_pixelSize) /
     m_subPixelSize);
  id.setSubPixel( subPixel );
  
  //  std::cout << subPixel << std::endl;
  
  return StatusCode::SUCCESS;
  
}



//=========================================================================
//  convert a smartID to a point on the inside of the HPD window
//=========================================================================
StatusCode DeRichHPDPanel::detectionPoint (const RichSmartID& smartID,
                                           HepPoint3D& windowHitGlobal)
{
  
  unsigned int HPDNumber = HPDRowColToNum( smartID.PDRow(), smartID.PDCol());

  // find the correct HPD and silicon block inside it
  const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
  const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
  const IPVolume* pvWindow = pvHPDSMaster->lvolume()->pvolume(2);

  // convert pixel number to silicon coordinates
  double inSiliconX = smartID.pixelCol() * m_pixelSize + m_pixelSize/2.0 -
    m_siliconHalfLengthX;
  double inSiliconY = smartID.pixelRow() * m_pixelSize + m_pixelSize/2.0 -
    m_siliconHalfLengthY;

  double inWindowX = -inSiliconX / m_deMagFactor[0];
  double inWindowY = -inSiliconY / m_deMagFactor[0];
  double inWindowZ = sqrt(m_winRsq-inWindowX*inWindowX-inWindowY*inWindowY);
  HepPoint3D windowHit(inWindowX, inWindowY, inWindowZ);
  //  std::cout << windowHit << std::endl;

  HepPoint3D windowHitInHPDS = pvWindow->toMother(windowHit);
  HepPoint3D windowHitInHPD = pvHPDSMaster->toMother(windowHitInHPDS);
  HepPoint3D windowHitInPanel = pvHPDMaster->toMother(windowHitInHPD);
  windowHitGlobal = geometry()->toGlobal(windowHitInPanel);
  
  return StatusCode::SUCCESS;
}


//=========================================================================
//  find an intersection with the inside of the HPD window
//=========================================================================
StatusCode DeRichHPDPanel::PDWindowPoint( const HepVector3D& vGlobal,
                                          const HepPoint3D& pGlobal,
                                          HepPoint3D& windowPointGlobal,
                                          RichSmartID& smartID,
                                          DeRichPDPanel::traceMode mode) {
  
  //  ++m_nTries;
  // transform point and vector to the HPDPanel coordsystem.
  HepPoint3D pLocal = geometry()->toLocal(pGlobal);
  HepVector3D vLocal = vGlobal;
  vLocal.transform(m_vectorTransf);

  // find the intersection with the detection plane (localPlane2)
  double scalar1 = vLocal*m_localPlaneNormal2;
  double distance = 0.0;

  if ( scalar1 == 0.0 ) return StatusCode::FAILURE;

  distance = -(m_localPlane2.d() + pLocal*m_localPlaneNormal2) / scalar1;
  HepPoint3D panelIntersection( pLocal + distance*vLocal );

  const IPVolume* pvHPDMaster = 0;
  const IPVolume* pvHPDSMaster = 0;
  const IPVolume* pvWindow = 0;
  const ISolid* windowSolid;

  HepPoint3D pInWindow;
  HepVector3D vInHPDMaster;

  ISolid::Ticks HPDWindowTicks;

  unsigned int  HPDNumber(0), HPDRow(0), HPDColumn(0);
  bool HPDFound(false);
  //  bool found2(false);
  unsigned int noTicks;

  RichSmartID id;
  if ( findHPDRowCol(panelIntersection, id) ) {

    //    ++m_nTryClever;

    HPDRow = id.PDRow();
    HPDColumn = id.PDCol();
    HPDNumber = HPDRowColToNum(HPDRow, HPDColumn);

    if ( mode == circle ) {  // do it quickly
      double x = panelIntersection.x() - m_HPDCentres[HPDNumber].x();
      double y = panelIntersection.y() - m_HPDCentres[HPDNumber].y();
      double hitRadius = sqrt( x*x + y*y );
      if (hitRadius > m_activeRadius ) return StatusCode::FAILURE;
      windowPointGlobal = geometry()->toGlobal( panelIntersection );
      
      smartID.setPDRow( HPDRow );
      smartID.setPDCol( HPDColumn );
      // For the moment do not bother with pixel info
      smartID.setPixelRow( 0 );
      smartID.setPixelCol( 0 );
      
      return StatusCode::SUCCESS;
    }
    
    // or do it slowly and exactly  

    // find the correct HPD and quartz window inside it
    pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
    // just in case
    if ( !pvHPDMaster ) {
      MsgStream log(msgSvc(), m_name );
      log << MSG::ERROR << "Inappropriate HPDNumber:" << HPDNumber
          << " from HPDRow:" << id.PDRow() << " and HPDColumn:" << id.PDCol()
          << " please notify Antonis" << endreq
          << " x:" << panelIntersection.x()
          << " y:" << panelIntersection.y() <<  endreq;
      return StatusCode::FAILURE;
    }
      
    pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
    pvWindow = pvHPDSMaster->lvolume()->pvolume(2);
    windowSolid = pvWindow->lvolume()->solid();
    
    // convert point to local coordinate systems
    pInWindow = pvWindow->toLocal(pvHPDSMaster->toLocal(pvHPDMaster->
                                                        toLocal(pLocal)));
    // convert local vector assuming that only the HPD can be rotated
    const HepTransform3D vectorTransfHPD = pvHPDMaster->matrix();
    vInHPDMaster = vLocal;
    vInHPDMaster.transform(vectorTransfHPD);
    
    noTicks = windowSolid->intersectionTicks(pInWindow, vInHPDMaster,
                                             HPDWindowTicks );
    if ( 0 != noTicks ) {
      HPDFound = true;
      //      ++m_nFoundByClever;
    }
    
  }
  //  else {
    //    ++m_failPanel;
    //    return StatusCode::FAILURE;
  //  }
  
  
  /*  
  if ( !HPDFound ) {
    // Not in central HPD : Try all others
    ++m_nTrySimple;
    for ( unsigned int HPD = 0; HPD < PDMax(); ++HPD ) {
      // convert point to local coordinate systems
      pInWindow = m_pvWindows[HPD]->toLocal(m_pvHPDSMasters[HPD]->
                                            toLocal(m_pvHPDMasters[HPD]->
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
        HPDRow = PDRow(HPDNumber);
        HPDColumn = PDCol(HPDNumber);
        //        found2 = true;        
        break;
      }
    }
  }
  */
  
  if ( !HPDFound ) return StatusCode::FAILURE;


  HepPoint3D windowPoint = pInWindow + HPDWindowTicks[1]*vInHPDMaster;
  HepPoint3D windowPointInHPDS = pvWindow->toMother(windowPoint);
  HepPoint3D windowPointInHPD = pvHPDSMaster->toMother(windowPointInHPDS);
  // check the active radius.
  double hitRadius = sqrt( windowPointInHPD.x()*windowPointInHPD.x() +
                           windowPointInHPD.y()*windowPointInHPD.y() );
  if (hitRadius > m_activeRadius ) return StatusCode::FAILURE;
  
  HepPoint3D windowPointInPanel = pvHPDMaster->toMother(windowPointInHPD);
  
  //  if (found2) ++m_nFoundBySimple;
    
  windowPointGlobal = geometry()->toGlobal(windowPointInPanel);

  //  ++m_nSuccess;
  smartID.setPDRow( HPDRow );
  smartID.setPDCol( HPDColumn );
  // For the moment do not bother with pixel info
  smartID.setPixelRow( 0 );
  smartID.setPixelCol( 0 );
  
  return StatusCode::SUCCESS;

}


//=========================================================================
//  return a list with all the valid readout channels (smartIDs)
//=========================================================================
StatusCode DeRichHPDPanel::readoutChannelList(std::vector<RichSmartID>&
                                             readoutChannels) {

  double activeRadius = m_activeRadius * m_deMagFactor[0];
  double xcorn;
  double ycorn;

  for (unsigned int PD = 0; PD < PDMax(); ++PD) {
    for (unsigned int pixRow = 0; pixRow < m_pixelRows; pixRow++) {        
      for (unsigned int pixCol = 0; pixCol < m_pixelColumns; pixCol++) {

        double xpix = (pixRow + 0.5)*m_pixelSize - m_siliconHalfLengthX;
        double ypix = (pixCol + 0.5)*m_pixelSize - m_siliconHalfLengthY;
        
        if( xpix < 0.0 ) 
          xcorn = xpix + 0.5*m_pixelSize ;
        else
          xcorn = xpix - 0.5*m_pixelSize ;
        
        if( ypix < 0.0 )
          ycorn = ypix + 0.5*m_pixelSize ;
        else
          ycorn = ypix - 0.5*m_pixelSize ;
          
        double radcorn = sqrt(xcorn*xcorn + ycorn*ycorn) ;
        if(radcorn <= (activeRadius) ) {          
          RichSmartID id(0,0,PDRow(PD),PDCol(PD),pixRow,pixCol);
          readoutChannels.push_back(id);
        }
      }   

    }
        
  }

  return StatusCode::SUCCESS;
}

