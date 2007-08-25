// $Id: PatTTMagnetTool.cpp,v 1.2 2007-08-25 14:27:37 jonrob Exp $

// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// Mathlib
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

#include "Event/State.h"
// from TrackInterfacces
#include "TrackInterfaces/ITrackExtrapolator.h"

// local
#include "PatTableForFunction.h"
#include "PatTTMagnetTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatTTMagnetTool
//
// 2006-09-25 : Mariusz Witek
//-----------------------------------------------------------------------------

using namespace Tf;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatTTMagnetTool );

// Standard Constructor
PatTTMagnetTool::PatTTMagnetTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent)
  : GaudiTool( type, name, parent ),
    m_magFieldSvc(0)

{
  declareInterface<PatTTMagnetTool>(this);

}

//=========================================================================
//  setMidTT
//=========================================================================
StatusCode PatTTMagnetTool::initialize ( ) {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;

  // retrieve pointer to magnetic field service
  m_magFieldSvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true ); 

  // == Get the ST Detector Element
  m_STDet = getDet<DeSTDetector>( DeSTDetLocation::TT );

  m_zCenterTT = 0.;

  m_zLayers.clear();
    // warning layers not in order of increasing z
  for ( std::vector<DeSTLayer*>::const_iterator itL = m_STDet->layers().begin();
          m_STDet->layers().end() != itL; ++itL ) {
     double zlay = (*(*itL)->sectors().begin())->globalCentre().Z(); 
     m_zLayers.push_back(zlay); 
     m_zCenterTT += zlay;
  }   

  m_zCenterTT /= m_zLayers.size();
  std::sort(m_zLayers.begin(),m_zLayers.end());

  prepareBdlTables();
  prepareDeflectionTables();

  m_zMidField = zBdlMiddle(0.05, 0.0, 0.0);

  m_noField = false;
  // check whether B=0
  f_bdl(0.,0.,400.*Gaudi::Units::mm, m_zCenterTT);
  double  tbdl = m_BdlTrack;
  if(fabs(tbdl)<10e-4) {
    m_noField = true;
  }

  if(m_noField) {
    info() << " No B field detected." << endreq;    
  }

  m_bdlIntegral_NoB = 0.1;
  m_zBdlMiddle_NoB  = 1900.;
  m_zMidField_NoB   = 1900.;
  m_averageDist2mom_NoB = 0.00004;  

  return StatusCode::SUCCESS;
}

//=========================================================================
// z middle of TT
//=========================================================================
double PatTTMagnetTool::zMidTT() {
  return m_zCenterTT;
}

//=========================================================================
// z middle of B field betweenz=0 and zMidTT
//=========================================================================
double PatTTMagnetTool::zMidField() {
  if( m_noField ) { 
    return m_zMidField_NoB;
  }
  return m_zMidField;
}

//=========================================================================
// averageDist2mom
//=========================================================================
double PatTTMagnetTool::averageDist2mom() {
  if( m_noField ) { 
    return m_averageDist2mom_NoB;
  }
  return m_dist2mom;
}

//=========================================================================
// prepareBdlTables
//=========================================================================
void PatTTMagnetTool::prepareBdlTables() {

  info() << "Start generation of VeloTT Bdl LUTs" << endreq;
  // prepare table with Bdl integrations
  // Bdl integral depends on 3 track parameters
  //  slopeY     - y slope of the track
  //  zOrigin    - z of track intersection with z axis (in YZ projection)
  //  zVeloEnd   - z of the track at which slopeY is given
  //
  //                      slopeY    zOrigin    zVeloEnd

  m_lutBdl       = tool<PatTableForFunction>("PatTableForFunction/table1",this);
  m_lutZHalfBdl  = tool<PatTableForFunction>("PatTableForFunction/table2",this);

  m_lutBdl->addVariable(30, -0.3, 0.3);
  m_lutBdl->addVariable(10, -250.*Gaudi::Units::mm, 250.*Gaudi::Units::mm);
  m_lutBdl->addVariable(10,    0.*Gaudi::Units::mm, 800.*Gaudi::Units::mm);
  m_lutBdl->prepareTable();

  m_lutZHalfBdl->addVariable(30, -0.3, 0.3); 
  m_lutZHalfBdl->addVariable(10, -250.*Gaudi::Units::mm, 250.*Gaudi::Units::mm); 
  m_lutZHalfBdl->addVariable(10,    0.*Gaudi::Units::mm, 800.*Gaudi::Units::mm); 
  m_lutZHalfBdl->prepareTable(); 

  m_lutVar.clear();
  m_lutVar.push_back(0.0);
  m_lutVar.push_back(0.0);
  m_lutVar.push_back(0.0);
  m_lutBdl->resetIndexVector();
  m_lutZHalfBdl->resetIndexVector();
  int iover = 0;
  while(!iover) {
    m_lutBdl->getVariableVector(m_lutVar);
    double slopeY   = m_lutVar[0];
    double zOrigin  = m_lutVar[1];
    double zEndVelo = m_lutVar[2];
    f_bdl(slopeY, zOrigin, zEndVelo, m_zCenterTT);
    m_lutBdl->fillTable(m_BdlTrack);
    m_lutZHalfBdl->fillTable(m_zHalfBdlTrack);
    iover = m_lutBdl->incrementIndexVector();
    iover = m_lutZHalfBdl->incrementIndexVector();
  }

  info() << "Generation of VeloTT Bdl LUTs finished" << endreq;
  return;

}

