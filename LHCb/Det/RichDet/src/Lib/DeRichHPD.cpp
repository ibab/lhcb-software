//=============================================================================
/** @file DeRichHPD.cpp
 *
 * Implementation file for class : DeRichHPD
 *
 * $Id: DeRichHPD.cpp,v 1.5 2007-04-23 12:28:13 jonrob Exp $
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-09-19
 */
//=============================================================================

// Include files
//#include <time.h>

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

// DetDesc
#include "DetDesc/Condition.h"
#include "DetDesc/SolidBox.h"

// local
#include "RichDet/DeRichHPD.h"
#include "RichDet/DeRich.h"

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
DeRichHPD::DeRichHPD():
  m_deSiSensor    ( NULL ),
  m_pvWindow      ( NULL ),
  m_demagMapR     ( NULL ),
  m_demagMapPhi   ( NULL ),
  m_magMapR       ( NULL ),
  m_magMapPhi     ( NULL ),
  m_firstUpdate   ( true ),
  m_refactParams  ( 4, 0)
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
  const std::string::size_type pos = name().find("PDPanel");
  m_name = ( std::string::npos != pos ? name().substr(pos) : "DeRichHPD_NO_NAME" );

  // extract HPD number from detector element name
  const std::string::size_type pos2 = name().find(':');
  if ( std::string::npos == pos2 ) {
    msg << MSG::FATAL << "An HPD without a number!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_number = atoi( name().substr(pos2+1).c_str() );

  StatusCode sc = getParameters();
  if ( sc.isFailure() ) return sc;

  msg << MSG::DEBUG << "RichHpdPixelsize:" << m_pixelSize << " ActiveRadius:"
      << m_activeRadius << endreq;
  msg << MSG::DEBUG << "DeMagFactor(0): " << m_deMagFactor[0]
      << " DeMagFactor(1): " << m_deMagFactor[1] << endmsg;

  const IPVolume * pvHPDSMaster = geometry()->lvolume()->pvolume(0);
  const IPVolume * pvSilicon = pvHPDSMaster->lvolume()->pvolume("pvRichHPDSiDet");

  const ISolid* siliconSolid = pvSilicon->lvolume()->solid();
  msg << MSG::VERBOSE << "About to do a dynamic cast SolidBox "
      << siliconSolid->name() << endmsg;
  const SolidBox* siliconBox = dynamic_cast<const SolidBox*>(siliconSolid);
  if ( !siliconBox )
  {
    msg << MSG::FATAL << " Failed to get SolidBox for silicon" << endreq;
    return StatusCode::FAILURE;
  }

  // Get sizes of silicon box
  m_siliconHalfLengthX = siliconBox->xHalfLength();
  m_siliconHalfLengthY = siliconBox->yHalfLength();

  // get the pv and the solid for the HPD quartz window
  m_pvWindow  = pvHPDSMaster->lvolume()->pvolume("pvRichHPDQuartzWindow");
  const ISolid* windowSolid = m_pvWindow->lvolume()->solid();
  // get the inside radius of the window
  ISolid::Ticks windowTicks;
  const unsigned int windowTicksSize = 
    windowSolid->intersectionTicks( Gaudi::XYZPoint(0.0, 0.0, 0.0),
                                    Gaudi::XYZVector(0.0, 0.0, 1.0),
                                    windowTicks );
  if ( windowTicksSize != 2 ) 
  {
    msg << MSG::FATAL << "Problem getting window radius" << endmsg;
    return StatusCode::FAILURE;
  }

  // Get inner and out window radii
  const double winInR = windowTicks[0];
  m_winInRsq = winInR*winInR;
  const double winOutR = windowTicks[1];
  m_winOutRsq = winOutR*winOutR;
  msg << MSG::DEBUG << "winInR = " << winInR << " winOutR = " << winOutR << endmsg;

  // Transformation for HPD window to global ccord system
  m_fromWindowToGlobal = 
    geometry()->matrixInv() * pvHPDSMaster->matrixInv() * m_pvWindow->matrixInv();

  // get the pointer to the silicon sensor detector element
  m_deSiSensor = childIDetectorElements().front();

  if (m_UseHpdMagDistortions) 
  {

    //if(m_UseRandomBField) init_mm();
    msg << MSG::DEBUG<< "Current set is UseBFieldTestMap="<<m_UseBFieldTestMap
        << " LongitudinalBField="<<m_LongitudinalBField
      //<< " UseRandomBField="<<m_UseRandomBField
      //<< " within:"<<m_RandomBFieldMinimum<<"-"<<m_RandomBFieldMaximum
        << endmsg;

    m_demagCond = condition( "DemagParameters" );
    if ( m_demagCond ) updMgrSvc()->
                         registerCondition(this, m_demagCond.path(),
                                           &DeRichHPD::updateDemagProperties );
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
    MsgStream msg( msgSvc(), myName() );
    msg << MSG::ERROR << "Could not load DeRich1" << endmsg;
    return StatusCode::FAILURE;
  }

  m_pixelSize = deRich1->param<double>("RichHpdPixelXsize");
  m_activeRadius = deRich1->param<double>("RichHpdActiveInpRad");

  if (deRich1->exists("RefractHPDQrtzWin") )
    m_refactParams = deRich1->param<std::vector<double> >("RefractHPDQrtzWin");

  m_UseHpdMagDistortions= deRich1->param<int>("UseHpdMagDistortions");
  m_UseBFieldTestMap    = deRich1->param<int>("UseBFieldTestMap");
  m_LongitudinalBField  = deRich1->param<double>("LongitudinalBField");
  //m_UseRandomBField     = deRich1->param<int>("UseRandomBField");
  //m_RandomBFieldMinimum = deRich1->param<double>("RandomBFieldMinimum");
  //m_RandomBFieldMaximum = deRich1->param<double>("RandomBFieldMaximum");

  // load old demagnification factors
  SmartDataPtr<TabulatedProperty> HPDdeMag
    (dataSvc(),"/dd/Materials/RichMaterialTabProperties/HpdDemagnification");
  if (!HPDdeMag) 
  {
    MsgStream msg( msgSvc(), myName() );
    msg << MSG::FATAL << "Could not load HpdDemagnification" << endmsg;
    return StatusCode::FAILURE;
  }
  const TabulatedProperty::Table & DeMagTable = HPDdeMag->table();
  m_deMagFactor[0] = DeMagTable[0].second;
  m_deMagFactor[1] = DeMagTable[1].second;
 
  return StatusCode::SUCCESS;
}

