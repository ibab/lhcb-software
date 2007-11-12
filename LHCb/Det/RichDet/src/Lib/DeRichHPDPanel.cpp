//----------------------------------------------------------------------------
/** @file DeRichHPDPanel.cpp
 *
 *  Implementation file for detector description class : DeRichHPDPanel
 *
 *  $Id: DeRichHPDPanel.cpp,v 1.61 2007-11-12 09:42:04 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

// Include files
#include <time.h>
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichSystem.h"

// MathCore files
#include "GaudiKernel/Transform3DTypes.h"

// DetDesc
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/TabulatedProperty.h"

#include "GaudiKernel/IUpdateManagerSvc.h"

// GSL
#include "gsl/gsl_math.h"

//----------------------------------------------------------------------------

const CLID CLID_DeRichHPDPanel = 12010;  // User defined

// Standard Constructor
DeRichHPDPanel::DeRichHPDPanel() :
  m_deRichS    (     NULL              ),
  m_rich       ( Rich::InvalidDetector ),
  m_side       ( Rich::InvalidSide     ),
  m_panelRichID( LHCb::RichSmartID()   ),
  m_kaptonSolid(     NULL              )
{}

// Standard Destructor
DeRichHPDPanel::~DeRichHPDPanel() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichHPDPanel::classID()
{
  return CLID_DeRichHPDPanel;
}

//=========================================================================
//  Initialize
//=========================================================================
StatusCode DeRichHPDPanel::initialize()
{

  // store the name of the panel, without the /dd/Structure part
  const std::string::size_type pos = name().find("Rich");
  m_name = ( std::string::npos != pos ? name().substr(pos) : "DeRichHPDPanel_NO_NAME" );

  MsgStream msg ( msgSvc(), "DeRichHPDPanel" );

  const Gaudi::XYZPoint zero(0.0, 0.0, 0.0);
  const Gaudi::XYZPoint centreGlobal(geometry()->toGlobal( zero ));

  SmartDataPtr<DetectorElement> deRich1(dataSvc(),DeRichLocations::Rich1 );
  if ( !deRich1 ) {
    msg << MSG::ERROR << "Could not load DeRich1" << endmsg;
    return StatusCode::FAILURE;
  }

  // Work out what Rich/panel I am
  if ( name().find("Rich1") != std::string::npos )
  {
    m_rich = Rich::Rich1;
    if ( centreGlobal.y() > 0.0 )
      m_side = Rich::top;
    else
      m_side = Rich::bottom;
  }
  else if ( name().find("Rich2") != std::string::npos )
  {
    m_rich = Rich::Rich2;
    if ( centreGlobal.x() > 0.0 )
      m_side = Rich::left;
    else
      m_side = Rich::right;
  }
  if ( m_rich == Rich::InvalidDetector ||
       m_side == Rich::InvalidSide )
  {
    msg << MSG::ERROR << "Error initializing HPD panel " << name() << endreq;
    return StatusCode::FAILURE;
  }
  bool rich1 = (m_rich == Rich::Rich1);

  msg << MSG::DEBUG << "------- Initializing HPD Panel: " << rich()
      << " Panel" << (int)side() << " -------" << endmsg;

  // prepare a smartID for this panel
  m_panelRichID.setRich( rich() );
  m_panelRichID.setPanel( side() );

  // find the RichSystem
  SmartDataPtr<DeRichSystem> deRichS(dataSvc(),DeRichLocations::RichSystem );
  if ( !deRichS ) {
    msg << MSG::ERROR << "Could not load DeRich System" << endmsg;
    return StatusCode::FAILURE;
  }
  m_deRichS = deRichS;

  m_pixelSize      = deRich1->param<double>("RichHpdPixelXsize");
  m_subPixelSize   = m_pixelSize/8.0;
  const double activeRadius = deRich1->param<double>("RichHpdActiveInpRad");
  m_activeRadiusSq = activeRadius*activeRadius;

  m_pixelColumns = deRich1->param<int>("RichHpdNumPixelCol");
  m_pixelRows    = deRich1->param<int>("RichHpdNumPixelRow");

  msg << MSG::DEBUG << "RichHpdPixelsize: " << m_pixelSize << " ActiveRadius: "
      << activeRadius << " pixelRows: " << m_pixelRows << " pixelColumns: "
      << m_pixelColumns << endreq;

  m_HPDColumns = param<int>("HPDColumns");
  m_HPDNumInCol = param<int>("HPDNumberInColumn");
  m_HPDMax = nHPDColumns() * nHPDsPerCol();

  m_HPDPitch = param<double>("HPDPitch");
  m_HPDColPitch = sqrt( 0.75 * m_HPDPitch*m_HPDPitch );
  msg << MSG::DEBUG << "HPDColumns:" << nHPDColumns() << " HPDNumberInColumns:"
      << nHPDsPerCol() << endmsg;

  if ( m_HPDColPitch  < activeRadius*2) {
    msg << MSG::WARNING << "The active area is bigger by:"
        << (activeRadius*2 - fabs(m_HPDColPitch))/Gaudi::Units::mm
        << " mm than the column pitch.  There could be loss of photons"
        << endmsg;
  }

  // read the position of the 1st HPD in columns 0 and 1
  std::vector<double> startColPos = param<std::vector<double> >("StartColumnPosition");
  // work in u,v coordinates: u is across a column, v is along
  double HPD00u(0.0), HPD00v(0.0), HPD10u(0.0), HPD10v(0.0);
  if ( rich1 ) {
    HPD00u = startColPos[1];
    HPD00v = startColPos[0];
    HPD10u = startColPos[3];
    HPD10v = startColPos[2];
  }
  else {
    HPD00u = startColPos[0];
    HPD00v = startColPos[1];
    HPD10u = startColPos[2];
    HPD10v = startColPos[3];
  }

  if ( HPD00v > 0.0 ) m_HPDPitch = -m_HPDPitch;
  if ( HPD00u > 0.0 ) m_HPDColPitch = -m_HPDColPitch;
  m_panelColumnSideEdge = HPD00u - 0.5*m_HPDColPitch;
  m_localOffset = fabs( m_panelColumnSideEdge );

  msg << MSG::DEBUG << "HPDPitch:" << m_HPDPitch << " panelColumnSideEdge:"
      << m_panelColumnSideEdge << endmsg;

  m_panelStartColPosEven = HPD00v - 0.5*m_HPDPitch;
  m_panelStartColPosOdd = HPD10v - 0.5*m_HPDPitch;

  // use the abs(largest) value as an ovearll panel edge
  m_panelStartColPos = fabs( m_panelStartColPosEven );
  if ( fabs( m_panelStartColPosOdd ) > m_panelStartColPos )
    m_panelStartColPos = fabs( m_panelStartColPosOdd );

  msg << MSG::DEBUG << "panelStartColPosEven:" << m_panelStartColPosEven
      << " panelStartColPosOdd:" << m_panelStartColPosOdd
      << " m_panelStartColPos:" << m_panelStartColPos
      << endreq;

  // get the first HPD and follow down to the silicon block
  const IPVolume* pvHPDMaster0 = geometry()->lvolume()->pvolume(0);
  const IPVolume* pvHPDSMaster0 = pvHPDMaster0->lvolume()->pvolume(0);
  const IPVolume* pvSilicon0 = pvHPDSMaster0->lvolume()->pvolume("pvRichHPDSiDet");

  const ISolid* siliconSolid = pvSilicon0->lvolume()->solid();
  msg << MSG::VERBOSE << "About to do a dynamic cast SolidBox" << endreq;
  const SolidBox* siliconBox = dynamic_cast<const SolidBox*>(siliconSolid);

  // assume same size for all silicon detectors
  m_siliconHalfLengthX = siliconBox->xHalfLength();
  m_siliconHalfLengthY = siliconBox->yHalfLength();

  // get the pv and the solid for the HPD quartz window
  const IPVolume* pvWindow0 = pvHPDSMaster0->lvolume()->
    pvolume("pvRichHPDQuartzWindow");
  const ISolid* windowSolid0 = pvWindow0->lvolume()->solid();
  // get the inside radius of the window
  ISolid::Ticks windowTicks;
  unsigned int windowTicksSize = windowSolid0->
    intersectionTicks(Gaudi::XYZPoint(0.0, 0.0, 0.0),Gaudi::XYZVector(0.0, 0.0, 1.0),
                      windowTicks);
  if (windowTicksSize != 2) {
    msg << MSG::FATAL << "Problem getting window radius" << endreq;
    return StatusCode::FAILURE;
  }
  const double winR = windowTicks[0];

  // get the HPD and SiSensor detector elements
  IDetectorElement::IDEContainer detelems =  childIDetectorElements();
  IDetectorElement::IDEContainer::iterator det_it;
  for (det_it = detelems.begin(); det_it != detelems.end(); ++det_it) {
    if ( std::string::npos != (*det_it)->name().find("HPD:") ) {
      SmartDataPtr<DeRichHPD> deHPD(dataSvc(), (*det_it)->name() );
      if ( !deHPD ) {
        msg << MSG::FATAL << "Non DeRichHPD detector element "
            << (*det_it)->name() << endmsg;
        return StatusCode::FAILURE;
      }
      m_DeHPDs.push_back( deHPD );
      m_DeSiSensors.push_back( deHPD->childIDetectorElements().front() );
    }
  }

  msg << MSG::DEBUG << "Centre of HPDPanel : " << geometry()->toGlobal(zero)
      << endmsg;
  msg << MSG::VERBOSE << "Ideal local centre of HPD#0 "
      << geometry()->toLocal(DeHPD(0)->windowCentreInIdeal()) << endmsg;
  msg << MSG::VERBOSE << "Ideal local centre of HPD#" << nHPDsPerCol()-1 << " "
      << geometry()->toLocal(DeHPD(nHPDsPerCol()-1)->windowCentreInIdeal()) << endmsg;
  msg << MSG::VERBOSE << "Ideal local centre of HPD#" << nHPDsPerCol() << " "
      << geometry()->toLocal(DeHPD(nHPDsPerCol())->windowCentreInIdeal()) << endmsg;
  msg << MSG::VERBOSE << "Ideal local centre of HPD#" << 2*nHPDsPerCol()-1 << " "
      << geometry()->toLocal(DeHPD(2*nHPDsPerCol()-1)->windowCentreInIdeal()) << endmsg;

  // find the top of 3 HPDs to create a detection plane.
  Gaudi::XYZPoint pointA( DeHPD(0)->windowCentreInIdeal() );
  // for second point go to HPD at the end of the column.
  Gaudi::XYZPoint pointB( DeHPD(nHPDsPerCol()-1)->windowCentreInIdeal() );
  // now point C at the other end.
  Gaudi::XYZPoint pointC( DeHPD(nHPDs()-nHPDsPerCol()/2)->windowCentreInIdeal() );

  m_detectionPlane = Gaudi::Plane3D(pointA, pointB, pointC);
  msg << MSG::VERBOSE << "Detection plane:" << m_detectionPlane << endmsg;

  m_localPlane = geometry()->toLocalMatrix() * m_detectionPlane;
  msg << MSG::VERBOSE << "Detection plane local:" << m_localPlane << endmsg;
  m_localPlaneNormal = m_localPlane.Normal();

  // store the z coordinate of the detection plane
  m_detPlaneZ = geometry()->toLocal(pointA).z();
  msg << MSG::VERBOSE << "Local z coord of det plane" << m_detPlaneZ << endmsg;

  // localPlane2 is used when trying to locate the HPD row/column from
  // a point in the panel.
  const double detPlaneZdiff = winR - sqrt( winR*winR - m_activeRadiusSq );
  m_localPlane2 = Gaudi::Transform3D(Gaudi::XYZVector(0.0,0.0,detPlaneZdiff))(m_localPlane);
  m_localPlaneNormal2 = m_localPlane2.Normal();

  // Cache information for PDWindowPoint method
  m_kaptonSolid =
    pvHPDSMaster0->lvolume()->pvolume("pvRichHPDKaptonShield")->lvolume()->solid();

  msg << MSG::DEBUG << "Found " << m_DeHPDs.size() << " DeRichHPDs" << endmsg;

  msg << MSG::DEBUG << "Initialisation Complete" << endreq;
  return StatusCode::SUCCESS;
}

//=========================================================================
// convert a point on the silicon sensor to smartID
//=========================================================================
StatusCode DeRichHPDPanel::smartID ( const Gaudi::XYZPoint& globalPoint,
                                     LHCb::RichSmartID& id ) const
{
  const Gaudi::XYZPoint inPanel( geometry()->toLocal(globalPoint) );

  // find the HPD row/col of this point if not set
  if ( !id.hpdColIsSet() || !id.hpdNumInColIsSet() ) {
    if ( !findHPDColAndPos(inPanel, id) ) return StatusCode::FAILURE;
  }

  // check if the HPD is active or dead
  if ( !m_deRichS->hpdIsActive( id ) ) return StatusCode::FAILURE;

  const unsigned int HPDNumber = hpdNumber(id);

  const Gaudi::XYZPoint inSilicon( m_DeSiSensors[HPDNumber]->geometry()->toLocalMatrix()*globalPoint );

  double inSiliconX = inSilicon.x();
  double inSiliconY = inSilicon.y();

  // for points too close to the silicon edge subtarct 1/1000 of a mm
  if ( (fabs(inSiliconX)+0.001*Gaudi::Units::mm) > m_siliconHalfLengthX ) {
    const int signX = ( inSiliconX > 0.0 ? 1 : -1 );
    inSiliconX -= signX*0.001*Gaudi::Units::mm;
  }

  if ( (fabs(inSiliconY)+0.001*Gaudi::Units::mm) > m_siliconHalfLengthY ) {
    const int signY = ( inSiliconY > 0.0 ? 1 : -1 );
    inSiliconY -= signY*0.001*Gaudi::Units::mm;
  }

  // if point still outside silicon flag an error
  if ( (fabs(inSiliconX) - m_siliconHalfLengthX > 1E-3*Gaudi::Units::mm) ||
       (fabs(inSiliconY) - m_siliconHalfLengthY > 1E-3*Gaudi::Units::mm)   ) {
    MsgStream msg ( msgSvc(), myName() );
    msg << MSG::ERROR << "Point " << inSilicon << " is outside the silicon box "
        << DeHPD(HPDNumber)->name() << endmsg;
    return StatusCode::FAILURE;
  }

  // pixel 0,0 is at min x and max y (top left corner)
  const unsigned int pixelColumn = static_cast<unsigned int>
    ((m_siliconHalfLengthX + inSiliconX) / m_pixelSize);
  const unsigned int pixelRow    = static_cast<unsigned int>
    ((m_siliconHalfLengthY - inSiliconY) / m_pixelSize);

  id.setPixelRow(pixelRow);
  id.setPixelCol(pixelColumn);

  // find subpixel (Alice mode)
  const unsigned int subPixel = static_cast<unsigned int>
    ((m_siliconHalfLengthY-inSiliconY-pixelRow*m_pixelSize) /
     m_subPixelSize);
  id.setPixelSubRow( subPixel );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  find an intersection with the inside of the HPD window
//=========================================================================
LHCb::RichTraceMode::RayTraceResult
DeRichHPDPanel::PDWindowPoint( const Gaudi::XYZVector& vGlobal,
                               const Gaudi::XYZPoint& pGlobal,
                               Gaudi::XYZPoint& windowPointGlobal,
                               LHCb::RichSmartID& smartID,
                               const LHCb::RichTraceMode mode ) const
{

  // transform vector to the HPDPanel coordsystem.
  const Gaudi::XYZVector vInPanel( geometry()->toLocalMatrix()*vGlobal );

  // find the intersection with the detection plane
  const double scalar = vInPanel.Dot(m_localPlaneNormal2);
  if ( fabs(scalar) < 1e-50 ) return LHCb::RichTraceMode::RayTraceFailed;

  // transform point to the HPDPanel coordsystem.
  const Gaudi::XYZPoint pInPanel( geometry()->toLocal(pGlobal) );

  // get panel intersection point
  const double distance = -m_localPlane2.Distance( pInPanel )/scalar;
  const Gaudi::XYZPoint panelIntersection( pInPanel + distance*vInPanel );

  // Get HPD column and row numbers
  if ( !findHPDColAndPos(panelIntersection, smartID) ) return LHCb::RichTraceMode::RayTraceFailed;

  // HPD number
  const unsigned int HPDNumber = hpdNumber(smartID);
  const DeRichHPD* deHPD = DeHPD( HPDNumber );

  if ( mode.detPrecision() == LHCb::RichTraceMode::SimpleHPDs )
  {
    // do it quickly

    const Gaudi::XYZPoint& centre( deHPD->hpdWinInsideCentreInMother());
    const double x = panelIntersection.x() - centre.x();
    const double y = panelIntersection.y() - centre.y();

    if ( ( x*x + y*y ) > m_activeRadiusSq )
    {
      // not in an HPD, but are we in the HPD panel ?
      const LHCb::RichTraceMode::RayTraceResult res = checkPanelAcc(panelIntersection);
      if ( res == LHCb::RichTraceMode::InHPDPanel )
      {
        // set the window point to panel intersection point
        windowPointGlobal = geometry()->toGlobal( panelIntersection );
        return res;
      }
    }

    // check if the HPD is active or dead
    if ( !m_deRichS->hpdIsActive( smartID ) ) return LHCb::RichTraceMode::OutsideHPDPanel;

    // Check for shadowing effects by HPD kapton shields
    if ( mode.hpdKaptonShadowing() )
    {
      const Gaudi::XYZPoint  pInKapton( deHPD->fromPanelToKapton() * pInPanel );
      const Gaudi::XYZVector vInKapton( deHPD->fromPanelToKapton() * vInPanel );

      ISolid::Ticks kaptonTicks;
      if ( 0 != m_kaptonSolid->intersectionTicks(pInKapton, vInKapton, kaptonTicks) )
        return LHCb::RichTraceMode::OutsideHPDPanel;
    }

    // set the window point to panel intersection point
    windowPointGlobal = geometry()->toGlobal( panelIntersection );

  }
  else
  {
    // Overwise slower but fully detailed checks

    // Check for shadowing effects by HPD kapton shields
    if ( mode.hpdKaptonShadowing() )
    {
      const Gaudi::XYZPoint  pInKapton( deHPD->fromPanelToKapton() * pInPanel );
      const Gaudi::XYZVector vInKapton( deHPD->fromPanelToKapton() * vInPanel );

      ISolid::Ticks kaptonTicks;
      if ( 0 != m_kaptonSolid->intersectionTicks(pInKapton, vInKapton, kaptonTicks) )
        return LHCb::RichTraceMode::OutsideHPDPanel;
    }

    // convert point and vector to window  coordinate systems
    const Gaudi::XYZPoint pInWindow( deHPD->fromPanelToHPDWindow(pInPanel) );
    const Gaudi::XYZVector vInWindow( deHPD->fromPanelToHPDWindow(vInPanel) );

    ISolid::Ticks HPDWindowTicks;
    const unsigned int noTicks = deHPD->HPDWindowSolid()->
      intersectionTicks( pInWindow, vInWindow, HPDWindowTicks );
    if ( 0 == noTicks ) return LHCb::RichTraceMode::OutsideHPDPanel;

    const Gaudi::XYZPoint windowPoint( pInWindow + HPDWindowTicks[1]*vInWindow );
    const Gaudi::XYZPoint windowPointInHPD( deHPD->fromHPDWindowToHPD()*windowPoint );
    // check the active radius.
    const double hitRadiusSq = ( windowPointInHPD.x()*windowPointInHPD.x() +
                                 windowPointInHPD.y()*windowPointInHPD.y() );
    if ( hitRadiusSq > m_activeRadiusSq )
    {
      // not in an HPD, but are we in the HPD panel ?
      return checkPanelAcc(windowPointInHPD);
    }

    // check if the HPD is active or dead
    if ( !m_deRichS->hpdIsActive( smartID ) ) return LHCb::RichTraceMode::OutsideHPDPanel;

    // finally, set the window point
    windowPointGlobal =
      geometry()->toGlobal( deHPD->fromHPDToPanel(windowPointInHPD) );
  }

  return LHCb::RichTraceMode::InHPDTube;
}

//=========================================================================
//  return a list with all the valid readout channels (smartIDs)
//=========================================================================
StatusCode
DeRichHPDPanel::readoutChannelList ( LHCb::RichSmartID::Vector& readoutChannels ) const
{

  // Square of active radius
  const double activeRadiusSq = m_siliconHalfLengthX*m_siliconHalfLengthX;

  for ( unsigned int PD = 0; PD < nHPDs(); ++PD )
  {
    // Get HPD row and column numbers outside loops.
    const unsigned int pdCol = PD/nHPDsPerCol();
    const unsigned int pdPosInCol = PD%nHPDsPerCol();

    // Loop over pixels
    for ( unsigned int pixRow = 0; pixRow < m_pixelRows; ++pixRow )
    {
      for ( unsigned int pixCol = 0; pixCol < m_pixelColumns; ++pixCol )
      {

        const double xpix  = (pixRow + 0.5)*m_pixelSize - m_siliconHalfLengthX;
        const double ypix  = (pixCol + 0.5)*m_pixelSize - m_siliconHalfLengthY;
        const double xcorn = ( xpix < 0.0 ? xpix+0.5*m_pixelSize : xpix-0.5*m_pixelSize );
        const double ycorn = ( ypix < 0.0 ? ypix+0.5*m_pixelSize : ypix-0.5*m_pixelSize );
        const double radcornSq = xcorn*xcorn + ycorn*ycorn;

        if ( radcornSq <= activeRadiusSq )
        {
          // Add a smart ID for this pixel to the vector
          readoutChannels.push_back ( LHCb::RichSmartID(rich(),side(),pdPosInCol,pdCol,pixRow,pixCol) );
        }

      } // loop over pixel columns
    } // loop over pixel rows

  } // loop over HPDs

  return StatusCode::SUCCESS;
}

//=========================================================================
//  returns the intersection point with the detection plane
//=========================================================================
LHCb::RichTraceMode::RayTraceResult
DeRichHPDPanel::detPlanePoint( const Gaudi::XYZPoint& pGlobal,
                               const Gaudi::XYZVector& vGlobal,
                               Gaudi::XYZPoint& hitPosition,
                               LHCb::RichSmartID& smartID,
                               const LHCb::RichTraceMode mode ) const
{
  LHCb::RichTraceMode::RayTraceResult result = LHCb::RichTraceMode::RayTraceFailed;

  // transform to the Panel coord system.
  const Gaudi::XYZVector vInPanel( geometry()->toLocalMatrix()*vGlobal );

  // find the intersection with the detection plane
  const double scalar = vInPanel.Dot(m_localPlaneNormal);
  if ( fabs(scalar) < 1e-5 ) return result;

  // transform point to the HPDPanel coordsystem.
  const Gaudi::XYZPoint pInPanel( geometry()->toLocal(pGlobal) );

  // get panel intersection point
  const double distance = -m_localPlane.Distance(pInPanel) / scalar;
  const Gaudi::XYZPoint panelIntersection( pInPanel + distance*vInPanel );

  // set the (closest) HPD number
  findHPDColAndPos(panelIntersection,smartID);

  result = LHCb::RichTraceMode::InHPDPanel;
  if ( mode.detPlaneBound() == LHCb::RichTraceMode::RespectHPDPanel )
  {
    const double u = ( m_rich == Rich::Rich1 ? panelIntersection.y() : panelIntersection.x() );
    const double v = ( m_rich == Rich::Rich1 ? panelIntersection.x() : panelIntersection.y() );
    if ( fabs(u) >= fabs(m_panelColumnSideEdge) ||
         fabs(v) >= m_panelStartColPos ) { result = LHCb::RichTraceMode::OutsideHPDPanel; }
  }

  // set final position
  hitPosition = geometry()->toGlobal( panelIntersection );

  // return status
  return result;
}


//=========================================================================
// findHPDColAndPos
//=========================================================================
bool DeRichHPDPanel::findHPDColAndPos ( const Gaudi::XYZPoint& inPanel,
                                        LHCb::RichSmartID& id ) const
{
  bool OK = true;

  const double u = ( m_rich == Rich::Rich1 ? inPanel.y() : inPanel.x() );
  const double v = ( m_rich == Rich::Rich1 ? inPanel.x() : inPanel.y() );

  // work out nearest column
  int HPDCol = static_cast<int>(floor(u-m_panelColumnSideEdge)/m_HPDColPitch);
  if      ( HPDCol >= (int)nHPDColumns() ) { OK = false; HPDCol = nHPDColumns()-1; }
  else if ( HPDCol < 0                   ) { OK = false; HPDCol = 0;              }
  id.setHPDCol( HPDCol );

  // nearest number in column
  int HPDNumInCol
    = ( 0 == HPDCol%2 ?
        static_cast<int>(floor((v-m_panelStartColPosEven)/m_HPDPitch)) :
        static_cast<int>(floor((v-m_panelStartColPosOdd)/m_HPDPitch)) );
  if      ( HPDNumInCol >= (int)nHPDsPerCol() ) { OK = false; HPDNumInCol = nHPDsPerCol()-1; }
  else if ( HPDNumInCol < 0                   ) { OK = false; HPDNumInCol = 0;               }
  id.setHPDNumInCol(HPDNumInCol);

  return OK;
}
/*
  {
  const double u = ( m_rich == Rich::Rich1 ? inPanel.y() : inPanel.x() );
  const double v = ( m_rich == Rich::Rich1 ? inPanel.x() : inPanel.y() );

  const unsigned int HPDCol =
  static_cast<unsigned int>(floor(u-m_panelColumnSideEdge)/m_HPDColPitch);
  if (HPDCol >= nHPDColumns()) return false;
  id.setHPDCol( HPDCol );

  unsigned int HPDNumInCol
  = ( 0 == HPDCol%2 ?
  static_cast<unsigned int>(floor((v-m_panelStartColPosEven)/m_HPDPitch)) :
  static_cast<unsigned int>(floor((v-m_panelStartColPosOdd)/m_HPDPitch)) );

  if (HPDNumInCol >= nHPDsPerCol()) return false;
  id.setHPDNumInCol(HPDNumInCol);

  return true;
  }
*/

//=========================================================================
//  convert a point from the panel to the global coodinate system
//=========================================================================
Gaudi::XYZPoint
DeRichHPDPanel::globalPosition( const Gaudi::XYZPoint& localPoint,
                                const Rich::Side side) const
{
  const double z = localPoint.z() + m_detPlaneZ;
  double x( 0.0 );
  double y( 0.0 );

  if ( m_rich == Rich::Rich1 ) {
    const int sign = ( side == Rich::top ? -1 : 1 );
    x = localPoint.x();
    y = localPoint.y()+sign*m_panelColumnSideEdge;
  }
  else {
    const int sign = ( side == Rich::left ? -1 : 1 );
    x = localPoint.x()+sign*m_panelColumnSideEdge;
    y = localPoint.y();
  }

  return ( geometry()->toGlobal(Gaudi::XYZPoint(x, y, z) ) );
}

//=========================================================================
//  sensitiveVolumeID
//=========================================================================
const int DeRichHPDPanel::sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const
{
  LHCb::RichSmartID id( m_panelRichID );
  return ( smartID(globalPoint,id).isSuccess() ? id : LHCb::RichSmartID() );
}

