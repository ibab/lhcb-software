// Include files

// from Gaudi
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// Mathlib
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

#include "Event/State.h"
// from TrackInterfacces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// local
#include "PrTableForFunction.h"
#include "PrUTMagnetTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrUTMagnetTool
//
// 2006-09-25 : Mariusz Witek
//-----------------------------------------------------------------------------


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PrUTMagnetTool )

// Standard Constructor
PrUTMagnetTool::PrUTMagnetTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent)
  : GaudiTool( type, name, parent )
  , m_magFieldSvc(0)
  , m_lutBdl(0)
  , m_lutZHalfBdl(0)
  , m_lutDxLay(0)
  , m_lutDxToMom(0)
  , m_STDet(0)
{
  declareInterface<PrUTMagnetTool>(this);
}

//=========================================================================
//  setMidUT
//=========================================================================
StatusCode PrUTMagnetTool::initialize ( ) {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if (msgLevel( MSG::DEBUG )) debug() << "==> Initialize" << endmsg;

  // set parameters for no field run
  m_bdlIntegral_NoB = 0.1;
  m_zBdlMiddle_NoB  = 1900.;
  m_zMidField_NoB   = 1900.;
  m_averageDist2mom_NoB = 0.00004;  

  // retrieve pointer to magnetic field service
  m_magFieldSvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true ); 

  // == Get the ST Detector Element
  m_STDet = getDet<DeSTDetector>( DeSTDetLocation::UT );

  
  // subscribe to the updatemanagersvc with a dependency on the magnetic field svc
  IUpdateManagerSvc* m_updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
  ILHCbMagnetSvc* m_fieldSvc = svc<ILHCbMagnetSvc>("MagneticFieldSvc", true);
  m_updMgrSvc->registerCondition( this,m_fieldSvc,&PrUTMagnetTool::updateField) ;

  // initialize with the current conditions
  return m_updMgrSvc->update(this) ;
}

//=========================================================================
// Callback function for field updates
//=========================================================================
StatusCode PrUTMagnetTool::updateField() 
{
  prepareBdlTables();
  prepareDeflectionTables();

  m_noField = false;
  // check whether B=0
  f_bdl(0.,0.,400.*Gaudi::Units::mm, m_zCenterUT);
  float  tbdl = m_BdlTrack;
  if(fabs(tbdl)<10e-4) {
    m_noField = true;
  }

  m_zMidField = zBdlMiddle(0.05, 0.0, 0.0);

  if(m_noField) {
    info() << " No B field detected." << endmsg;    
  }

  return StatusCode::SUCCESS ;
}


//=========================================================================
// z middle of UT
//=========================================================================
float PrUTMagnetTool::zMidUT() {
  return m_zCenterUT;
}

//=========================================================================
// z middle of B field betweenz=0 and zMidUT
//=========================================================================
float PrUTMagnetTool::zMidField() {
  if( m_noField ) { 
    return m_zMidField_NoB;
  }
  return m_zMidField;
}

//=========================================================================
// averageDist2mom
//=========================================================================
float PrUTMagnetTool::averageDist2mom() {
  if( m_noField ) { 
    return m_averageDist2mom_NoB;
  }
  return m_dist2mom;
}