//=================================================================================
// Demagnification Tables for simulation and reconstruction (Marco Musy 07/09/2006)
//=================================================================================
StatusCode DeRichHPD::updateDemagProperties() 
{

  if ( !m_firstUpdate ) {
    MsgStream msg ( msgSvc(), myName() );
    msg << MSG::INFO << "Updating properties for HPD:" << m_number <<endmsg;
    m_firstUpdate = false;
  }

  // delete the current interpolators
  cleanUpInterps();

  // Initialise the demagnifcation
  StatusCode sc = fillHpdDemagTable();
  if (!sc) {
    MsgStream msg ( msgSvc(), myName() );
    msg<<MSG::FATAL<< "Could not initialise demagnification table for HPD:" << m_number <<endmsg;
    return sc;
  }

  // Initialise the magnification
  sc = fillHpdMagTable();
  if (!sc) {
    MsgStream msg ( msgSvc(), myName() );
    msg<<MSG::FATAL<< "Could not initialise magnification table for HPD:" << m_number <<endmsg;
    return sc;
  }

  return sc;
}

//=========================================================================
StatusCode DeRichHPD::fillHpdDemagTable()
{
  const std::string paraLoc = "hpd"+boost::lexical_cast<std::string>(m_number)+"_sim";
  const std::vector<double> & coeff_sim = m_demagCond->paramVect<double>(paraLoc);

  SmartDataPtr<TabulatedProperty> dem(dataSvc(), XmlHpdDemagPath+"Sim_"+
                                      boost::lexical_cast<std::string>(m_number));
  if (!dem) {
    MsgStream msg ( msgSvc(), myName() );
    msg<<MSG::ERROR<<"Could not load "<<(XmlHpdDemagPath+"Sim_")<<m_number<<endmsg;
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
    MsgStream msg ( msgSvc(), myName() );
    msg<<MSG::ERROR<< "coeff_sim.size()<8"<<endmsg;
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

  m_demagMapR   = new Rich::TabulatedFunction1D( tableR );
  m_demagMapPhi = new Rich::TabulatedFunction1D( tablePhi );

  return StatusCode::SUCCESS;
}

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
    MsgStream msg ( msgSvc(), myName() );
    msg<<MSG::ERROR<< "coeff_rec.size()<8"<<endmsg;
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

  m_magMapR     = new Rich::TabulatedFunction1D( tableR );
  m_magMapPhi   = new Rich::TabulatedFunction1D( tablePhi );

  return StatusCode::SUCCESS;
}

