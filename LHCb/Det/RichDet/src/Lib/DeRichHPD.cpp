
//=============================================================================
/** @file DeRichHPD.cpp
 *
 * Implementation file for class : DeRichHPD
 *
 * $Id: DeRichHPD.cpp,v 1.21 2010-01-14 16:39:03 papanest Exp $
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-09-19
 */
//=============================================================================

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

// DetDesc
#include "DetDesc/Condition.h"
#include "DetDesc/SolidBox.h"

// local
#include "RichDet/DeRichHPD.h"
#include "RichDet/DeRich.h"
#include "RichDet/DeRichSystem.h"

// GSL
#include "gsl/gsl_math.h"

// Boost
#include "boost/lexical_cast.hpp"

//=============================================================================

const CLID CLID_DERichHPD = 12015;  // User defined

// anon namespace
namespace
{
  static const std::string& XmlHpdDemagPath = "/dd/Materials/RichMaterialTabProperties/RichHpdDemag";
  static const int totbins = 50; //do not change
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichHPD::DeRichHPD(const std::string & name):
  DeRichBase          ( name ),
  m_deSiSensor        ( NULL ),
  m_pvWindow          ( NULL ),
  m_windowSolid       ( NULL ),
  m_kaptonSolid       ( NULL ),
  m_demagMapR         ( NULL ),
  m_demagMapPhi       ( NULL ),
  m_magMapR           ( NULL ),
  m_magMapPhi         ( NULL ),
  m_hpdQuantumEffFunc ( NULL ),
  m_refactParams      ( 4,  0)
{ }

//=============================================================================
// Destructor
//=============================================================================
DeRichHPD::~DeRichHPD()
{
  cleanUpInterps();
}

//=============================================================================
// Clean up interpolators
//=============================================================================
void DeRichHPD::cleanUpInterps()
{
  if ( m_demagMapR )   { delete m_demagMapR;   m_demagMapR   = NULL; }
  if ( m_demagMapPhi ) { delete m_demagMapPhi; m_demagMapPhi = NULL; }
  if ( m_magMapR )     { delete m_magMapR;     m_magMapR     = NULL; }
  if ( m_magMapPhi )   { delete m_magMapPhi;   m_magMapPhi   = NULL; }
  if ( m_hpdQuantumEffFunc && flags[6] )
  {
    delete m_hpdQuantumEffFunc; m_hpdQuantumEffFunc = NULL;
  }
}

//=============================================================================
// Retrieve reference to class defininition structure
//=============================================================================
const CLID& DeRichHPD::classID()
{
  return CLID_DERichHPD;
}

//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRichHPD::initialize ( )
{
  MsgStream msg( msgSvc(), "DeRichHPD" );
  msg << MSG::DEBUG << "Initialize " << name() << endmsg;

  // store the name of the HPD, without the /dd/Structure part
  const std::string::size_type pos = name().find("HPD:");
  setMyName( std::string::npos != pos ? name().substr(pos) : "DeRichHPD_NO_NAME" );

  // extract HPD number from detector element name
  const std::string::size_type pos2 = name().find(":");
  if ( std::string::npos == pos2 ) {
    msg << MSG::FATAL << "An HPD without a number!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_number = atoi( name().substr(pos2+1).c_str() );

  StatusCode sc = getParameters();
  if ( sc.isFailure() ) return sc;

  msg << MSG::DEBUG << "RichHpdPixelsize:" << m_pixelSize << " ActiveRadius:"
      << m_activeRadius << endmsg;
  msg << MSG::DEBUG << "DeMagFactor(0): " << m_deMagFactor[0]
      << " DeMagFactor(1): " << m_deMagFactor[1] << endmsg;

  // find the subMaster volume, normally the first physical volume
  const IPVolume * pvHPDSMaster = geometry()->lvolume()->pvolume(0);
  if ( pvHPDSMaster->name().find("HPDSMaster") == std::string::npos )
  {
    msg << MSG::FATAL << "Cannot find HPDSMaster volume; " << pvHPDSMaster->name() << endmsg;
    return StatusCode::FAILURE;
  }

  // find pvRichHPDSiDet volume
  const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume("pvRichHPDSiDet");
  if ( pvSilicon == NULL ) // multiple HPD volumes
  {
    pvSilicon = pvHPDSMaster->lvolume()->pvolume(10);
    if ( pvSilicon == NULL || pvSilicon->name().find("pvRichHPDSiDet") == std::string::npos )
    {
      msg << MSG::FATAL << "Cannot find pvRichHPDSiDet volume ";
      if ( pvSilicon != NULL ) msg << MSG::FATAL << pvSilicon->name();
      msg << MSG::FATAL << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // pointer to kapton
  m_kaptonSolid =
    pvHPDSMaster->lvolume()->pvolume("pvRichHPDKaptonShield")->lvolume()->solid();

  const ISolid* siliconSolid = pvSilicon->lvolume()->solid();
  msg << MSG::VERBOSE << "About to do a dynamic cast SolidBox "
      << siliconSolid->name() << endmsg;
  const SolidBox* siliconBox = dynamic_cast<const SolidBox*>(siliconSolid);
  if ( !siliconBox )
  {
    msg << MSG::FATAL << " Failed to get SolidBox for silicon" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get sizes of silicon box
  m_siliconHalfLengthX = siliconBox->xHalfLength();
  m_siliconHalfLengthY = siliconBox->yHalfLength();

  // get the pv and the solid for the HPD quartz window
  m_pvWindow  = pvHPDSMaster->lvolume()->pvolume("pvRichHPDQuartzWindow");
  m_windowSolid = m_pvWindow->lvolume()->solid();
  // find intersections with the window
  ISolid::Ticks windowTicks;
  const unsigned int windowTicksSize =
    m_windowSolid->intersectionTicks( Gaudi::XYZPoint(0.0, 0.0, 0.0),
                                      Gaudi::XYZVector(0.0, 0.0, 1.0),
                                      windowTicks );
  if ( windowTicksSize != 2 )
  {
    msg << MSG::FATAL << "Problem getting window radius" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get inner and outer window radii
  const double winInR = windowTicks[0];
  m_winInRsq = winInR*winInR;
  const double winOutR = windowTicks[1];
  m_winOutRsq = winOutR*winOutR;
  msg << MSG::DEBUG << "winInR = " << winInR << " winOutR = " << winOutR << endmsg;

  // get the pointer to the silicon sensor detector element
  m_deSiSensor = ( !childIDetectorElements().empty() ?
                   childIDetectorElements().front() : NULL );
  if ( !m_deSiSensor )
  {
    msg << MSG::ERROR << "Cannot find SiSensor detector element for HPD " << myName() << endmsg;
    return StatusCode::FAILURE;
  }


  // register updates for the locally cached geometry information
  updMgrSvc()->registerCondition(this, geometry(),
                                 &DeRichHPD::updateTransformations );
  sc = updMgrSvc()->update(this);
  if ( sc.isFailure() ) {
    msg << MSG::FATAL << "ums update failure for transformations"<<endmsg;
    return sc;
  }

  // get quantum efficiency tabulated property from LHCBCOND if available
  SmartDataPtr<DeRichSystem> deRichS( dataSvc(), DeRichLocations::RichSystem );
  if ( !deRichS )
  {
    msg << MSG::ERROR << "Cannot locate RichSystem at " <<  DeRichLocations::RichSystem << endmsg;
    return StatusCode::FAILURE;
  }

  if ( deRichS->exists( "HpdQuantumEffCommonLoc" ) )  // use hardware ID to locate QE
  {
    // convert copy number to smartID
    LHCb::RichSmartID id(deRichS->richSmartID(Rich::DAQ::HPDCopyNumber(m_number)));
    std::string qePath = deRichS->param<std::string>("HpdQuantumEffCommonLoc");
    std::string hID( deRichS->hardwareID(id) );
    SmartDataPtr<TabulatedProperty> hpdQuantumEffTabProp( dataSvc(), qePath+hID );
    if ( !hpdQuantumEffTabProp )
    {
      msg << MSG::FATAL << "Could not load HPD's Quantum Efficiency tabproperty for " << myName()
          << " from " << qePath+hID <<endmsg;
      return StatusCode::FAILURE;
    }
    m_hpdQuantumEffFunc = new Rich::TabulatedProperty1D( hpdQuantumEffTabProp );
    flags[6] = true;
    debug() << "HPD:" << id << endmsg;
  }
  else           // use copy number to locate QE
  {
    if ( !hasCondition("QuantumEffTable") )
    {
      // use common QE for all HPDs
      SmartDataPtr<DeRich> deRich1( dataSvc(), DeRichLocations::Rich1 );
      m_hpdQuantumEffFunc = deRich1->nominalHPDQuantumEff();
    }
    else
    {
      // get QE from the CondDB at the location stored in condition QuantumEffTable
      SmartRef<Condition> hpdQuantumEffCond = condition("QuantumEffTable");
      SmartDataPtr<TabulatedProperty> hpdQuantumEffTabProp( dataSvc(), hpdQuantumEffCond.path() );
      if ( !hpdQuantumEffTabProp )
      {
        msg<<MSG::FATAL<<"Could not load HPD's Quantum Efficiency tabproperty for "<<myName()<<endmsg;
        return StatusCode::FAILURE;
      }
      m_hpdQuantumEffFunc = new Rich::TabulatedProperty1D( hpdQuantumEffTabProp );
      flags[6] = true;
    }
  }
  msg << MSG::DEBUG << "QE from location:" << m_hpdQuantumEffFunc->tabProperty()->name() << endmsg;
  debug() << "QE from location:" << m_hpdQuantumEffFunc->tabProperty()->name() << endmsg;
  debug() << m_hpdQuantumEffFunc->tabProperty() << endmsg;

  // Magnetic Distortions
  // Make interpolators
  m_demagMapR   = new Rich::TabulatedFunction1D();
  m_demagMapPhi = new Rich::TabulatedFunction1D();
  m_magMapR     = new Rich::TabulatedFunction1D();
  m_magMapPhi   = new Rich::TabulatedFunction1D();

  if (m_UseHpdMagDistortions)
  {
    //if(m_UseRandomBField) init_mm();
    msg << MSG::DEBUG<< "Current set is UseBFieldTestMap="<<m_UseBFieldTestMap
        << " LongitudinalBField="<<m_LongitudinalBField
      //<< " UseRandomBField="<<m_UseRandomBField
      //<< " within:"<<m_RandomBFieldMinimum<<"-"<<m_RandomBFieldMaximum
        << endmsg;

    if ( hasCondition( "DemagParameters" ) ) {
      m_demagCond = condition( "DemagParameters" );
      updMgrSvc()->registerCondition(this, m_demagCond.path(),
                                     &DeRichHPD::updateDemagProperties );
    }
    else {
      m_demagCond = 0;
    }
    
    sc = updMgrSvc()->update(this);
    if ( sc.isFailure() ) {
      msg << MSG::FATAL << "Demagnification ums update failure."<<endmsg;
      return sc;
    }

  }

  return sc;
}

//=========================================================================
//  getParameters
//=========================================================================
StatusCode DeRichHPD::getParameters ( )
{
  SmartDataPtr<DetectorElement> deRich1(dataSvc(),DeRichLocations::Rich1 );
  if ( !deRich1 )
  {
    error() << "Could not load DeRich1" << endmsg;
    return StatusCode::FAILURE;
  }

  m_pixelSize = deRich1->param<double>("RichHpdPixelXsize");
  m_activeRadius = deRich1->param<double>("RichHpdActiveInpRad");

  if (deRich1->exists("RefractHPDQrtzWin") )
    m_refactParams = deRich1->param<std::vector<double> >("RefractHPDQrtzWin");
  else
  {
    warning() << "No parameters for refraction on HPD window! "
              << "Corrections will not work properly!" << endmsg;
  }

  //m_UseHpdMagDistortions = 0 != deRich1->param<int>("UseHpdMagDistortions");
  m_UseHpdMagDistortions = true;
  m_UseBFieldTestMap     = ( 0 != deRich1->param<int>("UseBFieldTestMap") );
  m_LongitudinalBField   = deRich1->param<double>("LongitudinalBField");
  //m_UseRandomBField     = deRich1->param<int>("UseRandomBField");
  //m_RandomBFieldMinimum = deRich1->param<double>("RandomBFieldMinimum");
  //m_RandomBFieldMaximum = deRich1->param<double>("RandomBFieldMaximum");

  // load old demagnification factors
  SmartDataPtr<TabulatedProperty> HPDdeMag
    (dataSvc(),"/dd/Materials/RichMaterialTabProperties/HpdDemagnification");
  if (!HPDdeMag)
  {
    fatal() << "Could not load HpdDemagnification" << endmsg;
    return StatusCode::FAILURE;
  }
  const TabulatedProperty::Table & DeMagTable = HPDdeMag->table();
  m_deMagFactor[0] = DeMagTable[0].second;
  m_deMagFactor[1] = DeMagTable[1].second;

  return StatusCode::SUCCESS;
}


//=========================================================================
// update the localy cached transforms
//=========================================================================
StatusCode DeRichHPD::updateTransformations ( )
{
  // Do not print message the first time
  if ( flags[2] )
  {
    info() << "Updating geometry transformations for HPD:" << m_number <<endmsg;
  }
  // print the message the following times.
  flags[2] = true;

  // find the subMaster volume, normally the first physical volume
  const IPVolume * pvHPDSMaster = geometry()->lvolume()->pvolume(0);
  if ( pvHPDSMaster->name().find("HPDSMaster") == std::string::npos )
  {
    fatal() << "Cannot find HPDSMaster volume; " << pvHPDSMaster->name() << endmsg;
    return StatusCode::FAILURE;
  }
  const IPVolume * pvKapton = pvHPDSMaster->lvolume()->pvolume("pvRichHPDKaptonShield");

  // Transformation from HPD window to global coord system
  m_fromWindowToGlobal = geometry()->toGlobalMatrix() * pvHPDSMaster->matrixInv() *
    m_pvWindow->matrixInv();
  // Transformation from HPD panel to HPD window coord system
  m_fromPanelToWindow = m_pvWindow->matrix() * pvHPDSMaster->matrix() *
    geometry()->ownMatrix();
  // Transformation from HPD panel to kapton coord system
  m_fromPanelToKapton = pvKapton -> matrix() * pvHPDSMaster->matrix() *
    geometry()->ownMatrix();
  // Transformation for HPD window to HPD coord system
  m_fromWindowToHPD = pvHPDSMaster->matrixInv() * m_pvWindow->matrixInv();
  // Transformation for HPD to HPD-Panel coord system
  m_fromHPDToPanel = geometry()->ownMatrix().Inverse();

  // cache the position of the inside window centre in the panel coordinate system
  m_windowInsideCentreMother = geometry()->ownMatrix().Inverse() *
    pvHPDSMaster->matrixInv() *
    m_pvWindow->toMother(Gaudi::XYZPoint(0,0,std::sqrt(m_winInRsq)));

  // from silicon sensor to HPD including misalignment
  m_SiSensorToHPDMatrix = m_deSiSensor->geometry()->ownMatrix().Inverse();

  return StatusCode::SUCCESS;
}

//=================================================================================
// Demagnification Tables for simulation and reconstruction (Marco Musy 07/09/2006)
//=================================================================================
StatusCode DeRichHPD::updateDemagProperties()
{
  // Do not print message the first time
  if ( flags[1] )
  {
    info() << "Updating Demag properties for HPD:" << m_number <<endmsg;
  }
  // print the message the following times.
  flags[1] = true;

  // Initialise the demagnifcation
  StatusCode sc = fillHpdDemagTable();
  if (!sc) {
    fatal() << "Could not initialise demagnification table for HPD:" << m_number <<endmsg;
    return sc;
  }

  // Initialise the magnification
  sc = fillHpdMagTable();
  if (!sc) {
    fatal() << "Could not initialise magnification table for HPD:" << m_number <<endmsg;
    return sc;
  }

  return sc;
}

//=========================================================================
//  fillHpdDemagTable
//=========================================================================
StatusCode DeRichHPD::fillHpdDemagTable()
{
  const std::string paraLoc = "hpd"+boost::lexical_cast<std::string>(m_number)+"_sim";
  const std::vector<double> & coeff_sim = m_demagCond->paramVect<double>(paraLoc);

  SmartDataPtr<TabulatedProperty> dem(dataSvc(), XmlHpdDemagPath+"Sim_"+
                                      boost::lexical_cast<std::string>(m_number));
  if (!dem) {
    error() << "Could not load "<<(XmlHpdDemagPath+"Sim_")<<m_number<<endmsg;
    return StatusCode::FAILURE;
  }
  TabulatedProperty::Table & simTable = dem->table();
  simTable.clear();
  simTable.reserve(totbins+1);

  // working data tables, used to initialise the interpolators
  std::map<double,double> tableR, tablePhi;
  tableR.clear();
  tablePhi.clear();

  if (coeff_sim.size() < 8)
  {
    error() << "coeff_sim.size()<8"<<endmsg;
    return StatusCode::FAILURE;
  }
  const double r_a0 = coeff_sim.at(0);
  const double r_a1 = coeff_sim.at(1);
  const double r_a2 = coeff_sim.at(2);
  const double r_a3 = coeff_sim.at(3);
  const double phi_a0 = coeff_sim.at(4);
  const double phi_a1 = coeff_sim.at(5);
  const double phi_a2 = coeff_sim.at(6);
  const double phi_a3 = coeff_sim.at(7);
  double BLong = 0;
  //if (m_UseRandomBField && m_UseBFieldTestMap)
  //{
  //  BLong = m_RandomBFieldMinimum + number_range( 0, 100 )/100.0
  //    * (m_RandomBFieldMaximum-m_RandomBFieldMinimum);
  //  msg << MSG::INFO<<"Random B field for Hpd" << m_number
  //      << " set to "<<BLong<<" gauss"<<endmsg;
  //}
  //else
  BLong = m_LongitudinalBField;

  // Simulation from cathode->anode
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
    tableR[ r_cathode ]   = r_anode;
    tablePhi[ r_cathode ] = phi_anode;

  }

  m_demagMapR   -> initInterpolator ( tableR   );
  m_demagMapPhi -> initInterpolator ( tablePhi );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  fillHpdMagTable
//=========================================================================
StatusCode DeRichHPD::fillHpdMagTable( )
{
  const std::string paraLoc = "hpd"+boost::lexical_cast<std::string>(m_number)+"_rec";
  const std::vector<double> & coeff_rec = m_demagCond->paramVect<double>(paraLoc);

  // working data tables, used to initialise the interpolators
  std::map<double,double> tableR, tablePhi;
  tableR.clear();
  tablePhi.clear();

  if(coeff_rec.size() < 8)
  {
    error() << "coeff_rec.size()<8"<<endmsg;
    return StatusCode::FAILURE;
  }
  const double r_a0 = coeff_rec.at(0);
  const double r_a1 = coeff_rec.at(1);
  const double r_a2 = coeff_rec.at(2);
  const double r_a3 = coeff_rec.at(3);
  const double phi_a0 = coeff_rec.at(4);
  const double phi_a1 = coeff_rec.at(5);
  const double phi_a2 = coeff_rec.at(6);
  const double phi_a3 = coeff_rec.at(7);

  // Reconstruction from anode->cathode
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

    tableR[ r_anode ]   = r_cathode;
    tablePhi[ r_anode ] = phi_cathode;

  }

  m_magMapR   -> initInterpolator( tableR   );
  m_magMapPhi -> initInterpolator( tablePhi );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  magnification to cathode and transformation to Global coordinates
//=========================================================================
StatusCode DeRichHPD::magnifyToGlobal( Gaudi::XYZPoint& detectPoint,
                                       bool photoCathodeSide ) const
{
  const double rAnode = detectPoint.R();

  // chose method to use for the rCathode
  double rCathode =
    ( m_UseHpdMagDistortions ?
      // use magnetic distortion
      magnification_RtoR()->value( rAnode ) :

      // use "old" demagnification
      // To go from the cathode to the anode Ra = Rc*(-d0 + d1*Rc)
      // The minus sign in d0 is for the cross-focussing effect
      // To go from the anode to the cathode solve: d1*Rc^2 - d0*Rc - Ra = 0
      // The difference is that Ra is now positive.
      // Chose the solution with the minus sign
      (m_deMagFactor[0] - sqrt(gsl_pow_2( m_deMagFactor[0] ) -
                               4*m_deMagFactor[1]*rAnode))/(2*m_deMagFactor[1] ) );


  // check if this point could have come from the photoCathode
  if ( m_winInRsq < rCathode*rCathode ) return StatusCode::FAILURE;

  if ( !photoCathodeSide ) {
    // add "extra" radius for the refraction on the HPD window,
    // assuming 90 degrees angle
    rCathode  = rCathode + m_refactParams[3]*gsl_pow_3(rCathode)+
      m_refactParams[2]*gsl_pow_2(rCathode)+m_refactParams[1]*rCathode+
      m_refactParams[0];
  }

  double xWindow(0.0), yWindow(0.0);
  if ( m_UseHpdMagDistortions )
  {
    const double result_phi = magnification_RtoPhi()->value( rAnode );

    double anodePhi = std::atan2( detectPoint.Y(), detectPoint.X() );
    if ( detectPoint.Y() < 0 ) anodePhi += Gaudi::Units::twopi;

    double new_phi = anodePhi + result_phi + Gaudi::Units::pi;
    if ( new_phi > Gaudi::Units::twopi ) new_phi -= Gaudi::Units::twopi;

    xWindow = rCathode * cos(new_phi);
    yWindow = rCathode * sin(new_phi);
  }
  else
  {
    // the minus sign is for the cross-focussing
    const double scaleUp = ( rAnode>0 ? -rCathode/rAnode : 0 );

    xWindow = scaleUp*detectPoint.x();
    yWindow = scaleUp*detectPoint.y();
  }
  const double XsqPlusYsq = xWindow*xWindow+yWindow*yWindow;

  const double winRadiusSq = ( photoCathodeSide ? m_winInRsq :m_winOutRsq );
  if ( winRadiusSq < XsqPlusYsq ) return StatusCode::FAILURE;
  const double zWindow = sqrt(winRadiusSq - XsqPlusYsq);

  detectPoint = m_fromWindowToGlobal * Gaudi::XYZPoint(xWindow,yWindow,zWindow);

  return StatusCode::SUCCESS;
}

//***********************************************************************
double DeRichHPD::demag(const double r, const double B)
{
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
  const double a= a0 + a1*B+a2*B*B;
  const double b=b0 + b1*B + b2*B*B + b3*B*B*B;

  // rho calculated from r, a and b, second order, 0 constant term polynomial
  return (a*r + b*r*r);
}

//***********************************************************************
double DeRichHPD::mag(const double rho, const double B)
{
  // function mag returns the r coordinate given rho and B as arguments
  // coefficients of the fits for the dependence of the mag law
  // from the magnetic field
  const double c0=5.63057, c1=-1.12557e-2, c2=-4.65552e-4;
  const double d0=0.10377, d1=-1.052e-3, d2=1.995e-4, d3=-2.734e-6;

  // calculate c and d from the field
  const double c = c0 + c1*B+c2*B*B;
  const double d = d0 + d1*B + d2*B*B + d3*B*B*B;

  // r calculated from rho, c and d, second order, 0 constant term polynomial
  return (c*rho + d*rho*rho);
}

//***********************************************************************
double DeRichHPD::Delta_Phi(const double r, const double B)
{
  // It returns the rotation angle \Delta\phi [rad] as a function of the
  // radial entrance
  // window coordinate r [mm] and the axial magnetic flux density B [G].
  // first calculate the coefficients a, b, c from the
  // value of the axial magnetic flux density
  // these are called \Delta\phi_0, \Delta\phi_2, \Delta\phi_3 in the
  // referenced thesis at the beginning

  // calculate a
  const double a1 = 0.0255208;
  const double a2 = -4.78963E-5;
  const double a = a1*B + a2*B*B;

  // calculate b
  const double b1 = -6.93056E-6;
  const double b2 = -8.31286E-7;
  const double b3 = 1.23044E-8;
  const double b =  b1*B + b2*B*B + b3*B*B*B;

  // calculate c
  const double c1 = 3.38129E-8;
  const double c2 = 1.6186E-8;
  const double c3 = -2.24814E-10;
  const double c = c1*B + c2*B*B + c3*B*B*B;

  // this calculates and returns \Delta\phi from a, b, c and r
  return ( a + b*r*r + c*r*r*r ) ;
}

//=========================================================================
// Test for shadowing by the kapton shield of the given DeRichHPD
//=========================================================================
bool DeRichHPD::testKaptonShadowing( const Gaudi::XYZPoint&  pInPanel,
                                     const Gaudi::XYZVector& vInPanel ) const
{
  const Gaudi::XYZPoint  pInKapton( m_fromPanelToKapton * pInPanel );
  const Gaudi::XYZVector vInKapton( m_fromPanelToKapton * vInPanel );
  ISolid::Ticks kaptonTicks;
  return ( 0 != m_kaptonSolid->intersectionTicks( pInKapton,
                                                  vInKapton,
                                                  kaptonTicks ) );
}