//=========================================================================
// prepareBdlTables
//=========================================================================
void PrUTMagnetTool::prepareBdlTables() {

  info() << "Start generation of VeloUT Bdl LUTs" << endmsg;
  // prepare table with Bdl integrations
  // Bdl integral depends on 3 track parameters
  //  slopeY     - y slope of the track
  //  zOrigin    - z of track intersection with z axis (in YZ projection)
  //  zVeloEnd   - z of the track at which slopeY is given
  //
  //                      slopeY    zOrigin    zVeloEnd

  // WDH: I do not understand why these tables are stored as
  // tools. what is wrong with just owning the objects?
  // m_zCenterUT is a normalization plane which should be close to middle of UT.
  // It is used to normalize dx deflection at different UT layers.
  // No need to update with small UT movement up to +- 5 cm. 

  m_zCenterUT = 2484.6;
  float zCenterUT = 0.;
  
  m_zLayers.clear();
  for ( std::vector<DeSTLayer*>::const_iterator itL = m_STDet->layers().begin();
          m_STDet->layers().end() != itL; ++itL ) {
     float zlay = (*(*itL)->sectors().begin())->globalCentre().Z(); 
     m_zLayers.push_back(zlay); 
     zCenterUT += zlay;
  }   

  zCenterUT /= m_zLayers.size();
  if (fabs( m_zCenterUT - zCenterUT ) > 50. ) {
    warning() << "Calculated center of UT station far away from nominal value: " 
              << zCenterUT << " wrt nominal " << m_zCenterUT << endmsg;
    warning() << " Calculated value taken: " << zCenterUT << endmsg;    
    m_zCenterUT = zCenterUT;
  }
  // warning layers not in order of increasing z
  std::sort(m_zLayers.begin(),m_zLayers.end());

  m_lutBdl       = tool<PrTableForFunction>("PrTableForFunction/table1",this);
  m_lutZHalfBdl  = tool<PrTableForFunction>("PrTableForFunction/table2",this);
  m_lutBdl->clear() ;
  m_lutZHalfBdl->clear() ;

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
    float slopeY   = m_lutVar[0];
    float zOrigin  = m_lutVar[1];
    float zEndVelo = m_lutVar[2];
    f_bdl(slopeY, zOrigin, zEndVelo, m_zCenterUT);
    m_lutBdl->fillTable(m_BdlTrack);
    m_lutZHalfBdl->fillTable(m_zHalfBdlTrack);
    iover = m_lutBdl->incrementIndexVector();
    iover = m_lutZHalfBdl->incrementIndexVector();
  }

  info() << "Generation of VeloUT Bdl LUTs finished" << endmsg;
  return;

}