//=========================================================================
// prepareDeflectionTables
//=========================================================================
void PatTTMagnetTool::prepareDeflectionTables() {

  info() << "Start generation of VeloTT deflection LUTs" << endreq;

  // prepare deflection tables

  m_lutDxLay    = tool<PatTableForFunction>("PatTableForFunction/table3",this);
  m_lutDxToMom  = tool<PatTableForFunction>("PatTableForFunction/table4",this);

  // Retrieve extrapolators
  ITrackExtrapolator* my_linear = tool<ITrackExtrapolator>( "TrackLinearExtrapolator" );
  ITrackExtrapolator* my_parabolic = tool<ITrackExtrapolator>( "TrackHerabExtrapolator" );

  // tmp state
  LHCb::State tmpState;
  double xBeg = 0.;
  double yBeg = 0.;
  double zBeg = 0.;
  double qpBeg = 1./(10.*Gaudi::Units::GeV);
  double dxdzBeg = 0.0;
  double dydzBeg = 0.0;
  tmpState.setState(  xBeg,  yBeg, zBeg, dxdzBeg, dydzBeg, qpBeg );
  // set dummy covariance matrix
  Gaudi::TrackSymMatrix cov = Gaudi::TrackSymMatrix();
  cov(0,0) = 0.1;
  cov(1,1) = 0.1;
  cov(2,2) = 0.1;
  cov(3,3) = 0.1;
  cov(4,4) = 0.1;
  tmpState.setCovariance( cov );

  // determine normalization factors for deflections in different TT layers
  // wrt center of TT

  m_lutDxLay->addVariable(3, 0., 3.); // index of TT layer 1 to 4
  m_lutDxLay->addVariable(30, 0.0, 0.3); // slope y
  m_lutDxLay->prepareTable();

  m_lutVar.clear();
  m_lutVar.push_back(0.0);
  m_lutVar.push_back(0.0);

  int iover = 0;
  while(!iover) {
    m_lutDxLay->getVariableVector(m_lutVar);
    int idLay       = int(m_lutVar[0]+0.000001);
    double dydzBeg  = m_lutVar[1];
    double dxdzBeg  = 0;
    double zLay = m_zLayers[idLay];
    
    xBeg = 0.;
    yBeg = 0.;
    zBeg = 0.;
    debug() << m_lutVar[0] << " " << m_lutVar[1] << " "  
            << " idlay " << idLay << " " << m_zLayers.size() << " zlay" 
            << zLay << " " << endreq;

    tmpState.setState(  xBeg,  yBeg, zBeg, dxdzBeg, dydzBeg, qpBeg );
    
    LHCb::State stalin_mid = tmpState;
    LHCb::State stapar_mid = tmpState;
    // extrapolate state to middle of TT
    StatusCode sc1 =    my_linear->propagate( stalin_mid, m_zCenterTT );
    StatusCode sc2 = my_parabolic->propagate( stapar_mid, m_zCenterTT );
    if( sc1.isFailure() || sc2.isFailure() ) {
      warning() << "Extrapolation failed " << endreq;
    } 

    double dx_mid = 0.;
    double dx_lay = 0.;
    double ratio = 0.;
    LHCb::State stalin_lay = tmpState;
    LHCb::State stapar_lay = tmpState;

    StatusCode sc3 =    my_linear->propagate( stalin_lay, zLay );
    StatusCode sc4 = my_parabolic->propagate( stapar_lay, zLay );
    if( sc3.isFailure() || sc4.isFailure()) {
      warning() << "Extrapolation failed " << endreq;
    } else {
      dx_mid = stapar_mid.x()-stalin_mid.x();
      dx_lay = stapar_lay.x()-stalin_lay.x();
      if(fabs(dx_mid)>1.e-8) ratio = dx_mid/dx_lay; 
    }
    m_lutDxLay->fillTable(ratio);
    iover = m_lutDxLay->incrementIndexVector();
  }

  // distance to momentum table (depends on y slope only)
  m_lutDxToMom->addVariable(30, 0.0, 0.3); // slope y
  m_lutDxToMom->prepareTable();

  iover = 0;
  m_lutVar.clear();
  m_lutVar.push_back(0.0);

  while(!iover) {
    m_lutDxToMom->getVariableVector(m_lutVar);
    double dxdzBeg  = 0.0;
    double dydzBeg  = m_lutVar[0];
    xBeg = 0.;
    yBeg = 0.;
    zBeg = 0.;

    tmpState.setState(  xBeg,  yBeg, zBeg, dxdzBeg, dydzBeg, qpBeg );

    LHCb::State stalin_mid = tmpState;
    LHCb::State stapar_mid = tmpState;
    // extrapolate state to middle of TT
    StatusCode sc1 =    my_linear->propagate( stalin_mid, m_zCenterTT );
    StatusCode sc2 = my_parabolic->propagate( stapar_mid, m_zCenterTT );

    double dx2mom = 0.;
    if( sc1.isFailure() || sc2.isFailure() ) {
      warning() << "Extrapolation failed " << endreq;
    } else {
      double dx = stapar_mid.x()-stalin_mid.x();
      if(fabs(dx)>1e-8) dx2mom = qpBeg / dx;    
    }
    m_lutDxToMom->fillTable(dx2mom);
    iover = m_lutDxToMom->incrementIndexVector();
  }
  
  // determine distToMomentum parameter
  m_lutVar.clear();
  m_lutVar.push_back(0.05);
  m_dist2mom = m_lutDxToMom->getValueFromTable(m_lutVar);

  info() << "Generation of VeloTT deflection LUTs finished" << endreq;

  return;
}
//=========================================================================
//  Destructor
//=========================================================================
PatTTMagnetTool::~PatTTMagnetTool() {  
};

