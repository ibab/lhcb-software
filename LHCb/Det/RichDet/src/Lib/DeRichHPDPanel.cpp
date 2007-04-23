//----------------------------------------------------------------------------
/** @file DeRichHPDPanel.cpp
 *
 *  Implementation file for detector description class : DeRichHPDPanel
 *
 *  $Id: DeRichHPDPanel.cpp,v 1.57 2007-04-23 12:28:13 jonrob Exp $
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
  m_rich       ( Rich::InvalidDetector ),
  m_side       ( Rich::InvalidSide     ),
  m_panelRichID( LHCb::RichSmartID()   )
{}

// Standard Destructor
DeRichHPDPanel::~DeRichHPDPanel() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichHPDPanel::classID() {
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
  if      ( name().find("Rich1") != std::string::npos )
  {
    m_rich = Rich::Rich1;
    if ( centreGlobal.y() > 0.0 ) {
      m_side = Rich::top;
    }
    else {
      m_side = Rich::bottom;
    }
  }
  else if ( name().find("Rich2") != std::string::npos )
  {
    m_rich = Rich::Rich2;
    if ( centreGlobal.x() > 0.0 ) {
      m_side = Rich::left;
    }
    else {
      m_side = Rich::right;
    }
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
  m_subPixelSize   = m_pixelSize/8;
  const double activeRadius = deRich1->param<double>("RichHpdActiveInpRad");
  m_activeRadiusSq = activeRadius*activeRadius;

  m_pixelColumns = deRich1->param<int>("RichHpdNumPixelCol");
  m_pixelRows    = deRich1->param<int>("RichHpdNumPixelRow");

  msg << MSG::DEBUG << "RichHpdPixelsize: " << m_pixelSize << " ActiveRadius: "
      << activeRadius << " pixelRows: " << m_pixelRows << " pixelColumns: "
      << m_pixelColumns << endreq;

  m_HPDColumns = param<int>("HPDColumns");
  m_HPDNumInCol = param<int>("HPDNumberInColumn");
  m_HPDMax = m_HPDColumns * m_HPDNumInCol;

  m_HPDPitch = param<double>("HPDPitch");
  m_HPDColPitch = sqrt( 0.75 * m_HPDPitch*m_HPDPitch );
  msg << MSG::DEBUG << "HPDColumns:" << m_HPDColumns << " HPDNumberInColumns:"
      << m_HPDNumInCol << endmsg;

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

  msg << MSG::DEBUG << "Centre of HPDPanel : " << geometry()->toGlobal(zero)
      << endmsg;
  msg << MSG::VERBOSE << "Centre of HPD#0 " << geometry()->lvolume()->
    pvolume(0)->toMother( zero ) << endmsg;
  msg << MSG::VERBOSE << "Centre of HPD#" << m_HPDNumInCol-1 << " " << geometry()->lvolume()->
    pvolume(m_HPDNumInCol-1)->toMother( zero ) << endmsg;
  msg << MSG::VERBOSE << "Centre of HPD#" << m_HPDNumInCol << " " << geometry()->lvolume()->
    pvolume(m_HPDNumInCol)->toMother( zero ) << endmsg;
  msg << MSG::VERBOSE << "Centre of HPD#" << 2*m_HPDNumInCol-1 << " " << geometry()->lvolume()->
    pvolume(2*m_HPDNumInCol-1)->toMother( zero ) << endmsg;

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

  // get the coordinate of the centre of the HPD quarz window
  Gaudi::XYZPoint HPDTop1(0.0, 0.0, winR);
  // convert this to HPDS master  coordinates
  Gaudi::XYZPoint HPDTop2 = pvWindow0->toMother(HPDTop1);

  // find the top of 3 HPDs to create a detection plane.  We already have the
  // first in HPDSMaster coordinates.
  // now to HPD coordinates
  Gaudi::XYZPoint pointAInHPD = pvHPDSMaster0->toMother(HPDTop2);
  Gaudi::XYZPoint pointAInPanel = pvHPDMaster0->toMother(pointAInHPD);
  Gaudi::XYZPoint pointA = geometry()->toGlobal(pointAInPanel);

  // for second point go to HPD at the end of the column.
  //The relative position inside the HPD is the same
  const IPVolume* pvHPDMasterB = geometry()->lvolume()->pvolume(m_HPDNumInCol-1);
  Gaudi::XYZPoint pointBInPanel = pvHPDMasterB->toMother(pointAInHPD);
  Gaudi::XYZPoint pointB = geometry()->toGlobal(pointBInPanel);

  // now point C at the other end.
  int numberForC = m_HPDMax - m_HPDNumInCol/2;
  const IPVolume* pvHPDMasterC = geometry()->lvolume()->pvolume(numberForC);
  Gaudi::XYZPoint pointCInPanel = pvHPDMasterC->toMother(pointAInHPD);
  Gaudi::XYZPoint pointC = geometry()->toGlobal(pointCInPanel);

  m_detectionPlane = Gaudi::Plane3D(pointA, pointB, pointC);
  m_localPlane = Gaudi::Plane3D(pointAInPanel, pointBInPanel, pointCInPanel);
  m_localPlaneNormal = m_localPlane.Normal();

  // store the z coordinate of the detection plane
  m_detPlaneZ = pointAInPanel.z();

  // localPlane2 is used when trying to locate the HPD row/column from
  // a point in the panel.
  const double detPlaneZdiff = winR - sqrt( winR*winR - m_activeRadiusSq );
  m_localPlane2 = Gaudi::Transform3D(Gaudi::XYZVector(0.0,0.0,detPlaneZdiff))(m_localPlane);
  m_localPlaneNormal2 = m_localPlane2.Normal();

  // Cache information for PDWindowPoint method
  m_vectorTransf = geometry()->matrix();
  m_kaptonSolid = 
    pvHPDSMaster0->lvolume()->pvolume("pvRichHPDKaptonShield")->lvolume()->solid();

  // Cache HPD information
  m_pvHPDMaster.clear();
  m_pvHPDSMaster.clear();
  m_pvSilicon.clear();
  m_pvKapton.clear();
  m_pvWindow.clear();
  m_HPDCentres.clear();
  for ( unsigned int HPD = 0; HPD < m_HPDMax; ++HPD ) {
    const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPD);
    if ( !pvHPDMaster )
    {
      msg << MSG::ERROR << "Failed to access HPDMaster" << endreq;
      return  StatusCode::FAILURE;
    }
    const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
    if ( !pvHPDSMaster )
    {
      msg << MSG::ERROR << "Failed to access HPDSMaster" << endreq;
      return  StatusCode::FAILURE;
    }
    const IPVolume* pvWindow = pvHPDSMaster->lvolume()->pvolume("pvRichHPDQuartzWindow");
    if ( !pvWindow )
    {
      msg << MSG::ERROR << "Failed to access HPDWindow" << endreq;
      return  StatusCode::FAILURE;
    }
    const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume("pvRichHPDSiDet");
    if ( !pvSilicon )
    {
      msg << MSG::ERROR << "Failed to access HPDSilicon" << endreq;
      return  StatusCode::FAILURE;
    }
    const IPVolume* pvKapton = pvHPDSMaster->lvolume()->pvolume("pvRichHPDKaptonShield");
    if ( !pvKapton )
    {
      msg << MSG::ERROR << "Failed to access Kapton shield" << endreq;
      return  StatusCode::FAILURE;
    }
    m_pvHPDMaster.push_back( pvHPDMaster );
    m_pvHPDSMaster.push_back( pvHPDSMaster );
    m_pvSilicon.push_back( pvSilicon );
    m_pvWindow.push_back( pvWindow );
    m_pvKapton.push_back( pvKapton );
    m_HPDCentres.push_back( pvHPDMaster->toMother(zero) );
    m_HPDWindowToGlobal.push_back( geometry()->matrixInv() * pvHPDMaster->matrixInv() *
                                   pvHPDSMaster->matrixInv() * pvWindow->matrixInv() );
    m_panelToSilicon.push_back( pvSilicon->matrix() * pvHPDSMaster->matrix() *
                                pvHPDMaster->matrix() );
  }

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
    }
  }
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

  const Gaudi::XYZPoint inPanel = geometry()->toLocal(globalPoint);

  // find the HPD row/col of this point if not set
  if ( !id.hpdColIsSet() || !id.hpdNumInColIsSet() ) {
    if ( !findHPDColAndPos(inPanel, id) ) return StatusCode::FAILURE;
  }

  // check if the HPD is active or dead
  if ( !m_deRichS->hpdIsActive( id ) ) return StatusCode::FAILURE;

  const unsigned int HPDNumber = hpdNumber(id);

  const Gaudi::XYZPoint inSilicon = m_panelToSilicon[HPDNumber] * inPanel;

  double inSiliconX = inSilicon.x();
  double inSiliconY = inSilicon.y();

  if ( (fabs(inSiliconX)+0.001*Gaudi::Units::mm) > m_siliconHalfLengthX ) {
    const int signX = ( inSiliconX > 0.0 ? 1 : -1 );
    inSiliconX -= signX*0.001*Gaudi::Units::mm;
  }

  if ( (fabs(inSiliconY)+0.001*Gaudi::Units::mm) > m_siliconHalfLengthY ) {
    const int signY = ( inSiliconY > 0.0 ? 1 : -1 );
    inSiliconY -= signY*0.001*Gaudi::Units::mm;
  }

  if ( (fabs(inSiliconX) - m_siliconHalfLengthX > 1E-3*Gaudi::Units::mm) ||
       (fabs(inSiliconY) - m_siliconHalfLengthY > 1E-3*Gaudi::Units::mm)   ) {
    MsgStream msg ( msgSvc(), myName() );
    msg << MSG::ERROR << "Point " << inSilicon << " is outside the silicon box "
        << HPDMaster(HPDNumber)->name() << endmsg;
    return StatusCode::FAILURE;
  }

  const unsigned int pixelColumn = static_cast<unsigned int>
    ((m_siliconHalfLengthX + inSiliconX) / m_pixelSize);
  const unsigned int pixelRow    = static_cast<unsigned int>
    ((m_siliconHalfLengthY - inSiliconY) / m_pixelSize);

  id.setPixelRow(pixelRow);
  id.setPixelCol(pixelColumn);

  const unsigned int subPixel = static_cast<unsigned int>
    ((m_siliconHalfLengthY-inSiliconY-pixelRow*m_pixelSize) /
     m_subPixelSize);
  id.setPixelSubRow( subPixel );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  find an intersection with the inside of the HPD window
//=========================================================================
StatusCode DeRichHPDPanel::PDWindowPoint( const Gaudi::XYZVector& vGlobal,
                                          const Gaudi::XYZPoint& pGlobal,
                                          Gaudi::XYZPoint& windowPointGlobal,
                                          LHCb::RichSmartID& smartID,
                                          const LHCb::RichTraceMode mode ) const
{

  // transform point and vector to the HPDPanel coordsystem.
  const Gaudi::XYZPoint  pInPanel( geometry()->toLocal(pGlobal) );
  const Gaudi::XYZVector vInPanel( m_vectorTransf*vGlobal );

  // find the intersection with the detection plane (localPlane2)
  const double scalar = vInPanel.Dot(m_localPlaneNormal2);
  if ( fabs(scalar) < 1e-50 ) return StatusCode::FAILURE;

  const double distance = -m_localPlane2.Distance( pInPanel )/scalar;
  const Gaudi::XYZPoint panelIntersection( pInPanel + distance*vInPanel );

  // Get HPD column and row numbers
  if ( !findHPDColAndPos(panelIntersection, smartID) ) return StatusCode::FAILURE;

  // HPD number
  const unsigned int HPDNumber = hpdNumber(smartID);

  if ( mode.detPrecision() == LHCb::RichTraceMode::SimpleHPDs )
  {
    // do it quickly

    const Gaudi::XYZPoint & centre = m_HPDCentres[HPDNumber];
    const double x = panelIntersection.x() - centre.x();
    const double y = panelIntersection.y() - centre.y();
    if ( ( x*x + y*y ) > m_activeRadiusSq ) return StatusCode::FAILURE;

    // check if the HPD is active or dead
    if ( !m_deRichS->hpdIsActive( smartID ) ) return StatusCode::FAILURE;

    // Check for shadowing effects by HPD kapton shields
    if ( mode.hpdKaptonShadowing() )
    {
      const IPVolume* pvHPDMaster = HPDMaster(HPDNumber);
      const Gaudi::XYZPoint  pInHPD( pvHPDMaster->toLocal(pInPanel) );
      const Gaudi::XYZVector vInHPD( pvHPDMaster->matrix()*vInPanel );
      ISolid::Ticks kaptonTicks;
      if ( 0 != m_kaptonSolid->intersectionTicks(pInHPD, vInHPD, kaptonTicks) )
        return StatusCode::FAILURE;
    }

    // finally, set the window point
    windowPointGlobal = geometry()->toGlobal( panelIntersection );

  }
  else
  {
    // Overwise slower but fully detailed checks

    // find the correct HPD and quartz window inside it
    const IPVolume* pvHPDMaster = HPDMaster(HPDNumber);

    // Check for shadowing effects by HPD kapton shields
    if ( mode.hpdKaptonShadowing() )
    {
      const Gaudi::XYZPoint  pInHPD( pvHPDMaster->toLocal(pInPanel) );
      const Gaudi::XYZVector vInHPD( pvHPDMaster->matrix()*vInPanel );
      ISolid::Ticks kaptonTicks;
      if ( 0 != m_kaptonSolid->intersectionTicks(pInHPD, vInHPD, kaptonTicks) )
        return StatusCode::FAILURE;
    }

    const IPVolume* pvHPDSMaster = m_pvHPDSMaster[HPDNumber];
    const IPVolume* pvWindow     = m_pvWindow[HPDNumber];
    const ISolid* windowSolid    = pvWindow->lvolume()->solid();

    // convert point to local coordinate systems
    const Gaudi::XYZPoint pInWindow( pvWindow->toLocal(pvHPDSMaster->
                                                       toLocal(pvHPDMaster->toLocal(pInPanel))) );

    // convert local vector assuming that only the HPD can be rotated
    const Gaudi::XYZVector vInHPDMaster( pvHPDMaster->matrix()*vInPanel );

    ISolid::Ticks HPDWindowTicks;
    const unsigned int noTicks = windowSolid->intersectionTicks( pInWindow,
                                                                 vInHPDMaster,
                                                                 HPDWindowTicks );
    if ( 0 == noTicks ) return StatusCode::FAILURE;

    const Gaudi::XYZPoint windowPoint( pInWindow + HPDWindowTicks[1]*vInHPDMaster );
    const Gaudi::XYZPoint windowPointInHPD( pvHPDSMaster->
                                            toMother(pvWindow->toMother(windowPoint)) );
    // check the active radius.
    const double hitRadius2 = ( windowPointInHPD.x()*windowPointInHPD.x() +
                                windowPointInHPD.y()*windowPointInHPD.y() );
    if ( hitRadius2 > m_activeRadiusSq ) return StatusCode::FAILURE;

    // check if the HPD is active or dead
    if ( !m_deRichS->hpdIsActive( smartID ) ) return StatusCode::FAILURE;

    // finally, set the window point
    windowPointGlobal =
      geometry()->toGlobal( pvHPDMaster->toMother(windowPointInHPD) );

  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  return a list with all the valid readout channels (smartIDs)
//=========================================================================
StatusCode
DeRichHPDPanel::readoutChannelList ( LHCb::RichSmartID::Vector& readoutChannels ) const
{

  // Square of active radius
  const double activeRadiusSq = m_siliconHalfLengthX*m_siliconHalfLengthX;

  for ( unsigned int PD = 0; PD < m_HPDMax; ++PD )
  {
    // Get HPD row and column numbers outside loops.
    const unsigned int pdCol = PD/m_HPDNumInCol;
    const unsigned int pdPosInCol = PD%m_HPDNumInCol;

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
bool DeRichHPDPanel::detPlanePoint( const Gaudi::XYZPoint& pGlobal,
                                    const Gaudi::XYZVector& vGlobal,
                                    Gaudi::XYZPoint& hitPosition,
                                    const LHCb::RichTraceMode mode ) const
{

  // transform to the Panel coord system.
  Gaudi::XYZVector vInPanel( m_vectorTransf*vGlobal );

  const double scalar = vInPanel.Dot(m_localPlaneNormal);
  if ( scalar == 0.0 ) return false;

  const Gaudi::XYZPoint pInPanel( geometry()->toLocal(pGlobal) );
  const double distance = -m_localPlane.Distance(pInPanel) / scalar;
  const Gaudi::XYZPoint hitInPanel( pInPanel + distance*vInPanel );

  if ( mode.detPlaneBound() == LHCb::RichTraceMode::RespectHPDPanel )
  {
    double u(0.0);
    double v(0.0);
    if ( m_rich == Rich::Rich1 ) {
      u = hitInPanel.y();
      v = hitInPanel.x();
    }
    else {
      u = hitInPanel.x();
      v = hitInPanel.y();
    }

    if ( fabs(u) >= fabs(m_panelColumnSideEdge) ||
         fabs(v) >= m_panelStartColPos ) { return false; }
  }

  hitPosition = geometry()->toGlobal( hitInPanel );
  return true;
}


//=========================================================================
//  findHPDColAndPos
//=========================================================================
bool DeRichHPDPanel::findHPDColAndPos ( const Gaudi::XYZPoint& inPanel,
                                        LHCb::RichSmartID& id ) const
{
  double u(0.0);
  double v(0.0);
  if ( m_rich == Rich::Rich1 ) {
    u = inPanel.y();
    v = inPanel.x();
  }
  else {
    u = inPanel.x();
    v = inPanel.y();
  }

  const unsigned int HPDCol =
    static_cast<unsigned int>(floor(u - m_panelColumnSideEdge) /
                              m_HPDColPitch);
  if (HPDCol >= m_HPDColumns) return false;
  id.setHPDCol( HPDCol );

  unsigned int HPDNumInCol( 0 );
  if (0 == HPDCol%2) {
    HPDNumInCol = static_cast<unsigned int>
      (floor((v - m_panelStartColPosEven) / m_HPDPitch));
  } else {
    HPDNumInCol = static_cast<unsigned int>
      (floor((v - m_panelStartColPosOdd) / m_HPDPitch));
  }

  if (HPDNumInCol >= m_HPDNumInCol) return false;
  id.setHPDNumInCol(HPDNumInCol);

  return true;
}

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

//=========================================================================
//  convert an HPD number to an HPD copy number
//=========================================================================
int DeRichHPDPanel::copyNumber( unsigned int HPDNumber ) const
{
  // extract the copy number from physical volume name
  const std::string::size_type pos = HPDMaster(HPDNumber)->name().find(':');
  if ( std::string::npos == pos ) {
    MsgStream msg ( msgSvc(), myName() );
    msg << MSG::FATAL << "An HPD without a number!" << endmsg;
    return -1;
  }
  return ( atoi( HPDMaster(HPDNumber)->name().substr(pos+1).c_str() ) );
}
