
//=============================================================================
/** @file DeRichHPD.cpp
 *
 * Implementation file for class : DeRichHPD
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-09-19
 */
//=============================================================================

// STL
#include <sstream>

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

// DetDesc
#include "DetDesc/Condition.h"
#include "DetDesc/SolidBox.h"

// local
#include "RichDet/DeRichHPD.h"
#include "RichDet/DeRich.h"
#include "RichDet/DeRichSystem.h"

// VDT
#include "vdt/atan2.h"
#include "vdt/sincos.h"

//=============================================================================

const CLID CLID_DERichHPD = 12015;  // User defined

// anon namespace
namespace
{
  static const std::string& XmlHpdDemagPath =
    "/dd/Materials/RichMaterialTabProperties/RichHpdDemag";
  static const int totbins = 50; // do not change !!!
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichHPD::DeRichHPD ( const std::string & name ) : DeRichPD ( name  )
{
  for ( unsigned int field = 0; field < 2; ++field )
  {
    m_demagMapR.emplace_back   ( new Rich::TabulatedFunction1D() );
    m_demagMapPhi.emplace_back ( new Rich::TabulatedFunction1D() );
    m_magMapR.emplace_back     ( new Rich::TabulatedFunction1D() );
    m_magMapPhi.emplace_back   ( new Rich::TabulatedFunction1D() );
  }
}

//=============================================================================
// Destructor
//=============================================================================
DeRichHPD::~DeRichHPD()
{
  if ( m_magFieldSvc ) { m_magFieldSvc->release(); m_magFieldSvc = nullptr; }
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
  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "Initialize " << myName() << endmsg;

  // store the name of the HPD, without the /dd/Structure part
  const std::string::size_type pos = name().find("HPD:");
  setMyName( std::string::npos != pos ? name().substr(pos) : "DeRichHPD_NO_NAME" );

  // extract HPD number from detector element name
  const std::string::size_type pos2 = name().find(":");
  if ( std::string::npos == pos2 )
  {
    msg << MSG::FATAL << "An HPD without a number!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_number = atoi( name().substr(pos2+1).c_str() );

  StatusCode sc = getParameters();
  if ( sc.isFailure() ) return sc;

  // get the pointer to the silicon sensor detector element
  m_deSiSensor = ( !childIDetectorElements().empty() ?
                   childIDetectorElements().front() : nullptr );
  if ( !m_deSiSensor )
  {
    fatal() << "Cannot find SiSensor detector element" << endmsg;
    return StatusCode::FAILURE;
  }

  // register updates for the locally cached geometry information
  // Probably recalculates more than strictly needed, but easier and safer to
  // do it all, as it should be a rare thing, so CPU is not a problem.
  updMgrSvc()->registerCondition( this, geometry(),
                                  &DeRichHPD::updateGeometry );
  updMgrSvc()->registerCondition( this, m_deSiSensor->geometry(),
                                  &DeRichHPD::updateGeometry );

  m_demagConds.clear();
  if ( hasCondition("DemagParametersFieldNegative") &&
       hasCondition("DemagParametersFieldPositive") )
  {
    m_demagConds.push_back( condition( "DemagParametersFieldNegative" ));
    updMgrSvc()->registerCondition( this, m_demagConds[0].path(),
                                    &DeRichHPD::updateDemagProperties );

    m_demagConds.push_back( condition( "DemagParametersFieldPositive" ));
    updMgrSvc()->registerCondition( this, m_demagConds[1].path(),
                                    &DeRichHPD::updateDemagProperties );
  }
  else
  {
    if ( hasCondition( "DemagParameters" ) )
    {
      m_demagConds.push_back( condition("DemagParameters") );
      m_demagConds.push_back( condition("DemagParameters") );
      updMgrSvc()->registerCondition( this, m_demagConds[0].path(),
                                      &DeRichHPD::updateDemagProperties );
    }
    else
    {
      m_demagConds.push_back( SmartRef<Condition>() );
      m_demagConds.push_back( SmartRef<Condition>() );
    }
  }

  // Update HPD QE values whenever DeRichSystem updates
  updMgrSvc()->registerCondition( this, deRichSys(), &DeRichHPD::initHpdQuantumEff );

  // Trigger first update
  sc = updMgrSvc()->update(this);
  if ( sc.isFailure() ) { fatal() << "UMS updates failed" << endmsg; }

  // finally return
  return sc;
}

//=========================================================================
// Load the Magnetic field service when required
//=========================================================================
void DeRichHPD::loadMagSvc() const
{
  if ( !m_magFieldSvc )
  {
    // get the magnetic field service
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    if ( !svcLocator )
    {
      throw GaudiException( "ISvcLocator* points to nullptr!",
                            "DeRichHPD" , StatusCode::FAILURE );
    }
    const auto scMag = svcLocator->service("MagneticFieldSvc",m_magFieldSvc);
    if ( scMag.isFailure() )
    {
      throw GaudiException( "Could not locate MagneticFieldSvc",
                            "DeRichHPD" , StatusCode::FAILURE );
    }
  }
}

//=========================================================================
// Initialise the HPD quantum eff function
//=========================================================================
StatusCode DeRichHPD::initHpdQuantumEff()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Updating Q.E. for HPD:" << m_number << endmsg;

  // get quantum efficiency tabulated property from LHCBCOND if available
  if ( deRichSys()->exists( "HpdQuantumEffCommonLoc" ) )  // use hardware ID to locate QE
  {
    // convert copy number to smartID
    const auto id = deRichSys()->richSmartID(Rich::DAQ::HPDCopyNumber(m_number));
    const auto qePath   = deRichSys()->param<std::string>("HpdQuantumEffCommonLoc");
    const std::string hID( deRichSys()->hardwareID(id) );
    SmartDataPtr<TabulatedProperty> hpdQuantumEffTabProp( dataSvc(), qePath+hID );
    if ( !hpdQuantumEffTabProp )
    {
      fatal() << "Could not load HPD's quantum efficiency TabulatedProperty for " << myName()
              << " from " << qePath+hID << endmsg;
      return StatusCode::FAILURE;
    }
    m_pdQuantumEffFunc.reset( new Rich::TabulatedProperty1D( hpdQuantumEffTabProp ) );
  }
  else // use copy number to locate QE
  {
    if ( !hasCondition("QuantumEffTable") )
    {
      // use common QE for all HPDs
      SmartDataPtr<DeRich> deRich1( dataSvc(), DeRichLocations::Rich1 );
      m_pdQuantumEffFunc = deRich1->nominalPDQuantumEff();
    }
    else
    {
      // get QE from the CondDB at the location stored in condition QuantumEffTable
      SmartRef<Condition> hpdQuantumEffCond = condition("QuantumEffTable");
      SmartDataPtr<TabulatedProperty> hpdQuantumEffTabProp( dataSvc(),
                                                            hpdQuantumEffCond.path() );
      if ( !hpdQuantumEffTabProp )
      {
        fatal() << "Could not load HPD's quantum efficiency TabulatedProperty for " << myName()
                << " from " << hpdQuantumEffCond.path() << endmsg;
        return StatusCode::FAILURE;
      }
      m_pdQuantumEffFunc.reset( new Rich::TabulatedProperty1D( hpdQuantumEffTabProp ) );
    }
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  getParameters
//=========================================================================
StatusCode DeRichHPD::getParameters()
{
  SmartDataPtr<DetectorElement> deRich1( dataSvc(), DeRichLocations::Rich1 );
  if ( !deRich1 )
  {
    error() << "Could not load DeRich1" << endmsg;
    return StatusCode::FAILURE;
  }

  m_pixelSize    = deRich1->param<double>("RichHpdPixelXsize");
  m_activeRadius = deRich1->param<double>("RichHpdActiveInpRad");

  if (deRich1->exists("RefractHPDQrtzWin") )
    m_refactParams = deRich1->param<std::vector<double> >("RefractHPDQrtzWin");
  else
  {
    warning() << "No parameters for refraction on HPD window! "
              << "Corrections will not work properly!" << endmsg;
  }

  m_UseHpdMagDistortions = ( 0 != deRich1->param<int>("UseHpdMagDistortions") );
  m_UseBFieldTestMap     = ( 0 != deRich1->param<int>("UseBFieldTestMap") );
  m_LongitudinalBField   = deRich1->param<double>("LongitudinalBField");
  //m_UseRandomBField     = deRich1->param<int>("UseRandomBField");
  //m_RandomBFieldMinimum = deRich1->param<double>("RandomBFieldMinimum");
  //m_RandomBFieldMaximum = deRich1->param<double>("RandomBFieldMaximum");

  // load old demagnification factors
  SmartDataPtr<TabulatedProperty> HPDdeMag
    ( dataSvc(), "/dd/Materials/RichMaterialTabProperties/HpdDemagnification" );
  if ( !HPDdeMag )
  {
    fatal() << "Could not load HpdDemagnification" << endmsg;
    return StatusCode::FAILURE;
  }
  const auto & DeMagTable = HPDdeMag->table();
  m_deMagFactor[0] = DeMagTable[0].second;
  m_deMagFactor[1] = DeMagTable[1].second;

  return StatusCode::SUCCESS;
}


//=========================================================================
// update the localy cached transforms
//=========================================================================
StatusCode DeRichHPD::updateGeometry()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Updating geometry transformations for HPD:" << m_number <<endmsg;

  // find the subMaster volume, normally the first physical volume
  const auto * pvHPDSMaster = geometry()->lvolume()->pvolume(0);
  if ( pvHPDSMaster->name().find("HPDSMaster") == std::string::npos )
  {
    fatal() << "Cannot find HPDSMaster volume : " << pvHPDSMaster->name() << endmsg;
    return StatusCode::FAILURE;
  }

  // find pvRichHPDSiDet volume
  const auto * pvSilicon = pvHPDSMaster->lvolume()->pvolume("pvRichHPDSiDet");
  if ( !pvSilicon ) // multiple HPD volumes
  {
    pvSilicon = pvHPDSMaster->lvolume()->pvolume(10);
    if ( !pvSilicon || pvSilicon->name().find("pvRichHPDSiDet") == std::string::npos )
    {
      fatal() << "Cannot find pvRichHPDSiDet volume ";
      if ( pvSilicon != nullptr ) fatal() << pvSilicon->name();
      fatal() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // pointer to kapton
  m_kaptonSolid =
    pvHPDSMaster->lvolume()->pvolume("pvRichHPDKaptonShield")->lvolume()->solid();

  const auto * siliconSolid = pvSilicon->lvolume()->solid();
  const auto * siliconBox = dynamic_cast<const SolidBox*>(siliconSolid);
  if ( !siliconBox )
  {
    fatal() << " Failed to get SolidBox for silicon" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get sizes of silicon box
  m_siliconHalfLengthX = siliconBox->xHalfLength();
  m_siliconHalfLengthY = siliconBox->yHalfLength();

  // get the pv and the solid for the HPD quartz window
  m_pvWindow    = pvHPDSMaster->lvolume()->pvolume("pvRichHPDQuartzWindow");
  m_windowSolid = m_pvWindow->lvolume()->solid();
  // find intersections with the window
  ISolid::Ticks windowTicks;
  const auto windowTicksSize =
    m_windowSolid->intersectionTicks( Gaudi::XYZPoint (0.0, 0.0, 0.0),
                                      Gaudi::XYZVector(0.0, 0.0, 1.0),
                                      windowTicks );
  if ( windowTicksSize != 2 )
  {
    fatal() << "Problem getting window radius" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get inner and outer window radii
  m_winInR    = windowTicks[0];
  m_winInRsq  = m_winInR*m_winInR;
  m_winOutR   = windowTicks[1];
  m_winOutRsq = m_winOutR*m_winOutR;

  // get kapton
  const auto * pvKapton = pvHPDSMaster->lvolume()->pvolume("pvRichHPDKaptonShield");

  // Transformation from HPD window to global coord system
  m_fromWindowToGlobal = ( geometry()->toGlobalMatrix() * 
                           pvHPDSMaster->matrixInv()    * 
                           m_pvWindow->matrixInv()      );
  // Transformation from HPD panel to HPD window coord system
  m_fromPanelToWindow = ( m_pvWindow->matrix()    * 
                          pvHPDSMaster->matrix()  * 
                          geometry()->ownMatrix() );
  // Transformation from HPD panel to kapton coord system
  m_fromPanelToKapton = ( pvKapton->matrix()      * 
                          pvHPDSMaster->matrix()  * 
                          geometry()->ownMatrix() );
  // Transformation for HPD window to HPD coord system
  m_fromWindowToHPD = ( pvHPDSMaster->matrixInv() * 
                        m_pvWindow->matrixInv() );
  // Transformation for HPD to HPD-Panel coord system
  m_fromHPDToPanel = geometry()->ownMatrix().Inverse();

  // cache the position of the inside window centre in the panel coordinate system
  m_windowInsideCentreMother = ( geometry()->ownMatrix().Inverse() *
                                 pvHPDSMaster->matrixInv()         *
                                 m_pvWindow->toMother(Gaudi::XYZPoint(0,0,m_winInR)) );

  // cache the centre of curvature of the HPD window outer surface
  // in the global frame
  m_windowCoCglobal = m_fromWindowToGlobal * Gaudi::XYZPoint(0,0,0);

  // from silicon sensor to HPD including misalignment
  m_SiSensorToHPDMatrix = m_deSiSensor->geometry()->ownMatrix().Inverse();

  // With new MDMS V1 updates that depend on Image radius, update
  // must be triggered if alignment of HPD and HPD sensor changes
  return DeRichHPD::updateDemagProperties();
}

//=================================================================================
// Demagnification Tables for simulation and reconstruction (Marco Musy 07/09/2006)
//=================================================================================
StatusCode DeRichHPD::updateDemagProperties()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Updating Demagnification properties for HPD:" << m_number << endmsg;

  StatusCode sc = StatusCode::SUCCESS;
  for ( unsigned int field = 0; field<2; ++field )
  {
    // Initialise the demagnifcation
    sc = fillHpdDemagTable(field);
    if ( sc.isFailure() )
    {
      fatal() << "Could not initialise demagnification table for HPD:"
              << m_number <<endmsg;
      return sc;
    }

    // Initialise the magnification
    sc = fillHpdMagTable(field);
    if ( sc.isFailure() )
    {
      fatal() << "Could not initialise magnification table for HPD:"
              << m_number << endmsg;
      return sc;
    }
  }

  return sc;
}

//=========================================================================
//  fillHpdDemagTable
//=========================================================================
StatusCode DeRichHPD::fillHpdDemagTable(const unsigned int field)
{

  std::ostringstream paraLoc, detLoc;
  paraLoc << "hpd" << m_number << "_sim";
  const auto & coeff_sim = m_demagConds[field]->paramVect<double>(paraLoc.str());
  detLoc << XmlHpdDemagPath << "Sim_" << m_number;
  SmartDataPtr<TabulatedProperty> dem ( dataSvc(), detLoc.str() );

  if ( !dem )
  {
    error() << "Could not load "<<(XmlHpdDemagPath+"Sim_")<<m_number<<endmsg;
    return StatusCode::FAILURE;
  }
  TabulatedProperty::Table & simTable = dem->table();
  simTable.clear();
  simTable.reserve(totbins+1);

  // working data tables, used to initialise the interpolators
  std::map<double,double> tableR, tablePhi;

  // Read the MDMS parameters
  if ( coeff_sim.size() < 8 )
  {
    error() << "coeff_sim.size()<8"<<endmsg;
    return StatusCode::FAILURE;
  }
  const double& r_a0   = coeff_sim.at(0);
  const double& r_a1   = coeff_sim.at(1);
  const double& r_a2   = coeff_sim.at(2);
  const double& r_a3   = coeff_sim.at(3);
  const double& phi_a0 = coeff_sim.at(4);
  const double& phi_a1 = coeff_sim.at(5);
  const double& phi_a2 = coeff_sim.at(6);
  const double& phi_a3 = coeff_sim.at(7);
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
  for ( int i = 0; i < totbins+1; ++i )
  {

    double r_anode(0), phi_anode(0);
    const double r_cathode = m_activeRadius/totbins * (double)i;

    if ( m_UseBFieldTestMap )
    {

      //calculate r_anode and deltaphi from hpd test data
      r_anode   =     demag( r_cathode, BLong );
      phi_anode = Delta_Phi( r_cathode, BLong );

    }
    else
    { //evaluate distorsions from condDB parameters

      // cache for speed
      const double rCathode2 = r_cathode * r_cathode ;
      const double rCathode3 = rCathode2 * r_cathode ;

      r_anode   = r_a0
        + ( r_a1 * r_cathode )
        + ( r_a2 * rCathode2 )
        + ( r_a3 * rCathode3 );
      phi_anode = phi_a0
        + ( phi_a1 * r_cathode )
        + ( phi_a2 * rCathode2 )
        + ( phi_a3 * rCathode3 );
    }

    simTable.push_back( TabulatedProperty::Entry( r_anode, phi_anode ) );
    tableR  [ r_cathode ] = r_anode;
    tablePhi[ r_cathode ] = phi_anode;

  }

  m_demagMapR[field]   -> initInterpolator ( tableR   );
  m_demagMapPhi[field] -> initInterpolator ( tablePhi );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  fillHpdMagTable
//=========================================================================
StatusCode DeRichHPD::fillHpdMagTable( const unsigned int field )
{

  // MDMS version
  m_MDMS_version[field] = ( m_demagConds[field]->exists("version") ?
                            m_demagConds[field]->param<int>("version") : 0 );
  if ( msgLevel(MSG::DEBUG) )
    debug() << " -> Field " << field << " MDMS version = " << m_MDMS_version[field] << endmsg;
  if ( m_MDMS_version[field] < 0 || m_MDMS_version[field] > 2 )
  {
    error() << "Unknown MDMS version " << m_MDMS_version[field] << endmsg;
    return StatusCode::FAILURE;
  }
  
  // Load the MDMS parameters
  std::ostringstream paraLoc;  
  paraLoc << "hpd" << m_number << "_rec";
  const std::vector<double>& coeff_rec = m_demagConds[field]->paramVect<double>(paraLoc.str());

  // Expected size of the MDMS parameters vector
  const unsigned int nMDMSParams = ( 0 == m_MDMS_version[field] ? 8  :
                                     1 == m_MDMS_version[field] ? 9  : 
                                     2 == m_MDMS_version[field] ? 12 : 
                                     8 );

  // Check the size of the MDMS parameters
  if ( coeff_rec.size() < nMDMSParams )
  {
    error() << "MDMS Version " << m_MDMS_version[field] << " requires " 
            << nMDMSParams << " parameters. Found only " << coeff_rec.size() 
            << "..." << endmsg;
    return StatusCode::FAILURE;
  }

  // working data tables, used to initialise the interpolators
  std::map<double,double> tableR, tablePhi;

  // Read the MDMS parameters
  const double r_a0   = coeff_rec.at(0);
  double       r_a1   = coeff_rec.at(1);
  const double r_a2   = coeff_rec.at(2);
  const double r_a3   = coeff_rec.at(3);
  const double phi_a0 = coeff_rec.at(4);
  const double phi_a1 = coeff_rec.at(5);
  const double phi_a2 = coeff_rec.at(6);
  const double phi_a3 = coeff_rec.at(7);

  // If MDMS V1, scale the r1 values
  if ( 1 == m_MDMS_version[field] )
  {

    // HPD image radius from data
    const Condition * siAlignCond = ( m_deSiSensor->geometry() ? 
                                      m_deSiSensor->geometry()->alignmentCondition() : nullptr );
    if ( !siAlignCond )
    {
      error() << "Failed to load SiSensor alignment Condition !" << endmsg;
      return StatusCode::FAILURE;
    }
    const double R_data = ( siAlignCond->exists("ImageRadius") ? 
                            siAlignCond->param<double>("ImageRadius") : -999 );

    // reference HPD image size from MDMS
    const double R_MDMS = coeff_rec.at(8);

    // Scale r1
    const double Rscale = ( R_data > 0.1 && R_MDMS > 0.1 ? R_data / R_MDMS : 1.0 );
    if ( msgLevel(MSG::DEBUG) )
      debug() << "R_data = " << R_data << " R_MDMS = " << R_MDMS 
              << " Scale = " << Rscale 
              << endmsg;
    r_a1 *= Rscale;

  }

  // Reconstruction from anode->cathode
  for ( int i = 0; i < totbins+1; ++i )
  {

    const double r_anode =
      std::min(m_siliconHalfLengthX,m_siliconHalfLengthY)/totbins * (double)i;
    double r_cathode(0), phi_cathode(0);

    if ( m_UseBFieldTestMap )
    {
      //calculate r_cathode and deltaphi from hpd test data
      r_cathode   =        mag( r_anode,   0 );
      phi_cathode = -Delta_Phi( r_cathode, 0 );

    }
    else
    { //evaluate distorsions from condDB parameters

      // cache square and cube for speed
      const double rAnode2 = r_anode * r_anode ;
      const double rAnode3 = rAnode2 * r_anode ;

      r_cathode = r_a0
        + ( r_a1 * r_anode )
        + ( r_a2 * rAnode2 )
        + ( r_a3 * rAnode3 );

      phi_cathode = phi_a0
        + ( phi_a1 * r_anode )
        + ( phi_a2 * rAnode2 )
        + ( phi_a3 * rAnode3 );
    }

    tableR  [ r_anode ] = r_cathode;
    tablePhi[ r_anode ] = phi_cathode;

  } // bins

  m_magMapR[field]   -> initInterpolator( tableR   );
  m_magMapPhi[field] -> initInterpolator( tablePhi );

  if ( UNLIKELY( 2 == m_MDMS_version[field] ) )
  {
    m_MDMSRotCentre = Gaudi::XYZVector( coeff_rec[8], coeff_rec[9], 0.0 );
    m_magnificationCoef1 = coeff_rec[10];
    m_magnificationCoef2 = coeff_rec[11];
  }
  else
  {
    m_MDMSRotCentre = Gaudi::XYZPoint( 0.0, 0.0, 0.0 );
    m_magnificationCoef1 = 0.0;
    m_magnificationCoef2 = 0.0;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  magnification to cathode and transformation to Global coordinates
//  with the magnet ON
//=========================================================================
StatusCode DeRichHPD::magnifyToGlobalMagnetON( Gaudi::XYZPoint& detectPoint,
                                               const bool photoCathodeSide ) const
{
  const auto field = ( magSvc()->isDown() ? 0 : 1 );

  // Only versions 0, 1 or 2 possible
  detectPoint = ( 2 > m_MDMS_version[field] ?
                  m_SiSensorToHPDMatrix * detectPoint  :
                  detectPoint - m_MDMSRotCentre );
  detectPoint.SetZ(0.0);
  
  const auto rAnode = detectPoint.R();

  double rCathode(0);
  if ( UNLIKELY( 2 == m_MDMS_version[field] ) )
  {
    detectPoint = m_SiSensorToHPDMatrix * detectPoint;
    detectPoint.SetZ(0.0);
    const auto new_rAnode = detectPoint.R();
    rCathode = ( (m_magnificationCoef1*new_rAnode) + 
                 (m_magnificationCoef2*new_rAnode*new_rAnode) );
  }
  else
  {
    rCathode = magnification_RtoR(field)->value( rAnode );
  }

  // check if this point could have come from the photoCathode
  if ( m_winInR < rCathode ) return StatusCode::FAILURE;

  // add "extra" radius for the refraction on the HPD window,
  // assuming 90 degrees angle
  if ( !photoCathodeSide ) rCathode += extraRadiusForRefraction(rCathode);
  
  // calculate angle phi
  auto anodePhi = vdt::fast_atan2( detectPoint.Y(), detectPoint.X() );
  if ( detectPoint.Y() < 0 ) anodePhi += Gaudi::Units::twopi;

  const auto result_phi = magnification_RtoPhi(field)->value( rAnode );

  auto new_phi = anodePhi + result_phi + Gaudi::Units::pi;
  if ( new_phi > Gaudi::Units::twopi ) new_phi -= Gaudi::Units::twopi;

  const auto& winRadius = ( photoCathodeSide ? m_winInR : m_winOutR );
  if ( winRadius < rCathode ) return StatusCode::FAILURE;

  double vdtsin(0), vdtcos(0);
  vdt::fast_sincos(new_phi,vdtsin,vdtcos);
  const auto xWindow = rCathode * vdtcos;
  const auto yWindow = rCathode * vdtsin;

  const auto& winRadiusSq = ( photoCathodeSide ? m_winInRsq : m_winOutRsq );
  detectPoint = ( m_fromWindowToGlobal *
                  Gaudi::XYZPoint( xWindow,
                                   yWindow, 
                                   std::sqrt(winRadiusSq-(rCathode*rCathode)) ) );
  
  //std::cout << detectPoint << std::endl;

  return StatusCode::SUCCESS;
}
//=========================================================================
//  magnification to cathode and transformation to Global coordinates
//  with the magnet OFF
//=========================================================================
StatusCode DeRichHPD::magnifyToGlobalMagnetOFF( Gaudi::XYZPoint& detectPoint,
                                                const bool photoCathodeSide ) const
{
  detectPoint = m_SiSensorToHPDMatrix * detectPoint;
  detectPoint.SetZ(0.0);
  const double rAnode = detectPoint.R();

  // To go from the cathode to the anode Ra = Rc*(-d0 + d1*Rc)
  // The minus sign in d0 is for the cross-focussing effect
  // To go from the anode to the cathode solve: d1*Rc^2 - d0*Rc - Ra = 0
  // The difference is that Ra is now positive.
  // Chose the solution with the minus sign
  double rCathode =
    ( m_deMagFactor[1] > 1e-6 ?
      ((m_deMagFactor[0]-std::sqrt(std::pow(m_deMagFactor[0],2)-4*m_deMagFactor[1]*rAnode))/
       (2*m_deMagFactor[1])) :
      rAnode/m_deMagFactor[0] );

  // check if this point could have come from the photoCathode
  if ( m_winInRsq < rCathode*rCathode ) return StatusCode::FAILURE;

  // add "extra" radius for the refraction on the HPD window,
  // assuming 90 degrees angle
  if ( !photoCathodeSide ) rCathode += extraRadiusForRefraction(rCathode);

  // the minus sign is for the cross-focussing
  const double scaleUp = ( rAnode>0 ? -rCathode/rAnode : 0 );

  const double xWindow = scaleUp*detectPoint.x();
  const double yWindow = scaleUp*detectPoint.y();
  const double XsqPlusYsq = xWindow*xWindow+yWindow*yWindow;

  const double winRadiusSq = ( photoCathodeSide ? m_winInRsq : m_winOutRsq );
  if ( winRadiusSq < XsqPlusYsq ) return StatusCode::FAILURE;
  const double zWindow = std::sqrt(winRadiusSq - XsqPlusYsq);

  detectPoint = m_fromWindowToGlobal * Gaudi::XYZPoint(xWindow,yWindow,zWindow);

  return StatusCode::SUCCESS;
}

//***********************************************************************
double DeRichHPD::demag(const double r, const double B)
{
  // the coefficients of the polynomial fits
  constexpr double a0=0.1771, a1=1.818e-4, a2=2.2197e-5;
  constexpr double b0=-4.713E-4, b1=8.613E-6, b2=-1.2794E-6, b3=+1.3596E-8;

  const auto BB  = B*B;
  const auto BBB = BB*B; 

  // a and b calculated from the field
  const auto a = a0 + a1*B+a2*BB;
  const auto b = b0 + b1*B + b2*BB + b3*BBB;

  // rho calculated from r, a and b, second order, 0 constant term polynomial
  return ( a + b*r ) * r;
}

//***********************************************************************
double DeRichHPD::mag( const double rho, const double B )
{
  constexpr double c0=5.63057, c1=-1.12557e-2, c2=-4.65552e-4;
  constexpr double d0=0.10377, d1=-1.052e-3, d2=1.995e-4, d3=-2.734e-6;

  const auto BB  = B*B;
  const auto BBB = BB*B; 

  // calculate c and d from the field
  const auto c = c0 + c1*B+c2*BB;
  const auto d = d0 + d1*B + d2*BB + d3*BBB;

  // r calculated from rho, c and d, second order, 0 constant term polynomial
  return ( c + d*rho ) * rho;
}

//***********************************************************************
double DeRichHPD::Delta_Phi(const double r, const double B)
{
  const auto BB  = B*B;
  const auto BBB = BB*B; 

  // calculate a
  constexpr double a1 = 0.0255208;
  constexpr double a2 = -4.78963E-5;
  const auto a = a1*B + a2*BB;

  // calculate b
  constexpr double b1 = -6.93056E-6;
  constexpr double b2 = -8.31286E-7;
  constexpr double b3 = 1.23044E-8;
  const auto b =  b1*B + b2*BB + b3*BBB;

  // calculate c
  constexpr double c1 = 3.38129E-8;
  constexpr double c2 = 1.6186E-8;
  constexpr double c3 = -2.24814E-10;
  const auto c = c1*B + c2*BB + c3*BBB;

  // this calculates and returns \Delta\phi from a, b, c and r
  return ( a + ( ( b + c*r ) * r * r ) ) ;
}

//=========================================================================
// Test for shadowing by the kapton shield of the given DeRichHPD
//=========================================================================
bool DeRichHPD::testKaptonShadowing( const Gaudi::XYZPoint&  pInPanel,
                                     const Gaudi::XYZVector& vInPanel ) const
{
  const auto pInKapton = m_fromPanelToKapton * pInPanel;
  const auto vInKapton = m_fromPanelToKapton * vInPanel;
  ISolid::Ticks kaptonTicks;
  return ( 0 != m_kaptonSolid->intersectionTicks( pInKapton,
                                                  vInKapton,
                                                  kaptonTicks ) );
}

//=========================================================================
// Converts a pair to a point in global coordinates
//=========================================================================
StatusCode DeRichHPD::detectionPoint ( const double fracPixelCol,
                                       const double fracPixelRow,
                                       Gaudi::XYZPoint& detectPoint,
                                       const bool photoCathodeSide ) const
{
  if ( fracPixelCol < 0.0 || fracPixelRow < 0.0 )
  {
    error() << "Negative pixel coordinate " << fracPixelCol << ","
            << fracPixelRow << endmsg;
    return StatusCode::FAILURE;
  }

  detectPoint = Gaudi::XYZPoint( fracPixelCol*m_pixelSize - m_siliconHalfLengthX,
                                 m_siliconHalfLengthY - fracPixelRow*m_pixelSize,
                                 0.0 );
  const StatusCode sc =
    ( m_UseHpdMagDistortions || fabs(magSvc()->signedRelativeCurrent()) > 0.5 ) ?
    magnifyToGlobalMagnetON  ( detectPoint, photoCathodeSide ) :
    magnifyToGlobalMagnetOFF ( detectPoint, photoCathodeSide ) ;
  detectPoint = geometry()->toLocal(detectPoint);
  return sc;
}

//=========================================================================
// Converts a RichSmartID to a point in global coordinates.
//=========================================================================
StatusCode DeRichHPD::detectionPoint ( const LHCb::RichSmartID smartID,
                                       Gaudi::XYZPoint& detectPoint,
                                       bool photoCathodeSide ) const
{
  detectPoint = pointOnSilicon(smartID);
  return ( ( m_UseHpdMagDistortions || fabs(magSvc()->signedRelativeCurrent()) > 0.5 ) ?
           magnifyToGlobalMagnetON  ( detectPoint, photoCathodeSide ) :
           magnifyToGlobalMagnetOFF ( detectPoint, photoCathodeSide ) );
}

//=========================================================================
// Intersect a given direction, from a given point, with the outer surface 
// of the HPD window in the global LHCb frame
//=========================================================================
bool DeRichHPD::intersectEntryWindow ( const Gaudi::XYZPoint& position,
                                       const Gaudi::XYZVector& direction,
                                       Gaudi::XYZPoint& intersection ) const
{
  bool OK = true;
  // find intersection point
  // for line sphere intersection look at http://www.realtimerendering.com/int/
  const auto a = direction.Mag2();
  if ( UNLIKELY( 0 == a ) ) { OK = false; }
  else
  {
    const auto delta = position - m_windowCoCglobal;
    const auto     b = 2.0 * direction.Dot( delta );
    const auto     c = delta.Mag2() - m_winOutRsq;
    const auto discr = b*b - 4.0*a*c;
    if ( UNLIKELY( discr < 0 ) ) { OK = false; }
    else
    {
      // note this is optimised for the 'outside to in' solution...
      const auto dist = -0.5 * ( std::sqrt(discr) + b ) / a;
      // set intersection point
      intersection = position + ( dist * direction );
    }
  }
  return OK;
}

//=========================================================================