//****************************************************************************
double PatTTMagnetTool::bdlIntegral(double ySlopeVelo, double zOrigin, double zVelo) {

    if( m_noField ) { 
      return m_bdlIntegral_NoB;
    }
    m_lutVar.clear();
    m_lutVar.push_back(ySlopeVelo);
    m_lutVar.push_back(zOrigin);
    m_lutVar.push_back(zVelo);
    return  m_lutBdl->getInterpolatedValueFromTable(m_lutVar);
}
//****************************************************************************
double PatTTMagnetTool::zBdlMiddle(double ySlopeVelo, double zOrigin, double zVelo) {

    if( m_noField ) { 
      return m_zBdlMiddle_NoB;
    }
    m_lutVar.clear();
    m_lutVar.push_back(ySlopeVelo);
    m_lutVar.push_back(zOrigin);
    m_lutVar.push_back(zVelo);
    return m_lutZHalfBdl->getInterpolatedValueFromTable(m_lutVar);
}

//****************************************************************************
double PatTTMagnetTool::dist2mom(double ySlope) {

  if( m_noField ) { 
    return m_averageDist2mom_NoB;
  }
  m_lutVar.clear();
  m_lutVar.push_back(ySlope);
  return m_lutDxToMom->getValueFromTable(m_lutVar);

}

//****************************************************************************
void PatTTMagnetTool::dxNormFactorsTT(double ySlope, std::vector<double>& nfact) {

  nfact.clear();
  if( m_noField ) { 
    for(int i=0; i<4; i++) {
      nfact.push_back(1.0);
    }    
    return;
  }
  m_lutVar.clear();
  m_lutVar.push_back(0.);
  m_lutVar.push_back(fabs(ySlope));
  for(int i=0; i<4; i++) {
    m_lutVar[0]=i;
    double nf = m_lutDxLay->getValueFromTable(m_lutVar);
    nfact.push_back(nf);
  }
}


//****************************************************************************
void PatTTMagnetTool::f_bdl( double slopeY, double zOrigin,
                             double zStart, double zStop){

    m_BdlTrack=0.0;
    m_zHalfBdlTrack=0.0;

    if(zStart>zStop) return;

    double Bdl=0.0;
    double zHalfBdl=0.0;

   // vectors to calculate z of half Bdl
    m_bdlTmp.clear();
    m_zTmp.clear(); 

    // prepare m_zBdlHalf;
    Gaudi::XYZPoint  aPoint(0.,0.,0.);
    Gaudi::XYZVector bField;

    int np    = 500;
    double dz = (zStop - zStart)/np;
    double dy = dz*slopeY;

    aPoint.SetX( 0.0 );

    double z = zStart+dz/2.;
    double y = slopeY*(zStart-zOrigin);

    double bdl = 0.;

    while( z<zStop ) {
      
      aPoint.SetY( y );
      aPoint.SetZ( z );

      m_magFieldSvc->fieldVector( aPoint, bField ).ignore();
      bdl += dy*bField.z() - dz*bField.y();
      if(z>100.*Gaudi::Units::cm) {
        m_bdlTmp.push_back(bdl);
        m_zTmp.push_back(z);
      }
      z += dz;
      y += dy;
    }

    Bdl=bdl;
    double bdlhalf = fabs(Bdl)/2.;

    for(unsigned int i=5; i<m_bdlTmp.size()-5; i++) {
      if(fabs(m_bdlTmp[i])>bdlhalf) {
        double zrat = (Bdl/2.-m_bdlTmp[i-1])/(m_bdlTmp[i]-m_bdlTmp[i-1]);
        zHalfBdl = m_zTmp[i-1]+dz*zrat;
        break;
      }
    } 

    m_BdlTrack = Bdl;
    m_zHalfBdlTrack = zHalfBdl;

 }

