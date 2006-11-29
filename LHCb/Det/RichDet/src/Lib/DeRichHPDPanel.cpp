
//----------------------------------------------------------------------------
/** @file DeRichHPDPanel.cpp
 *
 *  Implementation file for detector description class : DeRichHPDPanel
 *
 *  $Id: DeRichHPDPanel.cpp,v 1.50 2006-11-29 16:28:58 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#define DERICHHPDPANEL_CPP

// Include files
#include <time.h>
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

#include "GaudiKernel/IUpdateManagerSvc.h"

// GSL
#include "gsl/gsl_math.h"

//----------------------------------------------------------------------------

const CLID& CLID_DeRichHPDPanel = 12010;  // User defined

// Standard Constructor
DeRichHPDPanel::DeRichHPDPanel() :
  m_rich       ( Rich::InvalidDetector ),
  m_side       ( Rich::InvalidSide     ),
  m_panelRichID( LHCb::RichSmartID()   ),
  m_refactParams(4, 0)
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

  DetectorElement* deRich1;
  SmartDataPtr<DetectorElement> deRich1a(dataSvc(),DeRichLocation::Rich1 );
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
  m_Rich1TotNumHpd = deRich1->param<int>("Rich1TotNumHpd");
  m_Rich2TotNumHpd = deRich1->param<int>("Rich2TotNumHpd");

  // Work out what Rich/panel I am
  if      ( name().find("Rich1") != std::string::npos )
  {
    m_rich = Rich::Rich1;
    if ( centreGlobal.y() > 0.0 ) {
      m_side = Rich::top;
      m_nstart= 0;
      m_nstop = m_Rich1TotNumHpd/2 ;
    }
    else {
      m_side = Rich::bottom;
      m_nstart= m_Rich1TotNumHpd/2;
      m_nstop = m_Rich1TotNumHpd;
    }
  }
  else if ( name().find("Rich2") != std::string::npos )
  {
    m_rich = Rich::Rich2;
    if ( centreGlobal.x() > 0.0 ) {
      m_side = Rich::left;
      m_nstart= m_Rich1TotNumHpd;
      m_nstop = m_Rich1TotNumHpd+m_Rich2TotNumHpd/2;
    }
    else {
      m_side = Rich::right;
      m_nstart= m_Rich1TotNumHpd+m_Rich2TotNumHpd/2;
      m_nstop = m_Rich1TotNumHpd+m_Rich2TotNumHpd;
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
  m_winOutR = windowTicks[1];
  m_winOutRsq = m_winOutR*m_winOutR;

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

  //////////////////////////////////////////////////////////////////////
  msg << MSG::DEBUG << "Start Demagnification update for "<<rich()
      <<" panel"<<(int)side()<<endmsg;

  m_XmlHpdDemagPath = "/dd/Materials/RichMaterialTabProperties/RichHpdDemag";

  m_UseHpdMagDistortions= deRich1->param<int>("UseHpdMagDistortions");
  m_UseBFieldTestMap    = deRich1->param<int>("UseBFieldTestMap");
  m_LongitudinalBField  = deRich1->param<double>("LongitudinalBField");
  m_UseRandomBField     = deRich1->param<int>("UseRandomBField");
  m_RandomBFieldMinimum = deRich1->param<double>("RandomBFieldMinimum");
  m_RandomBFieldMaximum = deRich1->param<double>("RandomBFieldMaximum");
  m_RichHpdQWToSiMaxDist= deRich1->param<double>("RichHpdQWToSiMaxDist");

  if(m_UseHpdMagDistortions) {

    if(m_UseRandomBField) init_mm();
    msg << MSG::INFO<< "Current set is UseBFieldTestMap="<<m_UseBFieldTestMap
        << " LongitudinalBField="<<m_LongitudinalBField
        << " UseRandomBField="<<m_UseRandomBField
        << " within:"<<m_RandomBFieldMinimum<<"-"<<m_RandomBFieldMaximum<<endmsg;

    m_demagCond = condition( "DemagParameters" );
    if ( m_demagCond ) updMgrSvc()->
                         registerCondition(this, m_demagCond.path(),
                                           &DeRichHPDPanel::updateDemagProperties );
    StatusCode sc = updMgrSvc()->update(this);
    if( !sc ) {
      msg << MSG::FATAL << "Demagnification ums update failure."<<endmsg;
      return sc;
    }
  }

  //load old demagnification factors//
  SmartDataPtr<TabulatedProperty> HPDdeMag(dataSvc(),
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
  //////////////////////////////////////////////////////////////////////////////

  if (deRich1->exists("RefractHPDQrtzWin") )
    m_refactParams = deRich1->param<std::vector<double> >("RefractHPDQrtzWin");

  msg << MSG::DEBUG << "Initialisation Complete" << endreq;
  return StatusCode::SUCCESS;
}

//=================================================================================
// Demagnification Tables for simulation and reconstruction (Marco Musy 07/09/2006)
//=================================================================================
StatusCode DeRichHPDPanel::updateDemagProperties() {

  MsgStream msg ( msgSvc(), myName() );
  StatusCode sc = StatusCode::SUCCESS;

  msg << MSG::INFO << "Updating properties for  hpds from nr."
      << m_nstart << " to nr." << m_nstop-1<<endmsg;

  for ( int ihpd = m_nstart; ihpd < m_nstop; ++ihpd ) {

    std::string numb = boost::lexical_cast<std::string>(ihpd);
    std::vector<double> coeff_sim = m_demagCond->paramVect<double>("hpd"+numb+"_sim");

    sc = fillHpdDemagTableSim( m_XmlHpdDemagPath+"Sim_"+numb, coeff_sim, ihpd );
    if (!sc) {
      msg<<MSG::FATAL<< "Could not fill simulation table for "<<("hpd"+numb+"_sim")
         <<" corresponding to demag map "<< (m_XmlHpdDemagPath+"Sim_"+numb) <<endmsg;
      return sc;
    }
    std::vector<double> coeff_rec = m_demagCond->paramVect<double>("hpd"+numb+"_rec");
    sc = fillHpdDemagTableRec( m_XmlHpdDemagPath+"Rec_"+numb, coeff_rec, ihpd );
    if (!sc) {
      msg<<MSG::FATAL<< "Could not fill simulation table for "<<("hpd"+numb+"_sim")
         <<" corresponding to demag map "<< (m_XmlHpdDemagPath+"Rec_"+numb) <<endmsg;
      return sc;
    }
  }
  return sc;
}

//=========================================================================
StatusCode DeRichHPDPanel::fillHpdDemagTableSim( std::string mypath,
                                                 std::vector<double>& coeff_sim,
                                                 int& ihpd ) {
  const int totbins = 50; //do not change
  MsgStream msg ( msgSvc(), myName() );

  //get table
  SmartDataPtr<TabulatedProperty> simDemag( dataSvc(), mypath );
  TabulatedProperty* myTabP = simDemag.ptr();
  if(!myTabP) return 0;

  //reset table
  TabulatedProperty::Table& simTable = myTabP->table();
  simTable.clear();
  simTable.reserve(totbins+1);

  if(coeff_sim.size() < 8)  msg<<MSG::ERROR<< "coeff_sim.size()<8"<<endmsg;
  double r_a0 = coeff_sim.at(0);
  double r_a1 = coeff_sim.at(1);
  double r_a2 = coeff_sim.at(2);
  double r_a3 = coeff_sim.at(3);
  double phi_a0 = coeff_sim.at(4);
  double phi_a1 = coeff_sim.at(5);
  double phi_a2 = coeff_sim.at(6);
  double phi_a3 = coeff_sim.at(7);
  double BLong = 0;
  if(m_UseRandomBField && m_UseBFieldTestMap) {
    BLong = m_RandomBFieldMinimum + number_range( 0, 100 )/100.0
      * (m_RandomBFieldMaximum-m_RandomBFieldMinimum);
    msg<<MSG::INFO<<"Random B field for Hpd"
       <<ihpd<<" set to "<<BLong<<" gauss"<<endmsg;
  } else BLong = m_LongitudinalBField;

  msg<< MSG::DEBUG<<"Using sim parameters r  : "
     <<r_a0<<" "<<r_a1<<" "<<r_a2<<" "<<r_a3<<endreq;
  msg<< MSG::DEBUG<<"                 phi: "
     <<phi_a0<<" "<<phi_a1<<" "<<phi_a2<<" "<<phi_a3<<endreq;

  //Simulation from cathode->anode////////////////////
  for ( int i = 0; i < totbins+1; ++i ) {

    double r_anode = 0, phi_anode = 0;
    double r_cathode = m_activeRadius/totbins *i;

    if( m_UseBFieldTestMap ) {

      //calculate r_anode and deltaphi from hpd test data
      r_anode   =     demag( r_cathode, BLong );
      phi_anode = Delta_Phi( r_cathode, BLong );

    } else { //evaluate distorsions from condDB parameters

      r_anode   = r_a0
        + r_a1 * r_cathode
        + r_a2 * r_cathode * r_cathode
        + r_a3 * r_cathode * r_cathode * r_cathode;
      phi_anode = phi_a0
        + phi_a1 * r_cathode
        + phi_a2 * r_cathode * r_cathode
        + phi_a3 * r_cathode * r_cathode * r_cathode;
    }

    simTable.push_back( TabulatedProperty::Entry( r_anode, phi_anode ) );
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
StatusCode DeRichHPDPanel::fillHpdDemagTableRec( std::string mypath,
                                                 std::vector<double>& coeff_rec,
                                                 int& ihpd ) {
  const int totbins = 50; //do not change
  MsgStream msg ( msgSvc(), myName() );
  //get table
  SmartDataPtr<TabulatedProperty> recDemag( dataSvc(), mypath );
  TabulatedProperty* myTabP = recDemag.ptr();
  if(!myTabP) return 0;

  //reset table
  TabulatedProperty::Table& recTable = myTabP->table();
  recTable.clear();
  recTable.reserve(totbins+1);

  if(coeff_rec.size() < 8)  msg<<MSG::ERROR<< "coeff_rec.size()<8"<<endmsg;
  double r_a0 = coeff_rec.at(0);
  double r_a1 = coeff_rec.at(1);
  double r_a2 = coeff_rec.at(2);
  double r_a3 = coeff_rec.at(3);
  double phi_a0 = coeff_rec.at(4);
  double phi_a1 = coeff_rec.at(5);
  double phi_a2 = coeff_rec.at(6);
  double phi_a3 = coeff_rec.at(7);

  msg<< MSG::DEBUG<<"Hpd"<<ihpd<<": using rec parameters r: "
     <<r_a0<<" "<<r_a1<<" "<<r_a2<<" "<<r_a3
     <<" phi: "<<phi_a0<<" "<<phi_a1<<" "<<phi_a2<<" "<<phi_a3<<endreq;

  //Reconstruction from anode->cathode////////////////////
  for ( int i = 0; i < totbins+1; ++i ) {

    double r_anode = std::min(m_siliconHalfLengthX,m_siliconHalfLengthY)/totbins*i;
    double r_cathode = 0, phi_cathode = 0;

    if( m_UseBFieldTestMap ) {
      //calculate r_cathode and deltaphi from hpd test data
      r_cathode   =        mag( r_anode, 0 );
      phi_cathode = -Delta_Phi( r_cathode, 0 );

    } else { //evaluate distorsions from condDB parameters

      r_cathode   = r_a0
        + r_a1 * r_anode
        + r_a2 * r_anode * r_anode
        + r_a3 * r_anode * r_anode * r_anode;
      phi_cathode = phi_a0
        + phi_a1 * r_anode
        + phi_a2 * r_anode * r_anode
        + phi_a3 * r_anode * r_anode * r_anode;
    }

    recTable.push_back( TabulatedProperty::Entry( r_cathode, phi_cathode ) );
  }
  return StatusCode::SUCCESS;
}

//***********************************************************************
double DeRichHPDPanel::demag(double r, double B) {

  // Radial mapping function and rotation function
  // from Gianluca Aglieri Rinella and Ann Van Lysebetten
  // for the description and correction of the magnetic distortions
  // induced in the axially symmetric case on the HPD detector.
  // See for explanation and reference
  // CERN-THESIS-2005-060 on pages 92-93
  // http://cdsweb.cern.ch/search.py?recid=916449

  // function demag returns the rho coordinate given r and B as arguments
  // r [mm] is the radial coordinate of the led on the entrance window
  // B [G] is the magnetic flux density field (axial)
  // rho [mm] is the radial coordinate on the anode plane from the centre of
  // the cathode image

  // the coefficients of the polynomial fits
  const double a0=0.1771, a1=1.818e-4, a2=2.2197e-5;
  const double b0=-4.713E-4, b1=8.613E-6, b2=-1.2794E-6, b3=+1.3596E-8;

  // a and b calculated from the field
  double a= a0 + a1*B+a2*B*B;
  double b=b0 + b1*B + b2*B*B + b3*B*B*B;

  // rho calculated from r, a and b, second order, 0 constant term polynomial
  return (a*r + b*r*r);

}

//***********************************************************************
double DeRichHPDPanel::mag(double rho, double B) {

  // function mag returns the r coordinate given rho and B as arguments
  // coefficients of the fits for the dependence of the mag law
  // from the magnetic field
  const double c0=5.63057, c1=-1.12557e-2, c2=-4.65552e-4;
  const double d0=0.10377, d1=-1.052e-3, d2=1.995e-4, d3=-2.734e-6;

  // calculate c and d from the field
  double c = c0 + c1*B+c2*B*B;
  double d = d0 + d1*B + d2*B*B + d3*B*B*B;

  // r calculated from rho, c and d, second order, 0 constant term polynomial
  return (c*rho + d*rho*rho);
}
double DeRichHPDPanel::Delta_Phi(double r, const double B) {

  // It returns the rotation angle \Delta\phi [rad ]as a function of the
  // radial entrance
  // window coordinate r [mm] and the axial magnetic flux density B [G].
  // first calculate the coefficients a, b, c from the
  // value of the axial magnetic flux density
  // these are called \Delta\phi_0, \Delta\phi_2, \Delta\phi_3 in the
  // referenced thesis at the beginning

  // calculate a
  const double a1 = 0.0255208;
  const double a2 = -4.78963E-5;
  double a = a1*B + a2*B*B;

  // calculate b
  const double b1 = -6.93056E-6;
  const double b2 = -8.31286E-7;
  const double b3 = 1.23044E-8;
  double b =  b1*B + b2*B*B + b3*B*B*B;

  // calculate c
  const double c1 = 3.38129E-8;
  const double c2 = 1.6186E-8;
  const double c3 = -2.24814E-10;
  double c = c1*B + c2*B*B + c3*B*B*B;

  // this calculates and returns \Delta\phi from a, b, c and r
  return ( a + b*r*r + c*r*r*r ) ;
}
int DeRichHPDPanel::number_mm( void ) {
  int *piState= &rgiState[2];
  int iState1= piState[-2];
  int iState2= piState[-1];
  int iRand= ( piState[iState1] + piState[iState2] ) & (( 1 << 30 ) - 1);
  piState[iState1] = iRand;
  if ( ++iState1 == 55 ) iState1 = 0;
  if ( ++iState2 == 55 ) iState2 = 0;
  piState[-2] = iState1;
  piState[-1] = iState2;
  return iRand >> 6;
}
int DeRichHPDPanel::number_range( int from, int to ) {
  int power, number;
  if ( ( to = to - from + 1 ) <= 1 )  return from;
  for ( power = 2; power < to; power <<= 1 );
  while ( ( number = number_mm( ) & ( power - 1 ) ) >= to );
  return from + number;
}
void DeRichHPDPanel::init_mm( ) {
  int *piState;
  int iState;
  piState = &rgiState[2];
  piState[-2] = 0;
  piState[-1] = 31;
  piState[0] = ( (int) time( NULL ) ) & ( ( 1 << 30 ) - 1 );
  piState[1] = 1;
  for ( iState = 2; iState < 55; iState++ )
    piState[iState] = ( piState[iState-1] + piState[iState-2] )
      & ( ( 1 << 30 ) - 1 );
  return;
}

//=========================================================================
Gaudi::XYZPoint DeRichHPDPanel::demagToCathode_new( int HPDNumber,
                                                    double inSiliconX,
                                                    double inSiliconY ) const
{
  MsgStream msg ( msgSvc(), myName() );

  //load demagnification table
  SmartDataPtr<TabulatedProperty> dem(dataSvc(),m_XmlHpdDemagPath+"Rec_"+
                                      boost::lexical_cast<std::string>(HPDNumber));
  if (!dem) {
    msg<<MSG::ERROR<<"Could not load "<<(m_XmlHpdDemagPath+"Rec_")<<HPDNumber<<endmsg;
    return Gaudi::XYZPoint(0,0,0);
  }
  TabulatedProperty::Table demag = dem->table();
  int lenght = demag.size()-1;
  if(lenght<2) {
    msg<<MSG::ERROR<<"Demag Table too short or empty. "<<endmsg;
    return Gaudi::XYZPoint(0,0,0);
  }
  double inSiliconR = sqrt(inSiliconX*inSiliconX + inSiliconY*inSiliconY);
  double binwidth   = std::min(m_siliconHalfLengthX, m_siliconHalfLengthY) / lenght;
  int bin = int( inSiliconR / binwidth);

  std::pair<double, double> pair1 = demag.at(bin);
  std::pair<double, double> pair2 = demag.at(bin+1);
  double r1 = pair1.first;
  double r2 = pair2.first;
  double p1 = pair1.second;
  double p2 = pair2.second;
  double result_r  = (r2-r1)/binwidth * (inSiliconR - binwidth*bin) + r1;
  double result_phi= (p2-p1)/binwidth * (inSiliconR - binwidth*bin) + p1;

  double anodePhi = std::atan2( inSiliconY, inSiliconX );
  if(inSiliconY < 0) anodePhi += 6.2832;
  double new_phi=0;
  if( anodePhi+result_phi+ 3.1416 >  6.2832 ) new_phi= anodePhi+result_phi-3.1416;
  else new_phi= anodePhi+result_phi + 3.1416;

  if(result_r > m_activeRadius) {
    msg<<MSG::WARNING<<"Demagnification for Hpd" << HPDNumber
       << " goes beyond active photocathode" << " r= "
       << inSiliconR << " -> " << result_r <<" Forced to "<< m_activeRadius <<endmsg;
    result_r = m_activeRadius;
  }
  double inWindowX = result_r * cos(new_phi);
  double inWindowY = result_r * sin(new_phi);
  double inWindowZ = sqrt(m_winRsq-inWindowX*inWindowX-inWindowY*inWindowY);

  //msg<<MSG::DEBUG<<"New demag r= "<<inSiliconR<<" -> "<<result_r<<endmsg;
  //msg<<MSG::DEBUG<<"inWindow  "<<inWindowX<<" "<<inWindowY<<" "<<inWindowZ<<endmsg;

  return Gaudi::XYZPoint(inWindowX,inWindowY,inWindowZ);
}

//=========================================================================
Gaudi::XYZPoint DeRichHPDPanel::demagToCathode_old( double inSiliconX,
                                                    double inSiliconY ) const
{
  MsgStream msg ( msgSvc(), myName() );

  const double inSiliconR = sqrt(inSiliconX*inSiliconX + inSiliconY*inSiliconY);

  // Now calculate the radius at the cathode.
  // To go from the cathode to the anode Ra = Rc*(-d0 + d1*Rc)
  // The minus sign in d0 is for the cross-focussing effect
  // To go from the anode to the cathode solve: d1*Rc^2 - d0*Rc - Ra = 0
  // The difference is that Ra is now positive.
  // Chose the solution with the minus sign
  double rInWindow = ( m_deMagFactor[0] -
                       sqrt(gsl_pow_2( m_deMagFactor[0] ) -
                            4*m_deMagFactor[1]*inSiliconR))/(2*m_deMagFactor[1]);
  //if(rInWindow > m_activeRadius) {
  //  msg<<MSG::WARNING<<"Demagnification for Hpd" << HPDNumber
  //     <<" goes beyond active photocathode" <<" r= "
  //     <<inSiliconR<<" -> "<<rInWindow<<" Forced to "<< m_activeRadius <<endmsg;
  //  rInWindow = m_activeRadius;
  //}
  const double rOutWin = rInWindow + m_refactParams[3]*gsl_pow_3(rInWindow)+
    m_refactParams[2]*gsl_pow_2(rInWindow)+m_refactParams[1]*rInWindow+
    m_refactParams[0];
  // the minus sign is for the cross-focussing
  const double scaleUp = (inSiliconR>0 ? -rOutWin/inSiliconR : 0 );
  const double outWindowX = scaleUp*inSiliconX;
  const double outWindowY = scaleUp*inSiliconY;
  const double outWindowZ = sqrt(m_winOutRsq-outWindowX*outWindowX-outWindowY*outWindowY);

  //msg<<MSG::INFO<< "Old demag r= "<<inSiliconR<<" -> "<<rInWindow<<endmsg;
  //msg<<MSG::INFO<< "inWindow  "<<inWindowX<<" "<<inWindowY<<" "<<inWindowZ<<endmsg;

  return Gaudi::XYZPoint(outWindowX,outWindowY,outWindowZ);

}
//=========================================================================
void DeRichHPDPanel::testdemagnification( int HPDNumber, double x, double y,
                                          double z, double B ) {
  //produce some text output for testing, this method
  //can be called from an external algorithm

  MsgStream msg ( msgSvc(), myName() );

  //print input coords on cathode
  msg<<MSG::INFO<< "TESTDEMAG on cath 1 "<<HPDNumber<<" "<<copyNumber(HPDNumber)
     <<std::setprecision(7)<< " " << x << " " << y << " " << z << endmsg;


  //find corresponding r_a and deltaphi_a from demag to anode
  //print coords on anode
  Gaudi::XYZPoint outa = demagToAnode_test( copyNumber(HPDNumber), x, y );
  msg<<MSG::INFO<< "TESTDEMAG on anod 2 "<<HPDNumber<<" "<<copyNumber(HPDNumber)
     <<std::setprecision(7)<< " " << outa.x() << " " << outa.y() << " " << outa.z()<<endmsg;

  //go back to cathode through new demag tables
  //print coords back on cathode
  Gaudi::XYZPoint outc = demagToCathode_new( copyNumber(HPDNumber), outa.x(), outa.y() );
  msg<<MSG::INFO<< "TESTDEMAG on cath 3 "<<HPDNumber<<" "<<copyNumber(HPDNumber)
     <<std::setprecision(7)<< " " << outc.x() << " " <<outc.y()<< " " <<outc.z()<<endmsg;

  //same: go back to cathode through old demag tables
  Gaudi::XYZPoint outo = demagToCathode_old( outa.x(), outa.y() );
  msg<<MSG::INFO<< "TESTDEMAG on cath 4 "<<HPDNumber<<" "<<copyNumber(HPDNumber)
     <<std::setprecision(7)<< " " << outo.x() << " " <<outo.y()<< " " <<outo.z()<<endmsg;

  //go back to cathode in the presence of a field B.
  //parameters a_i for demag must be adapted accordingly (see CERN-THESIS-2005-060)
  double rcath   = mag(sqrt(outa.x()*outa.x()+outa.y()*outa.y()), B);
  double deltaphi= -Delta_Phi(rcath, B);
  double phi = atan2(-outa.y(), -outa.x()); if(-outa.y()<0) phi+= 6.2832;
  phi += deltaphi;
  double xcath = rcath*cos(phi);
  double ycath = rcath*sin(phi);
  msg<<MSG::INFO<< "TESTDEMAG on cath 5 "<<HPDNumber<<" "<<copyNumber(HPDNumber)
     <<std::setprecision(9)<< " " << xcath << " " << ycath << " " << outa.z() << endmsg;
}

//=========================================================================
Gaudi::XYZPoint DeRichHPDPanel::demagToAnode_test( int HPDNumber,
                                                   double cathX, double cathY ) {
  //This method is the same as getPositionOnAnode in
  //Sim/GaussRICH/src/PhysProcess/RichHpdDeMag.cpp. It is repeated here
  //for fast testing only purposes by testdemagnification method.

  MsgStream msg ( msgSvc(), myName() );
  SmartDataPtr<TabulatedProperty> dem(dataSvc(), m_XmlHpdDemagPath+"Sim_"+
                                      boost::lexical_cast<std::string>(HPDNumber));
  if (!dem) {
    msg<<MSG::ERROR<<"Could not load "<<(m_XmlHpdDemagPath+"Sim_")<<HPDNumber<<endmsg;
    return Gaudi::XYZPoint(0,0,0);
  }
  TabulatedProperty::Table demag = dem->table();
  int lenght = demag.size()-1;
  if(lenght<2) {
    msg<<MSG::ERROR<<"Demag Table too short or empty. "<<endmsg;
    return Gaudi::XYZPoint(0,0,0);
  }

  double CathodeRadius = sqrt(cathX*cathX+cathY*cathY);
  double CathodePhi = atan2(cathY,cathX);
  if(cathY < 0) CathodePhi += 6.2832;
  double binwidth = m_activeRadius / lenght;
  int bin = int( CathodeRadius / binwidth);

  std::pair<double, double> pair1 = demag.at(bin);
  std::pair<double, double> pair2 = demag.at(bin+1);
  double r1 = pair1.first;
  double r2 = pair2.first;
  double p1 = pair1.second;
  double p2 = pair2.second;
  double result_r  = (r2-r1)/binwidth * (CathodeRadius - binwidth*bin) + r1;
  double result_phi= (p2-p1)/binwidth * (CathodeRadius - binwidth*bin) + p1;

  double new_phi=0;
  if( CathodePhi+result_phi+ 3.1416 >  6.283 ) new_phi = CathodePhi+result_phi - 3.1416;
  if( CathodePhi+result_phi+ 3.1416 <= 6.283 ) new_phi = CathodePhi+result_phi + 3.1416;

  //msg<<MSG::DEBUG<<"Test demag r= "<<CathodeRadius<<" -> "<<result_r<<endmsg;
  //msg<<MSG::DEBUG<<"inWindow  "<<inWindowX<<" "<<inWindowY<<" "<<inWindowZ<<endmsg;

  double onAnodeX = result_r * cos(new_phi);
  double onAnodeY = result_r * sin(new_phi);
  double onAnodeZ = -(m_RichHpdQWToSiMaxDist-m_winR);

  return Gaudi::XYZPoint( onAnodeX, onAnodeY, onAnodeZ );
}

//=========================================================================
//  convert a smartID to a point on the inside of the HPD window
//=========================================================================
Gaudi::XYZPoint DeRichHPDPanel::detectionPoint ( const LHCb::RichSmartID& smartID ) const
{
  unsigned int HPDNumber = smartID.hpdCol() * m_HPDNumInCol + smartID.hpdNumInCol();

  // convert pixel number to silicon coordinates
  double inSiliconX =
    smartID.pixelCol()*m_pixelSize + m_pixelSize/2.0 - m_siliconHalfLengthX;
  double inSiliconY =
    m_siliconHalfLengthY - smartID.pixelRow()*m_pixelSize - m_pixelSize/2.0;

  Gaudi::XYZPoint pointOnWin(0,0,0);

  if(m_UseHpdMagDistortions) { //M.Musy 07/09/2006

    pointOnWin = demagToCathode_new(copyNumber(HPDNumber), inSiliconX, inSiliconY);

  } else { //old demag parameters//

    pointOnWin = demagToCathode_old( inSiliconX, inSiliconY);

  }
  return (m_HPDWindowToGlobal[HPDNumber] * pointOnWin);
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
                                          const LHCb::RichTraceMode mode ) const
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

  if ( mode.detPrecision() == LHCb::RichTraceMode::SimpleHPDs ) {  // do it quickly

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

//=========================================================================
//  convert an HPD number to an HPD copy number
//=========================================================================
int DeRichHPDPanel::copyNumber( unsigned int HPDNumber ) const
{
  // extract the copy number from physical volume name
  const std::string::size_type pos = m_pvHPDMaster[HPDNumber]->name().find(':');
  if ( std::string::npos == pos ) {
    MsgStream msg ( msgSvc(), myName() );
    msg << MSG::FATAL << "An HPD  without a number!" << endmsg;
    return -1;
  }
  return (atoi( m_pvHPDMaster[HPDNumber]->name().substr(pos+1).c_str() ) );
}
