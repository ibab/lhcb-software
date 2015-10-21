//----------------------------------------------------------------------------
/** @file DeRichHPDPanel.cpp
 *
 *  Implementation file for detector description class : DeRichHPDPanel
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

// STL
#include <time.h>

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// local
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichSystem.h"

// MathCore files
#include "GaudiKernel/Transform3DTypes.h"

// DetDesc
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/TabulatedProperty.h"

// GSL
#include "gsl/gsl_math.h"

//----------------------------------------------------------------------------

const CLID CLID_DeRichHPDPanel = 12010;  // User defined

// Standard Constructor
DeRichHPDPanel::DeRichHPDPanel(const std::string & name) :
  DeRichPDPanel ( name ) 
{
  // Set the PD type to HPD
  m_pdType = LHCb::RichSmartID::HPDID;
}

// Standard Destructor
DeRichHPDPanel::~DeRichHPDPanel() { }

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
  setMyName( std::string::npos != pos ? name().substr(pos) : "DeRichHPDPanel_NO_NAME" );

  MsgStream msg ( msgSvc(), "DeRichHPDPanel" );

  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "Initialize " << name() << endmsg;

  // register UMS dependency on local geometry
  updMgrSvc()->registerCondition( this, geometry(), &DeRichHPDPanel::geometryUpdate );

  // get the HPD and SiSensor detector elements
  m_DeHPDs.clear();
  m_DeSiSensors.clear();
  const IDetectorElement::IDEContainer & detelems = childIDetectorElements();
  for ( IDetectorElement::IDEContainer::const_iterator det_it = detelems.begin(); 
        det_it != detelems.end(); ++det_it )
  {
    if ( std::string::npos != (*det_it)->name().find("HPD:") ) 
    {

      // get HPD
      SmartDataPtr<DeRichHPD> deHPD( dataSvc(), (*det_it)->name() );
      if ( !deHPD )
      {
        msg << MSG::FATAL << "Non DeRichHPD detector element "
            << (*det_it)->name() << endmsg;
        return StatusCode::FAILURE;
      }

      // save to list of HPDs
      m_DeHPDs.push_back( deHPD );
      // register UMS dependency
      updMgrSvc()->registerCondition( this, deHPD->geometry(), &DeRichHPDPanel::geometryUpdate );

      // get SiSensor
      if ( !deHPD->childIDetectorElements().empty() )
      {
        // save to list of sensors
        m_DeSiSensors.push_back( deHPD->childIDetectorElements().front() );
        // register UMS dependency
        updMgrSvc()->registerCondition( this, 
                                        deHPD->childIDetectorElements().front()->geometry(), 
                                        &DeRichHPDPanel::geometryUpdate );
      }
      else
      {
        msg << MSG::FATAL << "Problem getting SiSensor for HPD " << (*det_it)->name()
            << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  // trigger first UMS update
  const StatusCode update = updMgrSvc()->update(this);
 
  return update;
}

//=========================================================================
// convert a point on the silicon sensor to a smartID
//=========================================================================
StatusCode DeRichHPDPanel::smartID ( const Gaudi::XYZPoint& globalPoint,
                                     LHCb::RichSmartID& id ) const
{
  const Gaudi::XYZPoint inPanel( geometry()->toLocal(globalPoint) );

  // find the HPD row/col of this point if not set
  if ( !id.pdIsSet() )
  {
    if ( !findHPDColAndPos(inPanel,id) ) return StatusCode::FAILURE;
  }

  // check if the HPD is active or dead
  if ( !deRichSys()->pdIsActive( id ) ) return StatusCode::FAILURE;

  const unsigned int HPDNumber = pdNumber(id);
  if ( HPDNumber > nPDs() )
  {
    error() << "Inappropriate HPDNumber : " << HPDNumber;
    return StatusCode::FAILURE;
  }

  const Gaudi::XYZPoint inSilicon( m_DeSiSensors[HPDNumber]->geometry()->toLocalMatrix()*globalPoint );

  double inSiliconX = inSilicon.x();
  double inSiliconY = inSilicon.y();

  // for points too close to the silicon edge subtarct 1/1000 of a mm
  if ( (fabs(inSiliconX)+0.001*Gaudi::Units::mm) > m_siliconHalfLengthX ) 
  {
    const int signX = ( inSiliconX > 0.0 ? 1 : -1 );
    inSiliconX -= signX*0.001*Gaudi::Units::mm;
  }

  if ( (fabs(inSiliconY)+0.001*Gaudi::Units::mm) > m_siliconHalfLengthY ) 
  {
    const int signY = ( inSiliconY > 0.0 ? 1 : -1 );
    inSiliconY -= signY*0.001*Gaudi::Units::mm;
  }

  // if point still outside silicon flag an error
  if ( ( fabs(inSiliconX) - m_siliconHalfLengthX > 1E-3*Gaudi::Units::mm ) ||
       ( fabs(inSiliconY) - m_siliconHalfLengthY > 1E-3*Gaudi::Units::mm )  )
  {
    error() << "Point " << inSilicon << " is outside the silicon box "
            << dePD(HPDNumber)->name() << endmsg;
    return StatusCode::FAILURE;
  }
  
  // pixel 0,0 is at min x and max y (top left corner)
  const unsigned int pixelColumn = (unsigned int)((m_siliconHalfLengthX+inSiliconX)/m_pixelSize);
  const unsigned int pixelRow    = (unsigned int)((m_siliconHalfLengthY-inSiliconY)/m_pixelSize);

  id.setPixelRow ( pixelRow    );
  id.setPixelCol ( pixelColumn );

  // find subpixel (Alice mode)
  const unsigned int subPixel = (unsigned int)
    ((m_siliconHalfLengthY-inSiliconY-pixelRow*m_pixelSize)/m_subPixelSize);
  id.setPixelSubRow ( subPixel );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  find an intersection with the HPD window
//=========================================================================
LHCb::RichTraceMode::RayTraceResult
DeRichHPDPanel::PDWindowPoint( const Gaudi::XYZVector& vGlobal,
                               const Gaudi::XYZPoint& pGlobal,
                               Gaudi::XYZPoint& windowPointGlobal,
                               LHCb::RichSmartID& smartID,
                               const LHCb::RichTraceMode mode ) const
{

  // transform vector to the HPDPanel coordsystem.
  const Gaudi::XYZVector vInPanel( geometry()->toLocalMatrix() * vGlobal );

  // find the intersection with the detection plane
  const double scalar = vInPanel.Dot(m_localPlaneNormal);
  if ( fabs(scalar) < 1e-50 ) return LHCb::RichTraceMode::RayTraceFailed;

  // transform point to the HPDPanel coordinate system
  const Gaudi::XYZPoint pInPanel( geometry()->toLocal(pGlobal) );

  // get panel intersection point
  double distance = -m_localPlane2.Distance(pInPanel) / scalar;
  Gaudi::XYZPoint panelIntersection( pInPanel + distance*vInPanel );

  // Get HPD column and row numbers
  if ( !findHPDColAndPos(panelIntersection,smartID) )
    return LHCb::RichTraceMode::RayTraceFailed;

  // Find the correct DeRichHPD
  const DeRichHPD* HPD = deHPD( pdNumber(smartID) );

  // Refind intersection point using other local plane
  // ( Can reuse scalar as both local planes have the same normal vector )
  distance = -m_localPlane.Distance(pInPanel) / scalar;
  panelIntersection = pInPanel + distance*vInPanel;

  // default acceptance is inside an HPD
  LHCb::RichTraceMode::RayTraceResult res = LHCb::RichTraceMode::InHPDTube;

  // how are the checks to be done ?
  if ( mode.detPrecision() == LHCb::RichTraceMode::SimpleHPDs )
  {
    // do it quickly using a simpliefied HPD acceptance (window description)

    const double x = panelIntersection.x() - HPD->hpdWinInsideCentreInMother().x();
    const double y = panelIntersection.y() - HPD->hpdWinInsideCentreInMother().y();
    if ( ( x*x + y*y ) > m_activeRadiusSq )
    {
      // not in an HPD, but are we in the HPD panel ?
      res = checkPanelAcc(panelIntersection);
    }
    else
    {
      // Inside an HPD
      if ( !deRichSys()->pdIsActive(smartID) ||  // check if the HPD is active or dead
           ( mode.hpdKaptonShadowing() &&       // check for intersection with kapton shield
             HPD->testKaptonShadowing(pInPanel,vInPanel) ) )
      {
        res = LHCb::RichTraceMode::OutsideHPDPanel;
      }
    }

    // set the window point to panel intersection point
    windowPointGlobal = geometry()->toGlobal( panelIntersection );

  }
  else
  {
    // Overwise slower but fully detailed checks using full HPD windows

    // Check for shadowing effects by HPD kapton shields
    if ( mode.hpdKaptonShadowing() && HPD->testKaptonShadowing(pInPanel,vInPanel) )
    {
      // set final point using panel intersection
      windowPointGlobal = geometry()->toGlobal( panelIntersection );
      // set acceptance flag to outside the HPDPanel
      res = LHCb::RichTraceMode::OutsideHPDPanel;
    }
    else
    {

      // convert point and vector to window coordinate systems
      const Gaudi::XYZPoint  pInWindow( HPD->fromPanelToHPDWindow() * pInPanel );
      const Gaudi::XYZVector vInWindow( HPD->fromPanelToHPDWindow() * vInPanel );

      ISolid::Ticks HPDWindowTicks;
      const unsigned int numTicks =
        HPD->HPDWindowSolid()->intersectionTicks( pInWindow, vInWindow, HPDWindowTicks );
      if ( 0 == numTicks )
      {
        // set final point using panel intersection
        windowPointGlobal = geometry()->toGlobal( panelIntersection );
        // not in an HPD, but are we in the HPD panel ?
        res = checkPanelAcc(panelIntersection);
      }
      else
      {

        // get point in HPD entrance window ( 0=outside(best) 1=inside )
        const Gaudi::XYZPoint windowPoint      ( pInWindow + HPDWindowTicks[0]*vInWindow );

        // convert point in window to point in main HPD volume
        const Gaudi::XYZPoint windowPointInHPD ( HPD->fromHPDWindowToHPD()*windowPoint );

        // check the active radius.
        const double hitRadiusSq = ( windowPointInHPD.x()*windowPointInHPD.x() +
                                     windowPointInHPD.y()*windowPointInHPD.y() );
        if ( hitRadiusSq > m_activeRadiusSq )
        {
          // not in an HPD, but are we in the HPD panel ?
          res = checkPanelAcc(windowPointInHPD);
        }

        // finally, set the window point
        //windowPointGlobal = geometry()->toGlobal( HPD->fromHPDToPanel() * windowPointInHPD );
        windowPointGlobal = HPD->geometry()->toGlobal( windowPointInHPD );

        // check if the HPD is active or dead
        if ( !deRichSys()->pdIsActive(smartID) )
        {
          res = LHCb::RichTraceMode::OutsideHPDPanel;
        }

      } // found intersection with HPD window

    } // does not intersect kapton

  } // end of detailed checks

  return res;
}

//=========================================================================
//  return a list with all the valid readout channels (smartIDs)
//=========================================================================
StatusCode
DeRichHPDPanel::readoutChannelList ( LHCb::RichSmartID::Vector& readoutChannels ) const
{

  // Square of active radius
  const double activeRadiusSq = m_siliconHalfLengthX*m_siliconHalfLengthX;

  for ( unsigned int PD = 0; PD < nPDs(); ++PD )
  {
    // Get HPD row and column numbers outside loops.
    const unsigned int pdCol      = PD / nPDsPerCol();
    const unsigned int pdPosInCol = PD % nPDsPerCol();

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
          readoutChannels.push_back ( LHCb::RichSmartID( rich(), side(),
                                                         pdPosInCol, pdCol,
                                                         pixRow, pixCol ) );
        }

      } // loop over pixel columns
    } // loop over pixel rows

  } // loop over HPDs

  return StatusCode::SUCCESS;
}

//=========================================================================
// returns the intersection point with the detection plane
//=========================================================================
LHCb::RichTraceMode::RayTraceResult
DeRichHPDPanel::detPlanePoint( const Gaudi::XYZPoint& pGlobal,
                               const Gaudi::XYZVector& vGlobal,
                               Gaudi::XYZPoint& hitPosition,
                               LHCb::RichSmartID& smartID,
                               const LHCb::RichTraceMode mode ) const
{

  // transform to the Panel coord system.
  const Gaudi::XYZVector vInPanel( geometry()->toLocalMatrix() * vGlobal );

  // find the intersection with the detection plane
  const double scalar = vInPanel.Dot(m_localPlaneNormal);
  if ( fabs(scalar) < 1e-5 ) return LHCb::RichTraceMode::RayTraceFailed;

  // transform point to the HPDPanel coordsystem.
  const Gaudi::XYZPoint pInPanel( geometry()->toLocal(pGlobal) );

  // get panel intersection point
  const double distance = -m_localPlane.Distance(pInPanel) / scalar;
  const Gaudi::XYZPoint panelIntersection( pInPanel + distance*vInPanel );

  // set the (closest) HPD number
  findHPDColAndPos(panelIntersection,smartID);

  // set final position
  hitPosition = geometry()->toGlobal( panelIntersection );

  // return final acceptance
  return ( mode.detPlaneBound() == LHCb::RichTraceMode::RespectHPDPanel ?
           checkPanelAcc(panelIntersection) : LHCb::RichTraceMode::InHPDPanel );
}

//=========================================================================
// findHPDColAndPos
//=========================================================================
bool DeRichHPDPanel::findHPDColAndPos ( const Gaudi::XYZPoint& inPanel,
                                        LHCb::RichSmartID& id ) const
{
  bool OK = true;

  const double u = ( rich() == Rich::Rich1 ? inPanel.y() : inPanel.x() );
  const double v = ( rich() == Rich::Rich1 ? inPanel.x() : inPanel.y() );

  // work out nearest column
  //int HPDCol = static_cast<int>(std::floor((u-m_panelColumnSideEdge)/m_HPDColPitch));
  // CRJ : Faster than floor. Gets it wrong for negative values, but these are reset
  // to 0 anyway so does not matter
  //int HPDCol = (int)((u-m_panelColumnSideEdge)/m_HPDColPitch);
  long int HPDCol = (long int)((u-m_panelColumnSideEdge)*m_OneOverHPDColPitch);
  if      ( HPDCol >= (long int)nPDColumns() ) { OK = false; HPDCol = nPDColumns() - 1; }
  else if ( HPDCol < 0                       ) { OK = false; HPDCol = 0;                }

  // nearest number in column
  //int HPDNumInCol
  //  = ( 0 == HPDCol%2 ?
  //      static_cast<int>(std::floor((v-m_panelStartColPosEven)/m_HPDPitch)) :
  //      static_cast<int>(std::floor((v-m_panelStartColPosOdd)/m_HPDPitch)) );
  // CRJ : Faster than floor. Gets it wrong for negative values, but these are reset
  // to 0 anyway so does not matter
  // int HPDNumInCol = ( 0 == HPDCol%2 ?
  //                     (int)((v-m_panelStartColPosEven)/m_HPDPitch) :
  //                     (int)((v-m_panelStartColPosOdd )/m_HPDPitch) );
  long int HPDNumInCol = ( 0 == HPDCol%2 ?
                           (long int)((v-m_panelStartColPosEven)*m_OneOverHPDPitch) :
                           (long int)((v-m_panelStartColPosOdd )*m_OneOverHPDPitch) );
  if      ( HPDNumInCol >= (long int)nPDsPerCol() ) { OK = false; HPDNumInCol = nPDsPerCol()-1; }
  else if ( HPDNumInCol < 0                       ) { OK = false; HPDNumInCol = 0;              }

  // Set HPD information in RichSmartID
  id.setPD( (int)HPDCol, (int)HPDNumInCol );

  return OK;
}

//=========================================================================
// sensitiveVolumeID
//=========================================================================
int DeRichHPDPanel::sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const
{
  // Create a RichSmartID for this RICH and panel
  LHCb::RichSmartID id( rich(), side() );
  // set the remaining fields from the position
  return ( smartID(globalPoint,id).isSuccess() ?
           id : LHCb::RichSmartID(rich(),side()) );
}

//=========================================================================
// Returns the detector element for the given HPD number
//=========================================================================
const DeRichPD* DeRichHPDPanel::dePD( const unsigned int PDNumber ) const
{
  return deHPD( PDNumber );
}

//=========================================================================
//  generate the transfroms for global <-> local frames
//=========================================================================
StatusCode DeRichHPDPanel::geometryUpdate() 
{
  MsgStream msg ( msgSvc(), "DeRichHPDPanel" );

  SmartDataPtr<DetectorElement> deRich1(dataSvc(),DeRichLocations::Rich1 );
  if ( !deRich1 ) 
  {
    msg << MSG::ERROR << "Could not load DeRich1" << endmsg;
    return StatusCode::FAILURE;
  }

  const Gaudi::XYZPoint zero(0.0, 0.0, 0.0);
  const Gaudi::XYZPoint centreGlobal(geometry()->toGlobal(zero));

  // Work out what Rich/panel I am
  if ( name().find("Rich1") != std::string::npos )
  {
    m_rich = Rich::Rich1;
    if ( centreGlobal.y() > 0.0 )
    {
      m_side = Rich::top;
    }
    else
    {
      m_side = Rich::bottom;
    }
  }
  else if ( name().find("Rich2") != std::string::npos )
  {
    m_rich = Rich::Rich2;
    if ( centreGlobal.x() > 0.0 )
    {
      m_side = Rich::left;
    }
    else
    {
      m_side = Rich::right;
    }
  }
  if ( m_rich == Rich::InvalidDetector ||
       m_side == Rich::InvalidSide )
  {
    msg << MSG::ERROR << "Error initializing HPD panel " << name() << endmsg;
    return StatusCode::FAILURE;
  }

  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "------- Initializing HPD Panel: " << rich()
        << " Panel" << (int)side() << " -------" << endmsg;

  m_pixelSize      = deRich1->param<double>("RichHpdPixelXsize");
  m_subPixelSize   = m_pixelSize/8.0;
  const double activeRadius = deRich1->param<double>("RichHpdActiveInpRad");
  m_activeRadiusSq = activeRadius*activeRadius;

  m_pixelColumns = deRich1->param<int>("RichHpdNumPixelCol");
  m_pixelRows    = deRich1->param<int>("RichHpdNumPixelRow");

  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "RichHpdPixelsize: " << m_pixelSize << " ActiveRadius: "
        << activeRadius << " pixelRows: " << m_pixelRows << " pixelColumns: "
        << m_pixelColumns << endmsg;

  m_PDColumns  = param<int>("HPDColumns");
  m_PDNumInCol = param<int>("HPDNumberInColumn");
  m_PDMax      = nPDColumns() * nPDsPerCol();

  m_HPDPitch = param<double>("HPDPitch");
  m_HPDColPitch = std::sqrt( 0.75 * m_HPDPitch*m_HPDPitch );
  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "HPDColumns:" << nPDColumns() << " HPDNumberInColumns:"
        << nPDsPerCol() << endmsg;

  if ( m_HPDColPitch  < activeRadius*2)
  {
    msg << MSG::WARNING << "The active area is bigger by:"
        << (activeRadius*2 - fabs(m_HPDColPitch))/Gaudi::Units::mm
        << " mm than the column pitch.  There could be loss of photons"
        << endmsg;
  }

  // read the position of the 1st HPD in columns 0 and 1
  const std::vector<double>& startColPos = param<std::vector<double> >("StartColumnPosition");
  // work in u,v coordinates: u is across a column, v is along
  double HPD00u(0.0), HPD00v(0.0),  HPD10v(0.0);
  if ( m_rich == Rich::Rich1 ) 
  {
    HPD00u = startColPos[1];
    HPD00v = startColPos[0];
    HPD10v = startColPos[2];
  }
  else 
  {
    HPD00u = startColPos[0];
    HPD00v = startColPos[1];
    HPD10v = startColPos[3];
  }

  if ( HPD00v > 0.0 ) m_HPDPitch    = -m_HPDPitch;
  if ( HPD00u > 0.0 ) m_HPDColPitch = -m_HPDColPitch;
  m_OneOverHPDPitch    = 1.0 / m_HPDPitch;
  m_OneOverHPDColPitch = 1.0 / m_HPDColPitch;
  m_panelColumnSideEdge = HPD00u - 0.5*m_HPDColPitch;
  m_localOffset = fabs( m_panelColumnSideEdge );

  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "HPDPitch:" << m_HPDPitch << " panelColumnSideEdge:"
        << m_panelColumnSideEdge << endmsg;

  m_panelStartColPosEven = HPD00v - 0.5*m_HPDPitch;
  m_panelStartColPosOdd  = HPD10v - 0.5*m_HPDPitch;

  // use the abs(largest) value as an ovearll panel edge
  m_panelStartColPos = fabs( m_panelStartColPosEven );
  if ( fabs( m_panelStartColPosOdd ) > m_panelStartColPos )
    m_panelStartColPos = fabs( m_panelStartColPosOdd );

  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "panelStartColPosEven:" << m_panelStartColPosEven
        << " panelStartColPosOdd:" << m_panelStartColPosOdd
        << " m_panelStartColPos:" << m_panelStartColPos
        << endmsg;

  // get the first HPD and follow down to the silicon block
  const IPVolume* pvHPDMaster0  = geometry()->lvolume()->pvolume(0);

  // get subMaster volume
  const IPVolume* pvHPDSMaster0 = pvHPDMaster0->lvolume()->pvolume(0);
  if ( pvHPDSMaster0->name().find("HPDSMaster") == std::string::npos )
  {
    msg << MSG::FATAL << "Cannot find HPDSMaster volume : " 
        << pvHPDSMaster0->name() << endmsg;
    return StatusCode::FAILURE;
  }

  // find pvRichHPDSiDet volume
  const IPVolume* pvSilicon0 = pvHPDSMaster0->lvolume()->pvolume("pvRichHPDSiDet");
  if ( pvSilicon0 == NULL ) // multiple HPD volumes
  {
    pvSilicon0 = pvHPDSMaster0->lvolume()->pvolume(10);
    if ( pvSilicon0 == NULL || 
         pvSilicon0->name().find("pvRichHPDSiDet") == std::string::npos )
    {
      msg << MSG::FATAL << "Cannot find pvRichHPDSiDet volume ";
      if ( pvSilicon0 != NULL ) msg << MSG::FATAL << pvSilicon0->name();
      msg << MSG::FATAL << endmsg;
      return StatusCode::FAILURE;
    }
  }

  const ISolid* siliconSolid = pvSilicon0->lvolume()->solid();
  if ( msgLevel(MSG::VERBOSE,msg) )
    msg << MSG::VERBOSE << "About to do a dynamic cast SolidBox" << endmsg;
  const SolidBox* siliconBox = dynamic_cast<const SolidBox*>(siliconSolid);
  if ( !siliconBox )
  {
    error() << "Failed to cast to SolidBox" << endmsg;
    return StatusCode::FAILURE;
  }

  // assume same size for all silicon detectors
  m_siliconHalfLengthX = siliconBox->xHalfLength();
  m_siliconHalfLengthY = siliconBox->yHalfLength();

  // get the pv and the solid for the HPD quartz window
  const IPVolume* pvWindow0 = pvHPDSMaster0->lvolume() ->
    pvolume("pvRichHPDQuartzWindow");
  const ISolid* windowSolid0 = pvWindow0->lvolume()->solid();
  // get the inside radius of the window
  ISolid::Ticks windowTicks;
  unsigned int windowTicksSize = windowSolid0 ->
    intersectionTicks ( Gaudi::XYZPoint  ( 0.0, 0.0, 0.0 ),
                        Gaudi::XYZVector ( 0.0, 0.0, 1.0 ),
                        windowTicks );
  if (windowTicksSize != 2) {
    msg << MSG::FATAL << "Problem getting window radius" << endmsg;
    return StatusCode::FAILURE;
  }
  const double winR = windowTicks[0];

  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "Centre of HPDPanel : " << centreGlobal
        << endmsg;

  if ( msgLevel(MSG::VERBOSE,msg) )
  {
    msg << MSG::VERBOSE << "Ideal local centre of HPD#0 "
        << geometry()->toLocal(deHPD(0)->windowCentreInIdeal()) << endmsg;
    msg << MSG::VERBOSE << "Ideal local centre of HPD#" << nPDsPerCol()-1 << " "
        << geometry()->toLocal(deHPD(nPDsPerCol()-1)->windowCentreInIdeal()) << endmsg;
    msg << MSG::VERBOSE << "Ideal local centre of HPD#" << nPDsPerCol() << " "
        << geometry()->toLocal(deHPD(nPDsPerCol())->windowCentreInIdeal()) << endmsg;
    msg << MSG::VERBOSE << "Ideal local centre of HPD#" << 2*nPDsPerCol()-1 << " "
        << geometry()->toLocal(deHPD(2*nPDsPerCol()-1)->windowCentreInIdeal()) << endmsg;
  }

  // find the top of 3 HPDs to create a detection plane.
  const Gaudi::XYZPoint pointA( deHPD(0)->windowCentreInIdeal() );
  // for second point go to HPD at the end of the column.
  const Gaudi::XYZPoint pointB( deHPD(nPDsPerCol()-1)->windowCentreInIdeal() );
  // now point C at the other end.
  const Gaudi::XYZPoint pointC( deHPD(nPDs()-nPDsPerCol()/2)->windowCentreInIdeal() );

  m_detectionPlane = Gaudi::Plane3D(pointA,pointB,pointC);
  m_localPlane = geometry()->toLocalMatrix() * m_detectionPlane;
  m_localPlaneNormal = m_localPlane.Normal();

  // store the z coordinate of the detection plane
  m_detPlaneZ = geometry()->toLocal(pointA).z();

  // localPlane2 is used when trying to locate the HPD row/column from
  // a point in the panel.
  m_localPlaneZdiff = winR - std::sqrt( winR*winR - m_activeRadiusSq );
  m_localPlane2 = Gaudi::Transform3D(Gaudi::XYZVector(0.0,0.0,m_localPlaneZdiff))(m_localPlane);

  if ( msgLevel(MSG::VERBOSE,msg) )
  {
    msg << MSG::VERBOSE << "Detection plane        " << m_detectionPlane << endmsg;
    msg << MSG::VERBOSE << "Detection plane local  " << m_localPlane << endmsg;
    msg << MSG::VERBOSE << "Local z coord of det plane " << m_detPlaneZ << endmsg;
    msg << MSG::VERBOSE << "Detection plane local2 " << m_localPlane2 << endmsg;
  }
  
  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "Found " << m_DeHPDs.size() << " DeRichHPDs" << endmsg;

  // update transforms
  // create a transform with an offset to accommodate both detector panels in one histogram
  // and correct the x=0 (Rich1) and y=0 (Rich2) to match (more or less) the global coordinates
  ROOT::Math::Translation3D localTranslation;
  if ( rich() == Rich::Rich1 )
  {
    const int sign = ( side() == Rich::top ? 1 : -1 );
    localTranslation =  ROOT::Math::Translation3D( centreGlobal.x(), 
                                                   sign*localOffset(), 
                                                   -detectPlaneZcoord() );
  }
  else
  {
    const int sign = ( side() == Rich::left ? 1 : -1 );
    localTranslation = ROOT::Math::Translation3D( sign*localOffset(), 
                                                  centreGlobal.y(), 
                                                  -detectPlaneZcoord() );
  }
  m_globalToPDPanelTransform = localTranslation * geometry()->toLocalMatrix();
  m_PDPanelToGlobalTransform = m_globalToPDPanelTransform.Inverse();

  return StatusCode::SUCCESS;
}

//=========================================================================

unsigned int DeRichHPDPanel::pdNumber( const LHCb::RichSmartID& smartID ) const
{
  return ( smartID.rich() == rich() && smartID.panel() == side() ?
           smartID.pdCol() * nPDsPerCol() + smartID.pdNumInCol() :
           nPDs() + 1 );
}

//=========================================================================
