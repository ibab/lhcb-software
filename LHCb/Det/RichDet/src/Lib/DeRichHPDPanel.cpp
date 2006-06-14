
//----------------------------------------------------------------------------
/** @file DeRichHPDPanel.cpp
 *
 *  Implementation file for detector description class : DeRichHPDPanel
 *
 *  $Id: DeRichHPDPanel.cpp,v 1.42 2006-06-14 16:46:04 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#define DERICHHPDPANEL_CPP

// Include files
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "RichDet/DeRichHPDPanel.h"

// MathCore files
#include "Kernel/Transform3DTypes.h"

// DetDesc
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/TabulatedProperty.h"

// GSL
#include "gsl/gsl_math.h"

//----------------------------------------------------------------------------

const CLID& CLID_DeRichHPDPanel = 12010;  // User defined

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


  std::string rich1Location;
  if ( name().find("Magnet") == std::string::npos )
    rich1Location = DeRichLocation::Rich1_old;
  else
    rich1Location = DeRichLocation::Rich1;

  DetectorElement* deRich1;
  SmartDataPtr<DetectorElement> deRich1a(dataSvc(),rich1Location );
  if ( !deRich1a ) {
    // for the testbeam try Rich2
    SmartDataPtr<DetectorElement> deRich1b(dataSvc(),DeRichLocation::Rich2 );
    if ( !deRich1b ) {
      msg << MSG::ERROR << "Could not load DeRich1" << endmsg;
      return StatusCode::FAILURE;
    }
    deRich1 = deRich1b;
  }
  else {
    deRich1 = deRich1a;
  }

  // find the RichSystem
  SmartDataPtr<DeRichSystem> deRichS(dataSvc(),DeRichLocation::RichSystem );
  if ( !deRichS ) {
    msg << MSG::ERROR << "Could not load DeRich System" << endmsg;
    return StatusCode::FAILURE;
  }
  m_deRichS = deRichS;

  m_pixelSize = deRich1->param<double>("RichHpdPixelXsize");
  m_subPixelSize = m_pixelSize/8;
  m_activeRadius = deRich1->param<double>("RichHpdActiveInpRad");
  m_activeRadiusSq = m_activeRadius*m_activeRadius;

  m_pixelColumns = deRich1->param<int>("RichHpdNumPixelCol");
  m_pixelRows = deRich1->param<int>("RichHpdNumPixelRow");

  msg << MSG::DEBUG << "RichHpdPixelsize:" << m_pixelSize << " ActiveRadius:"
      << m_activeRadius << " pixelRows:" << m_pixelRows << " pixelColumns:"
      << m_pixelColumns << endreq;

  m_HPDColumns = param<int>("HPDColumns");
  m_HPDNumInCol = param<int>("HPDNumberInColumn");
  m_HPDMax = m_HPDColumns * m_HPDNumInCol;

  m_HPDPitch = param<double>("HPDPitch");
  m_HPDColPitch = sqrt( 0.75 * m_HPDPitch*m_HPDPitch );
  msg << MSG::DEBUG << "HPDColumns:" << m_HPDColumns << " HPDNumberInColumns:"
      << m_HPDNumInCol << endmsg;

  if ( m_HPDColPitch  < m_activeRadius*2) {
    msg << MSG::WARNING << "The active area is bigger by:"
        << (m_activeRadius*2 - fabs(m_HPDColPitch))/Gaudi::Units::mm
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


  SmartDataPtr<TabulatedProperty>
    HPDdeMag(dataSvc(),
             "/dd/Materials/RichMaterialTabProperties/HpdDemagnification");
  if (!HPDdeMag) {
    msg << MSG::ERROR << "Could not load HpdDemagnification" << endmsg;
    return StatusCode::FAILURE;
  }
  TabulatedProperty::Table DeMagTable = HPDdeMag->table();
  m_deMagFactor[0] = DeMagTable[0].second;
  m_deMagFactor[1] = DeMagTable[1].second;
  msg << MSG::DEBUG << "DeMagFactor(0):" << m_deMagFactor[0]
      << " DeMagFactor(1):" << m_deMagFactor[1] << endreq;

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

  // HPD #0 coordinates
  m_HPD0Centre = pvHPDMaster0->toMother(zero);
  msg << MSG::DEBUG << "Centre of HPDPanel:" << geometry()->toGlobal(zero)
      <<endmsg;
  msg << MSG::DEBUG<< "Centre of HPD#0:" << geometry()->toGlobal(m_HPD0Centre)
      <<endmsg;

  msg << MSG::VERBOSE << "Centre of HPD#0 " << geometry()->lvolume()->
    pvolume(0)->toMother( zero ) << endmsg;
  msg << MSG::VERBOSE << "Centre of HPD#" << m_HPDNumInCol-1 << geometry()->lvolume()->
    pvolume(m_HPDNumInCol-1)->toMother( zero ) << endmsg;
  msg << MSG::VERBOSE << "Centre of HPD#" << m_HPDNumInCol << geometry()->lvolume()->
    pvolume(m_HPDNumInCol)->toMother( zero ) << endmsg;
  msg << MSG::VERBOSE << "Centre of HPD#" << 2*m_HPDNumInCol-1 << geometry()->lvolume()->
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
  m_winR = windowTicks[0];
  m_winRsq = m_winR*m_winR;

  // get the coordinate of the centre of the HPD quarz window
  Gaudi::XYZPoint HPDTop1(0.0, 0.0, m_winR);
  // convert this to HPDS master  coordinates
  Gaudi::XYZPoint HPDTop2 = pvWindow0->toMother(HPDTop1);
  // and to silicon
  m_HPDTop = pvSilicon0->toLocal(HPDTop2);

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
  m_detPlaneZdiff = m_winR - sqrt( m_winRsq - m_activeRadiusSq );
  m_localPlane2 = Gaudi::Transform3D(Gaudi::XYZVector(0.0,0.0,m_detPlaneZdiff))(m_localPlane);
  m_localPlaneNormal2 = m_localPlane2.Normal();

  // Cache information for PDWindowPoint method
  m_vectorTransf = geometry()->matrix();
  m_HPDPanelSolid = geometry()->lvolume()->solid();
  m_kaptonSolid = pvHPDSMaster0->lvolume()->pvolume("pvRichHPDKaptonShield")->
    lvolume()->solid();

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
    m_trans1.push_back( geometry()->matrixInv() * pvHPDMaster->matrixInv() *
                        pvHPDSMaster->matrixInv() * pvWindow->matrixInv() );
    m_trans2.push_back( pvSilicon->matrix() * pvHPDSMaster->matrix() * pvHPDMaster->matrix() );
  }

  msg << MSG::VERBOSE << "----------  End ---------------------------" << endmsg;

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

  const unsigned int HPDNumber = id.hpdCol() * m_HPDNumInCol + id.hpdNumInCol();

  const Gaudi::XYZPoint inSilicon = m_trans2[HPDNumber] * inPanel;

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
        << m_pvHPDMaster[HPDNumber]->name() << endmsg;
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
//  convert a smartID to a point on the inside of the HPD window
//=========================================================================
Gaudi::XYZPoint DeRichHPDPanel::detectionPoint ( const LHCb::RichSmartID& smartID ) const
{
  const unsigned int HPDNumber = smartID.hpdCol() * m_HPDNumInCol + smartID.hpdNumInCol();

  // convert pixel number to silicon coordinates
  const double inSiliconX =
    smartID.pixelCol()*m_pixelSize + m_pixelSize/2.0 - m_siliconHalfLengthX;
  const double inSiliconY =
    m_siliconHalfLengthY - smartID.pixelRow()*m_pixelSize - m_pixelSize/2.0;

  const double inSiliconR = sqrt(inSiliconX*inSiliconX + inSiliconY*inSiliconY);

  // Now calculate the radius at the cathode.
  // To go from the cathode to the anode Ra = Rc*(-d0 + d1*Rc)
  // The minus sign in d0 is for the cross-focussing effect
  // To go from the anode to the cathode solve: d1*Rc^2 - d0*Rc - Ra = 0
  // The difference is that Ra is now possitive.
  // Chose the solution with the minus sign
  const double rInWindow = ( m_deMagFactor[0] -
                             sqrt(gsl_pow_2( m_deMagFactor[0] ) -
                                  4*m_deMagFactor[1]*inSiliconR)) / (2*m_deMagFactor[1]);

  // the minus sign is for the cross-focussing
  const double scaleUp = -rInWindow/inSiliconR;
  const double inWindowX = scaleUp*inSiliconX;
  const double inWindowY = scaleUp*inSiliconY;
  const double inWindowZ = sqrt(m_winRsq-inWindowX*inWindowX-inWindowY*inWindowY);

  return (m_trans1[HPDNumber] * Gaudi::XYZPoint(inWindowX,inWindowY,inWindowZ));
}


//=========================================================================
//  convert a SmartID to a point on the anode (global coord system)
//=========================================================================
Gaudi::XYZPoint DeRichHPDPanel::detPointOnAnode( const LHCb::RichSmartID& smartID ) const
{

  const unsigned int HPDNumber = smartID.hpdCol() * m_HPDNumInCol + smartID.hpdNumInCol();

  // convert pixel number to silicon coordinates
  const double inSiliconX =
    smartID.pixelCol()*m_pixelSize + m_pixelSize/2.0 - m_siliconHalfLengthX;
  const double inSiliconY =
    m_siliconHalfLengthY - smartID.pixelRow()*m_pixelSize - m_pixelSize/2.0;

  Gaudi::XYZPoint inSilicon( inSiliconX, inSiliconY, 0.0 );

  return(geometry()->toGlobal(m_pvHPDMaster[HPDNumber]->
                              toMother(m_pvHPDSMaster[HPDNumber]->
                                       toMother(m_pvSilicon[HPDNumber]->
                                                toMother(inSilicon )))));

}

//=========================================================================
//  convert a point from the global to the panel coodinate system
//=========================================================================
Gaudi::XYZPoint DeRichHPDPanel::globalToPDPanel( const Gaudi::XYZPoint& globalPoint ) const
{
  const Gaudi::XYZPoint localPoint( geometry()->toLocal( globalPoint ) );
  return Gaudi::XYZPoint( localPoint.x(), localPoint.y(), localPoint.z()-m_detPlaneZ );
}

//=========================================================================
//  find an intersection with the inside of the HPD window
//=========================================================================
StatusCode DeRichHPDPanel::PDWindowPoint( const Gaudi::XYZVector& vGlobal,
                                          const Gaudi::XYZPoint& pGlobal,
                                          Gaudi::XYZPoint& windowPointGlobal,
                                          LHCb::RichSmartID& smartID,
                                          const RichTraceMode mode ) const
{

  // transform point and vector to the HPDPanel coordsystem.
  const Gaudi::XYZPoint pInPanel( geometry()->toLocal(pGlobal) );
  Gaudi::XYZVector vInPanel( m_vectorTransf*vGlobal );

  // find the intersection with the detection plane (localPlane2)
  const double scalar = vInPanel.Dot(m_localPlaneNormal2);
  if ( scalar == 0.0 ) return StatusCode::FAILURE;

  const double distance = -m_localPlane2.Distance( pInPanel )/scalar;
  const Gaudi::XYZPoint panelIntersection( pInPanel + distance*vInPanel );

  unsigned int  HPDNumber(0);
  LHCb::RichSmartID id( smartID );

  if ( !findHPDColAndPos(panelIntersection, id) ) return StatusCode::FAILURE;
  // check if the HPD is active or dead
  if ( !m_deRichS->hpdIsActive( id ) ) return StatusCode::FAILURE;

  HPDNumber = id.hpdCol() * m_HPDNumInCol + id.hpdNumInCol();

  if ( mode.detPrecision() == RichTraceMode::circle ) {  // do it quickly

    const double x = panelIntersection.x() - m_HPDCentres[HPDNumber].x();
    const double y = panelIntersection.y() - m_HPDCentres[HPDNumber].y();
    if ( ( x*x + y*y ) > m_activeRadiusSq ) return StatusCode::FAILURE;

    Gaudi::XYZPoint pInHPD( m_pvHPDMaster[HPDNumber]->toLocal( pInPanel ));
    Gaudi::XYZVector vInHPD( m_pvHPDMaster[HPDNumber]->matrix()*vInPanel );
    ISolid::Ticks kaptonTicks;
    if ( 0 != m_kaptonSolid->intersectionTicks(pInHPD, vInHPD, kaptonTicks) )
      return StatusCode::FAILURE;

    windowPointGlobal = geometry()->toGlobal( panelIntersection );

    smartID = id;

    return StatusCode::SUCCESS;
  }

  // Overwise slow

  // find the correct HPD and quartz window inside it
  const IPVolume* pvHPDMaster = m_pvHPDMaster[HPDNumber];

  // just in case
  if ( !pvHPDMaster ) {
    MsgStream msg(msgSvc(), myName() );
    msg << MSG::ERROR << "Inappropriate HPDNumber:" << HPDNumber
        << " from HPDRow:" << id.hpdNumInCol() << " and HPDColumn:" << id.hpdCol() << endreq
        << " x:" << panelIntersection.x()
        << " y:" << panelIntersection.y() <<  endreq;
    return StatusCode::FAILURE;
  }

  Gaudi::XYZPoint pInHPD( m_pvHPDMaster[HPDNumber]->toLocal( pInPanel ));
  Gaudi::XYZVector vInHPD( m_pvHPDMaster[HPDNumber]->matrix()*vInPanel );
  ISolid::Ticks kaptonTicks;
  if ( 0 != m_kaptonSolid->intersectionTicks(pInHPD, vInHPD, kaptonTicks) )
    return StatusCode::FAILURE;

  const IPVolume* pvHPDSMaster = m_pvHPDSMaster[HPDNumber];
  const IPVolume* pvWindow     = m_pvWindow[HPDNumber];
  const ISolid* windowSolid  = pvWindow->lvolume()->solid();

  // convert point to local coordinate systems
  Gaudi::XYZPoint pInWindow( pvWindow->toLocal(pvHPDSMaster->
                                               toLocal(pvHPDMaster->toLocal(pInPanel))) );

  // convert local vector assuming that only the HPD can be rotated
  Gaudi::XYZVector vInHPDMaster( pvHPDMaster->matrix()*vInPanel );

  ISolid::Ticks HPDWindowTicks;
  unsigned int noTicks = windowSolid->intersectionTicks(pInWindow, vInHPDMaster,
                                                        HPDWindowTicks );
  if ( 0 == noTicks ) return StatusCode::FAILURE;

  Gaudi::XYZPoint windowPoint( pInWindow + HPDWindowTicks[1]*vInHPDMaster );
  Gaudi::XYZPoint windowPointInHPD( pvHPDSMaster->
                                    toMother(pvWindow->toMother(windowPoint)) );
  // check the active radius.
  const double hitRadius2 = ( windowPointInHPD.x()*windowPointInHPD.x() +
                              windowPointInHPD.y()*windowPointInHPD.y() );
  if ( hitRadius2 > m_activeRadiusSq ) return StatusCode::FAILURE;

  windowPointGlobal =
    geometry()->toGlobal( pvHPDMaster->toMother(windowPointInHPD) );

  smartID = id;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  return a list with all the valid readout channels (smartIDs)
//=========================================================================
StatusCode
DeRichHPDPanel::readoutChannelList ( LHCb::RichSmartID::Vector& readoutChannels ) const
{

  // Square of active radius
  //const double activeRadiusSq = gsl_pow_2
  //  (m_activeRadius*(m_deMagFactor[0]+m_deMagFactor[1]*m_activeRadius) );
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
                                    const RichTraceMode mode ) const
{

  // transform to the Panel coord system.
  Gaudi::XYZVector vInPanel( m_vectorTransf*vGlobal );

  const double scalar = vInPanel.Dot(m_localPlaneNormal);
  if ( scalar == 0.0 ) return false;

  const Gaudi::XYZPoint pInPanel( geometry()->toLocal(pGlobal) );
  const double distance = -m_localPlane.Distance(pInPanel) / scalar;
  const Gaudi::XYZPoint hitInPanel( pInPanel + distance*vInPanel );

  if ( mode.detPlaneBound() == RichTraceMode::tight)
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

    if ( fabs(u) >= m_panelColumnSideEdge ||
         fabs(v) >= m_panelStartColPos ) { return false; }
  }

  hitPosition = geometry()->toGlobal( hitInPanel );
  return true;
}


//=========================================================================
//  findHPDColAndPos
//=========================================================================
bool DeRichHPDPanel::findHPDColAndPos (const Gaudi::XYZPoint& inPanel,
                                       LHCb::RichSmartID& id) const
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
  double z = localPoint.z() + m_detPlaneZ;
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

  return (geometry()->toGlobal(Gaudi::XYZPoint(x, y, z) ) );
}

//=========================================================================
//  sensitiveVolumeID
//=========================================================================
const int DeRichHPDPanel::sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const
{
  LHCb::RichSmartID id( m_panelRichID );
  return ( smartID(globalPoint,id).isSuccess() ? id : LHCb::RichSmartID() );
}
