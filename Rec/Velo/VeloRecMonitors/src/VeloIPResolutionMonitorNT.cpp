// Include files
#include "VeloIPResolutionMonitorNT.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <string>
#include <sstream>

#include "Event/State.h"
#include "Event/Track.h"

#include "Event/Node.h"
#include "Event/Measurement.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/VeloRMeasurement.h"

#include "VeloDet/DeVeloPhiType.h"
#include "VeloDet/DeVeloRType.h"
#include "DetDesc/Material.h"

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#include <functional>
using std::bind;
using namespace std::placeholders;
#else
#include <boost/lambda/bind.hpp>
using namespace boost::lambda;
#endif

using namespace LHCb;
using namespace Gaudi::Utils;
using namespace Gaudi::Units;
using namespace std ;

#ifdef _WIN32 
#pragma warning ( disable : 4244 ) // Conversion of double to float, done in Root 
#endif 

//-----------------------------------------------------------------------------
// Implementation file for class : VeloIPResolutionMonitorNT
//
// 2009-05-15 : Michael Thomas Alexander
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo
{
  DECLARE_ALGORITHM_FACTORY( VeloIPResolutionMonitorNT )
}


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloIPResolutionMonitorNT::VeloIPResolutionMonitorNT( const string& name,
                                                        ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{

  // set input locations
  declareProperty("TrackLocation", m_trackLocation = TrackLocation::Default ) ;
  declareProperty("VertexLocation", m_vertexLocation = RecVertexLocation::Primary );

  // set whether to do comparison to MC
  declareProperty("WithMC",m_withMC=false);

  // Set whether to refit PVs without the track for which IP is being calculated
  declareProperty("RefitPVs", m_refitPVs = true );
  
}
//=============================================================================
// Destructor
//=============================================================================
Velo::VeloIPResolutionMonitorNT::~VeloIPResolutionMonitorNT() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitorNT::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // Set histo dir
  setHistoTopDir( "Velo/" );

  // Get vertexer if required
  if( m_refitPVs ){ 
    m_pvtool = tool<IPVOfflineTool>( "PVOfflineTool" );
    info() << "Will refit PVs excluding each track in turn" << endmsg ;
  }
  
  // get the track extrapolator used in calculating the IPs
  m_trackExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator","Extrapolator",this);

  // get the material locator
  m_materialLocator = tool<IMaterialLocator>("DetailedMaterialLocator","MaterialLocator",this) ;

  // get hit expectation tools
  m_TTExpectTool = tool<IHitExpectation>( "TTHitExpectation");
  m_VeloExpectTool = tool<IVeloExpectation>( "VeloExpectation");

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitorNT::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  counter( "Events Analysed" )++;
      
  // Get PVs
  const RecVertices* pvs = getIfExists<RecVertices>( m_vertexLocation );  
  if( NULL == pvs ){
    string counterName = string("No data at ") + m_vertexLocation ;
    counter( counterName )++;
    if ( msgLevel(MSG::DEBUG) )
      debug() << "No data at " << m_vertexLocation << endmsg;
    return StatusCode::SUCCESS;
  }

  // select only events with 1 reconstructed PV
  if( pvs->size() != 1 ) return StatusCode::SUCCESS;

  // get the tracks
  Tracks* tracks = getIfExists<Tracks>(m_trackLocation);
  if( NULL == tracks )
  {
    string counterName = string("No tracks at ") + m_trackLocation ;
    counter( counterName )++;
    return StatusCode::SUCCESS;
  }
  
  counter("Events Selected")++;

  m_pv = *(pvs->begin()) ;
  const LHCb::RecVertex* pv_noRefit = m_pv ;
    
  if( !m_pv->isPrimary() ) return StatusCode::SUCCESS ;

  counter("PVs Analysed")++;
  
  // Get tracks from current PV & loop
  Track::ConstVector PVtracks;

  // count number of tracks making this pv that are reconstructed in the velo 
  unsigned int nVeloTracks(0);
  for ( SmartRefVector< Track >::const_iterator tr = m_pv->tracks().begin(); 
        tr != m_pv->tracks().end() ; tr++ ){
    PVtracks.push_back( &(**tr) );
    if( (*tr)->type()==Track::Velo || (*tr)->type()==Track::Upstream || (*tr)->type()==Track::Long ){
      nVeloTracks += 1;
    }
  }
    
  // can't refit a PV with only one track!
  if( m_refitPVs && PVtracks.size() < 3 ) return StatusCode::SUCCESS ;
  counter("PVs Selected")++;
        
  // loop over tracks
  for( Tracks::const_iterator tr = tracks->begin(); tr != tracks->end(); ++tr ){
    
    if( (*tr)->type() != Track::Velo && (*tr)->type() != Track::Upstream && (*tr)->type() != Track::Long ) continue;      
    
    m_track = *tr;     
    
    // refit PV removing current track
    bool isInPV = ( find( PVtracks.begin(), PVtracks.end(), m_track ) != PVtracks.end() ) ;
    
    if( m_refitPVs && isInPV ){
      
      RecVertex* newVertex = new RecVertex( *m_pv ) ;
      vector< const Track* > trackToRemove( 1, m_track ) ;
      StatusCode scPVfit = 
        m_pvtool->reDoSinglePV( Gaudi::XYZPoint( newVertex->position() ), trackToRemove, *newVertex ) ;
      if( scPVfit.isFailure() ){
        delete newVertex ;
        continue ;
      }
      else
        m_pv = newVertex ;
    }             
    
    double ip3d, ip3dsigma, ipx, ipxsigma, ipy, ipysigma ;
    State POCAtoPVstate, stateAtPVZ ;
    StatusCode scCalcIPs = calculateIPs( m_pv, m_track, ip3d, ip3dsigma, ipx, ipxsigma, ipy, ipysigma, 
                                         stateAtPVZ, POCAtoPVstate );
    if( scCalcIPs.isFailure() ) continue ;

    counter("Tracks selected")++;
    
    double inversePT = 1./(m_track->pt()/GeV);
    
    Tuple tuple = nTuple( "IPResolutionsTuple" );
    
    tuple->column( "isInPV", isInPV );
    tuple->column( "TrackType", m_track->type() );
    tuple->column( "IsTrackForward", m_track->flag() != Track::Backward ) ;
    tuple->column( "IPRes3D", ip3d );
    tuple->column( "IPRes3Dsigma", ip3dsigma );
    tuple->column( "IPRes_X", ipx );
    tuple->column( "IPRes_Xsigma", ipxsigma );
    tuple->column( "IPRes_Y", ipy );
    tuple->column( "IPRes_Ysigma", ipysigma );
    tuple->column( "InversePT", inversePT );
    tuple->column( "P", m_track->p() );
    tuple->column( "Eta", m_track->pseudoRapidity() );
    tuple->column( "Phi", m_track->phi() );
    tuple->column( "TrackTx", m_track->slopes().x() );
    tuple->column( "TrackTy", m_track->slopes().y() );
    tuple->column( "TrackCharge", m_track->charge() );
    tuple->column( "TrackChi2", m_track->chi2() );
    tuple->column( "TrackNDOF", m_track->nDoF() );
    tuple->column( "PVNTracks", m_pv->tracks().size() );
    tuple->column( "PVNVeloTracks", nVeloTracks );
    tuple->column( "PVChi2", m_pv->chi2() );
    tuple->column( "PVNDOF", m_pv->nDoF() );
    tuple->column( "PVX", m_pv->position().x() );
    tuple->column( "PVY", m_pv->position().y() );
    tuple->column( "PVZ", m_pv->position().z() );
    tuple->column( "PVXerr", sqrt( m_pv->covMatrix()(0,0) ) );
    tuple->column( "PVYerr", sqrt( m_pv->covMatrix()(1,1) ) );
    tuple->column( "PVZerr", sqrt( m_pv->covMatrix()(2,2) ) );
    tuple->column( "PVXNoRefit", pv_noRefit->position().x() );
    tuple->column( "PVYNoRefit", pv_noRefit->position().y() );
    tuple->column( "PVZNoRefit", pv_noRefit->position().z() );
    tuple->column( "PVXerrNoRefit", sqrt( pv_noRefit->covMatrix()(0,0) ) );
    tuple->column( "PVYerrNoRefit", sqrt( pv_noRefit->covMatrix()(1,1) ) );
    tuple->column( "PVZerrNoRefit", sqrt( pv_noRefit->covMatrix()(2,2) ) );
    
    vector<double> statesX;
    vector<double> statesY;
    vector<double> statesZ;
    vector<unsigned int> stationNos;
    vector<bool> isR;
    vector<unsigned int> sensorNos;
    
    for( Track::ConstNodeRange::const_iterator inode = m_track->nodes().begin();
         inode != m_track->nodes().end(); ++inode){
      
      if( (*inode)->type() != Node::HitOnTrack ) continue;
      
      if( (*inode)->measurement().lhcbID().isVelo() ){
        
        statesX.push_back( (*inode)->state().position().x() );
        statesY.push_back( (*inode)->state().position().y() );
        statesZ.push_back( (*inode)->state().position().z() );
        if( (*inode)->measurement().type() == Measurement::VeloPhi ){
          stationNos.push_back( ((VeloPhiMeasurement&)((*inode)->measurement())).sensor().station() );
          sensorNos.push_back( ((VeloPhiMeasurement&)((*inode)->measurement())).sensor().sensorNumber() );
          isR.push_back( false );
        }
        else{
          stationNos.push_back( ((VeloRMeasurement&)((*inode)->measurement())).sensor().station() );
          sensorNos.push_back( ((VeloRMeasurement&)((*inode)->measurement())).sensor().sensorNumber() );
          isR.push_back( true );
        }
      }
    }
    
    State firstHitState = statesZ.size() != 0 ? m_track->closestState( *(statesZ.rbegin()) ) : m_track->firstState() ;
    
    tuple->farray( "VeloStates_X", statesX, "nMeasurements", 42 );
    tuple->farray( "VeloStates_Y", statesY, "nMeasurements", 42 );
    tuple->farray( "VeloStates_Z", statesZ, "nMeasurements", 42 );
    tuple->farray( "MeasStationNos", stationNos, "nMeasurements", 42 );
    tuple->farray( "MeasSensorNos", sensorNos, "nMeasurements", 42 );
    tuple->farray( "MeasIsR", isR, "nMeasurements", 42 );
    tuple->column( "firstState_Z", firstHitState.z() ) ;
    
    vector< unsigned int > rSens ;
    vector< unsigned int > pSens ;
    
    for( vector< LHCbID >::const_iterator idit = m_track->lhcbIDs().begin() ; idit != m_track->lhcbIDs().end(); ++idit ){
      if( idit->isVeloR() ) rSens.push_back( idit->veloID().sensor() ) ;
      else if( idit->isVeloPhi() ) pSens.push_back( idit->veloID().sensor() ) ;
    }
    tuple->farray( "rHitSensors", rSens, "nVeloRIDs", 23 ) ;
    tuple->farray( "phiHitSensors", pSens, "nVeloPhiIDs", 21 ) ;
    
    IMaterialLocator::Intersections intersections ;
    const StateVector stateVectorAtPVZ( stateAtPVZ.stateVector(), stateAtPVZ.z() ) ;
    const StateVector stateVectorAtFirstHit( firstHitState.stateVector(), firstHitState.z() ) ;
    
    m_materialLocator->intersect( stateVectorAtPVZ, stateVectorAtFirstHit, intersections ) ;
    int nIntersections = (int)intersections.size() ;
    
    double xOverX0 = 0. ;
    vector<double> distances ;
    vector<double> X0s ;
    vector<string> names ;
    vector<double> z1s ;
    vector<double> z2s ;
    
    for( IMaterialLocator::Intersections::iterator intersectionIt = intersections.begin() ;
         intersectionIt != intersections.end() ; ++ intersectionIt ){
      double deltaZ = intersectionIt->z2 - intersectionIt->z1 ;
      double distance = sqrt( pow( deltaZ, 2 ) + pow( intersectionIt->ty*deltaZ, 2 ) + pow( intersectionIt->tx*deltaZ, 2 ) ) ;
      xOverX0 += distance/intersectionIt->material->radiationLength() ;
      distances.push_back( distance ) ;
      X0s.push_back( intersectionIt->material->radiationLength() ) ;
      names.push_back( intersectionIt->material->name() ) ;
      z1s.push_back( intersectionIt->z1 ) ;
      z2s.push_back( intersectionIt->z2 ) ;
      
    }
    tuple->column( "xOverX0", xOverX0 ) ;
    tuple->column( "nIntersections", nIntersections ) ;
    tuple->farray( "materialDistances", distances, "dummyNIntersections", 100 ) ;
    tuple->farray( "materialX0s", X0s, "dummyNIntersections", 100 ) ;
    //tuple->farray( "materialNames", names, "dummyNIntersections", 100 ) ;
    tuple->farray( "materialZ1s", z1s, "dummyNIntersections", 100 ) ;
    tuple->farray( "materialZ2s", z2s, "dummyNIntersections", 100 ) ;
    
    const vector<LHCb::LHCbID>& ids = m_track->lhcbIDs();
    const unsigned int nTTHits=count_if(ids.begin(),ids.end(),bind(&LHCbID::isTT,_1));
    unsigned int nExpectedTTHits=m_TTExpectTool->nExpected(*m_track);
    
    tuple->column( "nTTHits", nTTHits ) ;
    tuple->column( "nExpectedTTHits", nExpectedTTHits ) ;
    
    const unsigned int nVeloHits=count_if(ids.begin(),ids.end(),bind(&LHCbID::isVelo,_1));
    unsigned int nExpectedVeloHits=m_VeloExpectTool->nExpected(*m_track);
    unsigned int nMissedVeloHits = m_VeloExpectTool->nMissed(*m_track);
    double pseudoEff=((double) nVeloHits)/((double) nExpectedVeloHits);
    
    tuple->column( "nVeloHits", nVeloHits ) ;
    tuple->column( "nExpectedVeloHits", nExpectedVeloHits ) ;
    tuple->column( "nMissedVeloHits", nMissedVeloHits ) ;
    tuple->column( "VeloHitPseudoEff", pseudoEff ) ;
    
    if( m_withMC ){
      MCVertex* mcpv = new MCVertex();
      mcpv->setPosition( Gaudi::XYZPoint( -999, -999, -999 ) );
      unsigned int mctype;
      double mcInversePT;
      checkMCAssoc( m_track, m_pv, mcpv, mcInversePT, mctype );
      RecVertex dummyPV = RecVertex( mcpv->position() );
      vector<float> matrixValues( 6, 0. );
      dummyPV.setCovMatrix( Gaudi::SymMatrix3x3( matrixValues.begin(), matrixValues.end() ) );
      double mcip3d, mcip3dsigma, mcipx, mcipxsigma, mcipy, mcipysigma ;
      calculateIPs( (const RecVertex*)(&dummyPV), m_track, mcip3d, mcip3dsigma, mcipx, mcipxsigma, 
                    mcipy, mcipysigma, stateAtPVZ, POCAtoPVstate );
      
      tuple->column( "MCType", mctype );
      tuple->column( "MCIPRes3D", mcip3d );
      tuple->column( "MCIPRes3Dsigma", mcip3dsigma );
      tuple->column( "MCIPRes_X", mcipx );
      tuple->column( "MCIPRes_Xsigma", mcipxsigma );
      tuple->column( "MCIPRes_Y", mcipy );
      tuple->column( "MCIPRes_Ysigma", mcipysigma );
      tuple->column( "MCInversePT", mcInversePT );
      tuple->column( "MCPVX", mcpv->position().x() );
      tuple->column( "MCPVY", mcpv->position().y() );
      tuple->column( "MCPVZ", mcpv->position().z() );
      delete mcpv;
    }
    
    tuple->write();
    
    if( m_refitPVs && isInPV ) delete m_pv;
    
  } // close loop over tracks
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitorNT::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//====================================================================
// Calculate ip_3D, ip_X and ip_y & their associated chi2
//====================================================================

StatusCode Velo::VeloIPResolutionMonitorNT::calculateIPs( const RecVertex* pv, const Track* track, 
                                                        double& ip3d, double& ip3dsigma, double& ipx, double& ipxsigma,
                                                        double& ipy, double& ipysigma, State& stateAtPVZ, State& POCAtoPVstate ){
  
  bool isSuccess( true ) ;

  // extrapolate the current track to its point of closest approach to the PV 
  // to get the 3D IP
  isSuccess &= m_trackExtrapolator->propagate( *track, pv->position(), POCAtoPVstate ).isSuccess();
  //State POCAtoPVstate( StateVector( extrapolateToPOCA( track, pv->position() ), track->slopes() ) ) ;
  distance( pv, POCAtoPVstate, ip3d, ip3dsigma, 0 );
  
  // extrapolate the current track to the same Z position as the PV to get X & Y IP
  stateAtPVZ = track->firstState();
  isSuccess &= m_trackExtrapolator->propagate( stateAtPVZ, pv->position().z() ).isSuccess() ;
  //State stateAtPVZ( StateVector( extrapolateToZ( track, pv->position().z() ), track->slopes() ) ) ; 
  distance( pv, stateAtPVZ, ipx, ipxsigma, 1 );

  distance( pv, stateAtPVZ, ipy, ipysigma, 2 );


  return StatusCode( isSuccess ) ;

}

//====================================================================
// calculate the distance & distance error between a vertex & a track state
// error is calculated assuming the PV position & state position are uncorrelated
//====================================================================

void Velo::VeloIPResolutionMonitorNT::distance( const RecVertex* pv, State state, 
                                                    double& dist, double& sigma, int type=0 )
{
  const Gaudi::XYZVector delta = pv->position() - state.position() ;
  double theta = state.slopes().theta()/rad ;
  
  Gaudi::SymMatrix3x3 covpv ( pv->covMatrix() ) ;
  const Gaudi::SymMatrix3x3 covpos( state.errPosition() );
  
  if( type==0 ){
    dist = sqrt( delta.mag2() );
    sigma = sqrt( pow( cos(theta), 2 )*( covpv(0,0) + covpos(0,0) + covpv(1,1) + covpos(1,1) ) + 
                  pow( sin(theta), 2 )*( covpv(2,2) + covpos(2,2) ) );
  }
  else if( type==1 ){ 
    dist = delta.x();
    sigma = sqrt( covpv(0,0) + covpos(0,0) );
  }
  else if( type==2 ){ 
    dist = delta.y();
    sigma = sqrt( covpv(1,1) + covpos(1,1) ); 
  }
  else{
    dist = -999;
    sigma = -999;
  }

}


//=========================================================================
//  Check MC association of tracks, rejecting non-prompt and/or using MC PV position if selected
//=========================================================================
StatusCode Velo::VeloIPResolutionMonitorNT::checkMCAssoc( const Track* track, const RecVertex* pv,
                                                        MCVertex*& mcpv, 
                                                        double& inversept, unsigned int& type )
{
  type = 999;
  inversept = 1./(-0.0001*GeV);
  
  LinkedTo<MCParticle,Track> directLink( evtSvc(), msgSvc(), TrackLocation::Default );
  MCParticle* mcPart = directLink.first( track );
  if( mcPart==NULL ){ 
    type = 2;
    counter("Ghost tracks")++;
    return StatusCode::SUCCESS;
  }
  inversept = 1./( mcPart->pt()/GeV );
  delete mcpv;
  mcpv = (MCVertex*)(mcPart->primaryVertex()->clone()) ;
  if( !mcpv ){ 
    counter("No MC primary vertex")++;
    return StatusCode::SUCCESS;
  }

  else if( mcPart->mother()==NULL || (*(mcPart->mother()->endVertices().begin()))->position() == mcpv->position() ) {
    if( (mcpv->position() - pv->position()).mag2() > 25.*( pv->covMatrix()(0,0) + pv->covMatrix()(1,1) + pv->covMatrix()(2,2) ) ){
      counter("Prompt assoc. MC w/ MCPV > 5 sigma from rec PV")++;
      type = 4;
    }
    else{    
      counter("Prompt assoc. MC particles")++;
      type = 0;
    }
    return StatusCode::SUCCESS;
  }
  else { 
    counter("Secondary assoc. MC particles")++;
    type = 1;
    return StatusCode::SUCCESS;
  }
          
}

//========================================================
// Linear extrapolator from a track to a given z position.
//========================================================
Gaudi::XYZPoint Velo::VeloIPResolutionMonitorNT::extrapolateToZ(const Track *track, double toZ)
{
  // get state parameters
  Gaudi::XYZPoint coord = track->position();
  Gaudi::XYZVector slope = track->slopes();
  Gaudi::XYZPoint result;
  double deltaZ=toZ-coord.z();
  result.SetXYZ(coord.x()+slope.x()*deltaZ, coord.y()+slope.y()*deltaZ, toZ);
  
  return result;
}

//========================================================
// Linear extrapolator from a track to its POCA to a given point.
//========================================================
Gaudi::XYZPoint Velo::VeloIPResolutionMonitorNT::extrapolateToPOCA(const Track *track, Gaudi::XYZPoint point )
{
  // get state parameters
  Gaudi::XYZPoint coord = track->position();
  Gaudi::XYZVector slope = track->slopes();
  Gaudi::XYZVector delta = coord - point ;
  Gaudi::XYZPoint result = coord - slope*(delta.Dot( slope )/slope.mag2() ) ;
  
  return result;
}

