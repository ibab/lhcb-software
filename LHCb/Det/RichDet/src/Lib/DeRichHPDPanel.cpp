
/** @file DeRichHPDPanel.cpp
 *
 *  Implementation file for detector description class : DeRichHPDPanel
 *
 *  CVS Log :-
 *  $Id: DeRichHPDPanel.cpp,v 1.18 2004-10-20 17:02:44 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.17  2004/07/27 08:55:23  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICHHPDPANEL_CPP

// Include files
#include "RichDet/DeRichHPDPanel.h"

// CLHEP files
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"

// DetDesc
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/TabulatedProperty.h"

//----------------------------------------------------------------------------

// Standard Constructor
DeRichHPDPanel::DeRichHPDPanel() {}

// Standard Destructor
DeRichHPDPanel::~DeRichHPDPanel() {}

//=========================================================================
//  Initialize
//=========================================================================
StatusCode DeRichHPDPanel::initialize() {

  MsgStream log(msgSvc(), "DeRichHPDPanel" );

  // store the name of the panel, without the /dd/Structure part
  const std::string::size_type pos = name().find("Rich");
  if( std::string::npos != pos ) {
    m_name = name().substr(pos);
  }
  else
    m_name = "NO_NAME";

  log << MSG::DEBUG <<"Initializing " << m_name << endreq;

  SmartDataPtr<DetectorElement> deRich1(dataSvc(), DeRichLocation::Rich1);
  m_pixelSize = deRich1->userParameterAsDouble("RichHpdPixelXsize");
  m_subPixelSize = m_pixelSize/8;
  m_activeRadius = deRich1->userParameterAsDouble("RichHpdActiveInpRad");
  m_activeRadiusSq = m_activeRadius*m_activeRadius;

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

  const HepPoint3D zero(0.0, 0.0, 0.0);

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
    return StatusCode::FAILURE;
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

  // store the z coordinate of the detection plane
  m_detPlaneZ = pointAInPanel.z();

  // localPlane2 is used when trying to locate the HPD row/column from
  // a point in the panel.
  m_localPlane2 = m_localPlane;
  m_detPlaneZdiff = m_winR - sqrt( m_winRsq - m_activeRadiusSq );
  m_localPlane2.transform(HepTranslateZ3D(m_detPlaneZdiff));
  m_localPlaneNormal2 = m_localPlane2.normal();
  //  std::cout << "m_localPlane2" << m_localPlane2 << std::endl;

  // Cache information for PDWindowPoint method
  m_vectorTransf = geometry()->matrix();
  m_HPDPanelSolid = geometry()->lvolume()->solid();

  for ( unsigned int HPD = 0; HPD < PDMax(); ++HPD ) {
    const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPD);
    m_HPDCentres.push_back( pvHPDMaster->toMother(zero) );
  }

  log << MSG::DEBUG << "Finished initialization" << endreq;
  return StatusCode::SUCCESS;
}

//=========================================================================
// convert a point on the silicon sensor to smartID
//=========================================================================
StatusCode DeRichHPDPanel::smartID (const HepPoint3D& globalPoint,
                                    RichSmartID& id) const
{

  HepPoint3D inPanel = geometry()->toLocal(globalPoint);

  // find the HPD row/col of this point
  if ( !findHPDRowCol(inPanel, id) ) return StatusCode::FAILURE;

  const unsigned int HPDNumber = HPDRowColToNum(id.PDRow(), id.PDCol());

  const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
  const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
  const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume(4);
  //  const ISolid* siliconSolid = pvSilicon->lvolume()->solid();

  HepPoint3D inSilicon =
    pvSilicon->toLocal(pvHPDSMaster->toLocal(pvHPDMaster->toLocal(inPanel)));

  double inSiliconX = inSilicon.x();
  double inSiliconY = inSilicon.y();

  if ( fabs(inSiliconX) > m_siliconHalfLengthX ) {
    const int signX = ( inSiliconX > 0.0 ? 1 : -1 );
    inSiliconX -= signX*0.001*mm;
  }

  if (fabs(inSiliconY) > m_siliconHalfLengthY ) {
    const int signY = ( inSiliconY > 0.0 ? 1 : -1 );
    inSiliconY -= signY*0.001*mm;
  }

  if ( (fabs(inSiliconX) > m_siliconHalfLengthX) ||
       (fabs(inSiliconY) > m_siliconHalfLengthY)    ) {
    MsgStream log(msgSvc(), m_name );
    log << MSG::ERROR << "Point " << inSilicon << " is outside the silicon box "
        << pvHPDMaster->name() << endreq;
    return StatusCode::FAILURE;
  }

  const unsigned int pixelColumn = static_cast<unsigned int>
    ((m_siliconHalfLengthX + inSiliconX) / m_pixelSize);
  const unsigned int pixelRow    = static_cast<unsigned int>
    ((m_siliconHalfLengthY + inSiliconY) / m_pixelSize);

  id.setPixelRow(pixelRow);
  id.setPixelCol(pixelColumn);

  const unsigned int subPixel = static_cast<unsigned int>
    ((m_siliconHalfLengthX+inSiliconX-pixelColumn*m_pixelSize) /
     m_subPixelSize);
  id.setSubPixel( subPixel );

  return StatusCode::SUCCESS;
}



//=========================================================================
//  convert a smartID to a point on the inside of the HPD window
//=========================================================================
StatusCode DeRichHPDPanel::detectionPoint (const RichSmartID& smartID,
                                           HepPoint3D& windowHitGlobal) const
{

  const unsigned int HPDNumber=HPDRowColToNum(smartID.PDRow(),smartID.PDCol());

  // find the correct HPD and silicon block inside it
  const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
  const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
  const IPVolume* pvWindow = pvHPDSMaster->lvolume()->pvolume(2);

  // convert pixel number to silicon coordinates
  const double inSiliconX = smartID.pixelCol() * m_pixelSize+m_pixelSize/2.0 -
    m_siliconHalfLengthX;
  const double inSiliconY = smartID.pixelRow() * m_pixelSize+m_pixelSize/2.0 -
    m_siliconHalfLengthY;

  const double inWindowX = -inSiliconX / m_deMagFactor[0];
  const double inWindowY = -inSiliconY / m_deMagFactor[0];
  const double inWindowZ = sqrt(m_winRsq-inWindowX*inWindowX-inWindowY*inWindowY);
  HepPoint3D windowHit(inWindowX, inWindowY, inWindowZ);

  windowHitGlobal =
    geometry()->toGlobal(pvHPDMaster->
                         toMother(pvHPDSMaster->
                                  toMother(pvWindow->toMother(windowHit))));

  return StatusCode::SUCCESS;
}


//=========================================================================
//  convert a point from the global to the panel coodinate system
//=========================================================================
HepPoint3D DeRichHPDPanel::globalToPDPanel( const HepPoint3D& globalPoint ) const 
{
  const HepPoint3D localPoint( geometry()->toLocal( globalPoint ) );
  return HepPoint3D(localPoint.x(), localPoint.y(),
                    localPoint.z() - m_detPlaneZ );
}


//=========================================================================
//  find an intersection with the inside of the HPD window
//=========================================================================
StatusCode DeRichHPDPanel::PDWindowPoint( const HepVector3D& vGlobal,
                                          const HepPoint3D& pGlobal,
                                          HepPoint3D& windowPointGlobal,
                                          RichSmartID& smartID,
                                          const RichTraceMode mode) const 
{

  // transform point and vector to the HPDPanel coordsystem.
  HepPoint3D pLocal = geometry()->toLocal(pGlobal);
  HepVector3D vLocal = vGlobal;
  vLocal.transform(m_vectorTransf);

  // find the intersection with the detection plane (localPlane2)
  double scalar = vLocal*m_localPlaneNormal2;
  if ( scalar == 0.0 ) return StatusCode::FAILURE;

  double distance = -m_localPlane2.distance( pLocal )/scalar;
  HepPoint3D panelIntersection( pLocal + distance*vLocal );

  unsigned int  HPDNumber(0), HPDRow(0), HPDColumn(0);
  RichSmartID id;

  if ( mode.detPrecision() == RichTraceMode::circle ) {  // do it quickly
    if ( findHPDRowCol(panelIntersection, id) ) {

      HPDRow = id.PDRow();
      HPDColumn = id.PDCol();
      HPDNumber = HPDRowColToNum(HPDRow, HPDColumn);

      const double x = panelIntersection.x() - m_HPDCentres[HPDNumber].x();
      const double y = panelIntersection.y() - m_HPDCentres[HPDNumber].y();
      if ( ( x*x + y*y ) > m_activeRadiusSq ) return StatusCode::FAILURE;

      windowPointGlobal = geometry()->toGlobal( panelIntersection );

      smartID.setPDRow( HPDRow );
      smartID.setPDCol( HPDColumn );

      return StatusCode::SUCCESS;
    }
    return StatusCode::FAILURE;
  }

  const IPVolume* pvHPDMaster = 0;
  const IPVolume* pvHPDSMaster = 0;
  const IPVolume* pvWindow = 0;
  const ISolid* windowSolid;

  HepPoint3D pInWindow;
  HepVector3D vInHPDMaster;

  ISolid::Ticks HPDWindowTicks;

  unsigned int noTicks;

  bool HPDFound(false);

  // Overwise slow
  if ( findHPDRowCol(panelIntersection, id) ) {

    HPDRow = id.PDRow();
    HPDColumn = id.PDCol();
    HPDNumber = HPDRowColToNum(HPDRow, HPDColumn);

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
    const HepTransform3D & vectorTransfHPD = pvHPDMaster->matrix();
    vInHPDMaster = vLocal;
    vInHPDMaster.transform(vectorTransfHPD);

    noTicks = windowSolid->intersectionTicks(pInWindow, vInHPDMaster,
                                             HPDWindowTicks );
    if ( 0 != noTicks ) {
      HPDFound = true;
    }

  }

  if ( !HPDFound ) return StatusCode::FAILURE;

  HepPoint3D windowPoint = pInWindow + HPDWindowTicks[1]*vInHPDMaster;
  HepPoint3D windowPointInHPD = pvHPDSMaster->
    toMother(pvWindow->toMother(windowPoint));
  // check the active radius.
  const double hitRadius2 = ( windowPointInHPD.x()*windowPointInHPD.x() +
                              windowPointInHPD.y()*windowPointInHPD.y() );
  if ( hitRadius2 > m_activeRadiusSq ) return StatusCode::FAILURE;

  windowPointGlobal =
    geometry()->toGlobal( pvHPDMaster->toMother(windowPointInHPD) );

  smartID.setPDRow( HPDRow );
  smartID.setPDCol( HPDColumn );

  return StatusCode::SUCCESS;

}


//=========================================================================
//  return a list with all the valid readout channels (smartIDs)
//=========================================================================
StatusCode DeRichHPDPanel::readoutChannelList(std::vector<RichSmartID>&
                                              readoutChannels) {

  double activeRadius = m_activeRadius * m_deMagFactor[0];
  double xcorn = 0;
  double ycorn = 0;

  for (unsigned int PD = 0; PD < PDMax(); ++PD) {
    for (unsigned int pixRow = 0; pixRow < m_pixelRows; ++pixRow ) {
      for (unsigned int pixCol = 0; pixCol < m_pixelColumns; ++pixCol ) {

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

        const double radcorn = sqrt(xcorn*xcorn + ycorn*ycorn) ;
        if(radcorn <= (activeRadius) ) {
          RichSmartID id(0,0,PDRow(PD),PDCol(PD),pixRow,pixCol);
          readoutChannels.push_back(id);
        }
      }

    }

  }

  return StatusCode::SUCCESS;
}


//=========================================================================
//  returns the intersection point with the detection plane
//=========================================================================
bool DeRichHPDPanel::detPlanePoint( const HepPoint3D& pGlobal,
                                    const HepVector3D& vGlobal,
                                    HepPoint3D& hitPosition,
                                    const RichTraceMode mode) const 
{

  // transform point and vector to the MaPMT Panel coordsystem.
  const HepPoint3D pLocal( geometry()->toLocal(pGlobal) );
  HepVector3D vLocal( vGlobal );
  vLocal.transform( m_vectorTransf );

  const double scalar1 = vLocal*m_localPlaneNormal;
  if ( scalar1 == 0.0 ) return false;

  const double distance = -(m_localPlane.d() + pLocal*m_localPlaneNormal) / scalar1;
  HepPoint3D hitInPanel( pLocal + distance*vLocal );

  if ( mode.detPlaneBound() == RichTraceMode::tight) {
    if ( fabs(hitInPanel.x()) >= m_detPlaneHorizEdge ||
         fabs(hitInPanel.y()) >= m_detPlaneVertEdge ) {
      return false;
    }
  }

  hitPosition = geometry()->toGlobal( hitInPanel );
  return true;
}