//=========================================================================
//  magnification to cathode without mag field
//=========================================================================
StatusCode DeRichHPD::magnifyToGlobal_old( Gaudi::XYZPoint& detectPoint ) const
{
  const double inSiliconR = detectPoint.R();

  // Now calculate the radius at the cathode.
  // To go from the cathode to the anode Ra = Rc*(-d0 + d1*Rc)
  // The minus sign in d0 is for the cross-focussing effect
  // To go from the anode to the cathode solve: d1*Rc^2 - d0*Rc - Ra = 0
  // The difference is that Ra is now positive.
  // Chose the solution with the minus sign
  const double rInWindow = ( m_deMagFactor[0] -
                             sqrt(gsl_pow_2( m_deMagFactor[0] ) -
                                  4*m_deMagFactor[1]*inSiliconR))/(2*m_deMagFactor[1]);

  // for the refraction on the HPD window
  const double rOutWin = rInWindow + m_refactParams[3]*gsl_pow_3(rInWindow)+
    m_refactParams[2]*gsl_pow_2(rInWindow)+m_refactParams[1]*rInWindow+
    m_refactParams[0];

  // the minus sign is for the cross-focussing
  const double scaleUp = ( inSiliconR>0 ? -rOutWin/inSiliconR : 0 );

  const double inWindowX = scaleUp*detectPoint.x();
  const double inWindowY = scaleUp*detectPoint.y();
  const double XsqPlusYsq = inWindowX*inWindowX+inWindowY*inWindowY;

  if ( m_winInRsq < XsqPlusYsq ) return StatusCode::FAILURE;

  const double inWindowZ = sqrt(m_winOutRsq - XsqPlusYsq);

  detectPoint = m_fromWindowToGlobal * Gaudi::XYZPoint(inWindowX,inWindowY,inWindowZ);

  return StatusCode::SUCCESS;
}

//=========================================================================
//  magnification to cathode with mag field
//=========================================================================
StatusCode DeRichHPD::magnifyToGlobal_new( Gaudi::XYZPoint& detectPoint ) const
{

  const double inSiliconR = detectPoint.R();

  const double result_r   = magnification_RtoR()->value( inSiliconR );
  const double result_phi = magnification_RtoPhi()->value( inSiliconR );

  double anodePhi = std::atan2( detectPoint.Y(), detectPoint.X() );
  if ( detectPoint.Y() < 0 ) anodePhi += Gaudi::Units::twopi;
  
  double new_phi = anodePhi + result_phi + Gaudi::Units::pi;
  if ( new_phi > Gaudi::Units::twopi ) new_phi -= Gaudi::Units::twopi;

  const double inWindowX = result_r * cos(new_phi);
  const double inWindowY = result_r * sin(new_phi);
  const double XsqPlusYsq = inWindowX*inWindowX+inWindowY*inWindowY;

  if ( m_winInRsq < XsqPlusYsq ) return StatusCode::FAILURE;
  const double inWindowZ = sqrt(m_winInRsq - XsqPlusYsq);

  detectPoint = m_fromWindowToGlobal * Gaudi::XYZPoint(inWindowX,inWindowY,inWindowZ);

  return StatusCode::SUCCESS;
}

//***********************************************************************
double DeRichHPD::demag(const double r, const double B) {

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
double DeRichHPD::mag(const double rho, const double B) {

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
double DeRichHPD::Delta_Phi(const double r, const double B) {

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

// CRJ Comment out by default since having a 57 element array in each HPD just for this is 
// wasteful, and this random stuff should go anyway IMHO ...
/*
//===========================================================================
int DeRichHPD::number_mm( void ) {
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
//===========================================================================
int DeRichHPD::number_range( int from, int to ) {
  int power, number;
  if ( ( to = to - from + 1 ) <= 1 )  return from;
  for ( power = 2; power < to; power <<= 1 );
  while ( ( number = number_mm() & ( power - 1 ) ) >= to );
  return from + number;
}
//===========================================================================
void DeRichHPD::init_mm( ) {
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
*/