//=========================================================================
// prepareDeflectionTables
//=========================================================================
void PrUTMagnetTool::prepareDeflectionTables() {

  info() << "Start generation of VeloUT deflection LUTs" << endmsg;
  const bool isDebug = msgLevel( MSG::DEBUG );

  // prepare deflection tables

  m_lutDxLay    = tool<PrTableForFunction>("PrTableForFunction/table3",this);
  m_lutDxToMom  = tool<PrTableForFunction>("PrTableForFunction/table4",this);
  m_lutDxLay->clear() ;
  m_lutDxToMom->clear() ;

  // Retrieve extrapolators
  ITrackExtrapolator* my_linear = tool<ITrackExtrapolator>( "TrackLinearExtrapolator" );
  ITrackExtrapolator* my_parabolic = tool<ITrackExtrapolator>( "TrackRungeKuttaExtrapolator" );

  // tmp state
  LHCb::State tmpState;
  float xBeg = 0.;
  float yBeg = 0.;
  float zBeg = 0.;
  float qpBeg = 1./(10.*Gaudi::Units::GeV);
  float dxdzBeg = 0.0;
  float dydzBeg = 0.0;
  tmpState.setState(  xBeg,  yBeg, zBeg, dxdzBeg, dydzBeg, qpBeg );
  // set dummy covariance matrix
  Gaudi::TrackSymMatrix cov = Gaudi::TrackSymMatrix();
  cov(0,0) = 0.1;
  cov(1,1) = 0.1;
  cov(2,2) = 0.1;
  cov(3,3) = 0.1;
  cov(4,4) = 0.1;
  tmpState.setCovariance( cov );

  // determine normalization factors for deflections in different UT layers
  // wrt center of UT

  m_lutDxLay->addVariable(3, 0., 3.); // index of UT layer 1 to 4
  m_lutDxLay->addVariable(30, 0.0, 0.3); // slope y
  m_lutDxLay->prepareTable();

  m_lutVar.clear();
  m_lutVar.push_back(0.0);
  m_lutVar.push_back(0.0);

  int iover = 0;
  while(!iover) {
    m_lutDxLay->getVariableVector(m_lutVar);
    int idLay       = int(m_lutVar[0]+0.000001);
    dydzBeg  = m_lutVar[1];
    dxdzBeg  = 0.0;
    float zLay = m_zLayers[idLay];
    
    xBeg = 0.;
    yBeg = 0.;
    zBeg = 0.;
    if( isDebug ) {
      debug() << m_lutVar[0] << " " << m_lutVar[1] << " "  
              << " idlay " << idLay << " " << m_zLayers.size() << " zlay" 
              << zLay << " " << endmsg;
    }
    
    tmpState.setState(  xBeg,  yBeg, zBeg, dxdzBeg, dydzBeg, qpBeg );
    
    LHCb::State stalin_mid = tmpState;
    LHCb::State stapar_mid = tmpState;
    // extrapolate state to middle of UT
    StatusCode sc1 =    my_linear->propagate( stalin_mid, m_zCenterUT );
    StatusCode sc2 = my_parabolic->propagate( stapar_mid, m_zCenterUT );
    if( sc1.isFailure() || sc2.isFailure() ) {
      Warning( "Extrapolation failed ", StatusCode::SUCCESS, 0 ).ignore();
    } 

    float dx_mid = 0.;
    float dx_lay = 0.;
    float ratio = 0.;
    LHCb::State stalin_lay = tmpState;
    LHCb::State stapar_lay = tmpState;

    StatusCode sc3 =    my_linear->propagate( stalin_lay, zLay );
    StatusCode sc4 = my_parabolic->propagate( stapar_lay, zLay );
    if( sc3.isFailure() || sc4.isFailure()) {
      Warning( "Extrapolation failed ", StatusCode::SUCCESS, 0 ).ignore();
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
    dxdzBeg = 0.0;
    dydzBeg = m_lutVar[0];
    xBeg = 0.;
    yBeg = 0.;
    zBeg = 0.;

    tmpState.setState(  xBeg,  yBeg, zBeg, dxdzBeg, dydzBeg, qpBeg );

    LHCb::State stalin_mid = tmpState;
    LHCb::State stapar_mid = tmpState;
    // extrapolate state to middle of UT
    StatusCode sc1 =    my_linear->propagate( stalin_mid, m_zCenterUT );
    StatusCode sc2 = my_parabolic->propagate( stapar_mid, m_zCenterUT );

    float dx2mom = 0.;
    if( sc1.isFailure() || sc2.isFailure() ) {
      Warning( "Extrapolation failed ", StatusCode::SUCCESS, 0 ).ignore();
    } else {
      float dx = stapar_mid.x()-stalin_mid.x();
      if(fabs(dx)>1e-8) dx2mom = qpBeg / dx;    
    }
    m_lutDxToMom->fillTable(dx2mom);
    iover = m_lutDxToMom->incrementIndexVector();
  }
  
  // determine distToMomentum parameter
  m_lutVar.clear();
  m_lutVar.push_back(0.05);
  m_dist2mom = m_lutDxToMom->getValueFromTable(m_lutVar);

  info() << "Generation of VeloUT deflection LUTs finished" << endmsg;

  return;
}
//=========================================================================
//  Destructor
//=========================================================================
PrUTMagnetTool::~PrUTMagnetTool() {}

//****************************************************************************
float PrUTMagnetTool::bdlIntegral(float ySlopeVelo, float zOrigin, float zVelo) {

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
float PrUTMagnetTool::zBdlMiddle(float ySlopeVelo, float zOrigin, float zVelo) {

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
float PrUTMagnetTool::dist2mom(float ySlope) {

  if( m_noField ) { 
    return m_averageDist2mom_NoB;
  }
  m_lutVar.clear();
  m_lutVar.push_back(ySlope);
  return m_lutDxToMom->getValueFromTable(m_lutVar);

}

//****************************************************************************
void PrUTMagnetTool::dxNormFactorsUT(float ySlope, std::vector<float>& nfact) {

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
    float nf = m_lutDxLay->getValueFromTable(m_lutVar);
    nfact.push_back(nf);
  }
}


//****************************************************************************
void PrUTMagnetTool::f_bdl( float slopeY, float zOrigin,
                             float zStart, float zStop){

    m_BdlTrack=0.0;
    m_zHalfBdlTrack=0.0;

    if(zStart>zStop) return;

    float Bdl=0.0;
    float zHalfBdl=0.0;

   // vectors to calculate z of half Bdl
    m_bdlTmp.clear();
    m_zTmp.clear(); 

    // prepare m_zBdlHalf;
    Gaudi::XYZPoint  aPoint(0.,0.,0.);
    Gaudi::XYZVector bField;

    int np    = 500;
    float dz = (zStop - zStart)/np;
    float dy = dz*slopeY;

    aPoint.SetX( 0.0 );

    float z = zStart+dz/2.;
    float y = slopeY*(zStart-zOrigin);

    float bdl = 0.;

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
    float bdlhalf = fabs(Bdl)/2.;

    for(unsigned int i=5; i<m_bdlTmp.size()-5; i++) {
      if(fabs(m_bdlTmp[i])>bdlhalf) {
        float zrat = (Bdl/2.-m_bdlTmp[i-1])/(m_bdlTmp[i]-m_bdlTmp[i-1]);
        zHalfBdl = m_zTmp[i-1]+dz*zrat;
        break;
      }
    } 

    m_BdlTrack = Bdl;
    m_zHalfBdlTrack = zHalfBdl;

 }

