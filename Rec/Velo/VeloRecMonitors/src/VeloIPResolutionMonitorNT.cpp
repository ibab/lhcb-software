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
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_vertexLocation(RecVertexLocation::Primary),
    m_trackLocation(TrackLocation::Default),
    m_pv(NULL),
    m_track(NULL),
    m_withMC(false),
    m_refitPVs(true),
    m_checkIDs(false), 
    m_checkFrac(0.7),
    m_trackExtrapolatorType(),
    m_materialLocatorType(),
    m_trackExtrapolator(NULL),
    m_pvtool(NULL),
    m_materialLocator(NULL),
    m_TTExpectTool(NULL),
    m_VeloExpectTool(NULL)
{

  // set input locations
  declareProperty("TrackLocation", m_trackLocation) ;
  declareProperty("VertexLocation", m_vertexLocation);

  // set whether to do comparison to MC
  declareProperty("WithMC",m_withMC=false);

  // Set whether to refit PVs without the track for which IP is being calculated
  declareProperty("RefitPVs", m_refitPVs);
  declareProperty("CheckIDs", m_checkIDs) ;
  declareProperty("CheckIDsFraction", m_checkFrac) ;

  declareProperty("TrackExtrapolatorType", m_trackExtrapolatorType) ;
  declareProperty("MaterialLocatorType", m_materialLocatorType) ;
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
  
  // Tried to get the material locator and track extrapolator from the 
  // TrackMasterFitter, but it crashed when trying to retrieve the properties
  // from the tool.
  //AlgTool* masterFitter = dynamic_cast<AlgTool*>(tool<ITrackFitter>("TrackMasterFitter")) ;
  
  string materialLocatorType = 
    m_materialLocatorType.size() > 0 ? m_materialLocatorType
    //: NULL != masterFitter ? 
    //static_cast<const PropertyWithValue<ToolHandle<IMaterialLocator> >* >(&masterFitter->getProperty("MaterialLocator"))->value()->name()
    : "DetailedMaterialLocator" ;

  string trackExtrapolatorType = 
    m_trackExtrapolatorType.size() > 0 ? m_trackExtrapolatorType
    //: NULL != masterFitter ? 
    //static_cast<const PropertyWithValue<ToolHandle<IMaterialLocator> >* >(&masterFitter->getProperty("Extrapolator"))->value()->name()
    : "TrackMasterExtrapolator" ;

  m_materialLocator = 
    tool<IMaterialLocator>(materialLocatorType,
			   "MaterialLocator",this) ;

  // get the track extrapolator used in calculating the IPs
  m_trackExtrapolator = 
    tool<ITrackExtrapolator>(trackExtrapolatorType,
			     "Extrapolator",this);

  // get hit expectation tools
  m_TTExpectTool = tool<IHitExpectation>( "TTHitExpectation");
  m_VeloExpectTool = tool<IVeloExpectation>( "VeloExpectation");

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitorNT::execute() {
  //std::cout << "begin execute" << std::endl ;

  
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
  // comment out ???
  //if( pvs->size() != 1 ) return StatusCode::SUCCESS;

  //std::cout << pvs->size() << " PVs in event" << std::endl;

  //Create Vector of PVs
  RecVertex::ConstVector PVertices;
  std::vector<unsigned int> nVeloTracks;//create empty vector for nVeloTtracks count 
  // *** You probably want RecVertices::const_iterator it_pv as pvs is of type RecVertices.
  for ( RecVertices::const_iterator it_pv = pvs->begin(); it_pv != pvs->end(); it_pv++)
  { 
    counter("PVs Analysed")++;
    //pv_noRefit and track# requirements moved to this loop
    // *** You want to check whether the vertex is primary and how many tracks
    // it has before you add it to the vector of PVs. I think you want something like
    // (*it_pv)->isPrimary()
    if( !(*it_pv)->isPrimary() ) continue;    
    if( m_refitPVs && (*it_pv)->tracks().size() < 3 ) continue;
    PVertices.push_back( &(**it_pv) );
    nVeloTracks.push_back(0);
    // can't refit a PV with only one track!
    // *** You haven't declared or filled PVtracks yet, so this won't work.
    // Again I think you'll want something like (*it_pv)->tracks().size()

    counter("PVs Selected")++;
    // *** You want to increment this counter before you apply any selection 
    // criteria to the PVs, ie, put this at the start of this code block.
    
  }
  RecVertex::ConstVector pv_noRefit(PVertices);

  // get the tracks
  Tracks* tracks = getIfExists<Tracks>(m_trackLocation);
  if( NULL == tracks )
  {
    string counterName = string("No tracks at ") + m_trackLocation ;
    counter( counterName )++;
    return StatusCode::SUCCESS;
  }
  
  counter("Events Selected")++;
  // Set up PV containesr???
  // Loop over PVs here???
  //m_pv = *(pvs->begin()) ;
  //const LHCb::RecVertex* pv_noRefit = m_pv ;    
  
  
  
  // Get tracks from current PV & loop
  std::vector<Track::ConstVector> PVtracks(PVertices.size()) ;


  //std::cout << "store pvs" << std::endl ;

  // count number of tracks making this pv that are reconstructed in the velo 
  // loop over PVs
  for( unsigned int jpv=0; jpv<PVertices.size(); jpv++)
  { 
    for ( SmartRefVector< Track >::const_iterator tr = PVertices[jpv]->tracks().begin(); 
        tr != PVertices[jpv]->tracks().end() ; tr++ ){
      PVtracks[jpv].push_back( &(**tr) );//will this work?
      if( (*tr)->type()==Track::Velo || (*tr)->type()==Track::Upstream || (*tr)->type()==Track::Long ){
        nVeloTracks[jpv] += 1;
      }
    }
  }
 
  // loop over tracks 
  //std::cout << "loop over tracks, " << tracks->size() << " tracks" << std::endl ;

  for( Tracks::const_iterator tr = tracks->begin(); tr != tracks->end(); ++tr ){

    //std::cout << "next loop" << endl ;
    //int _itrack = (tr - tracks->begin()) ;
    //std::cout << "track " << _itrack << std::endl ;

    if( (*tr)->type() != Track::Velo && (*tr)->type() != Track::Upstream && (*tr)->type() != Track::Long ) continue;      
    
    m_track = *tr;     

    // *** You want to declare *all* the vectors like this.
    vector<double> ip3d(PVertices.size(), 0.);
    vector<double> ip3dsigma(PVertices.size(), 0);
    vector<double> ipx(PVertices.size(), 0.); 
    vector<double> ipxsigma(PVertices.size(), 0.); 
    vector<double> ipy(PVertices.size(), 0.); 
    vector<double> ipysigma(PVertices.size(), 0.);  
    // *** I think you'll only need to store the track states at whichever PV gives the smallest IP, 
    // so you won't need vectors for these, just a single instance.
    State POCAtoPVstate, stateAtPVZ ;
    double temp_ip3d, temp_ip3dsigma, temp_ipx, temp_ipxsigma, temp_ipy, temp_ipysigma ;//temporary variables to store info for current PV
    State temp_POCAtoPVstate, temp_stateAtPVZ ;
    vector<double> vec_PVNTracks(PVertices.size(), 0.), vec_PVChi2(PVertices.size(), 0.), vec_PVNDOF(PVertices.size(), 0.), vec_PVX(PVertices.size(), 0.), vec_PVY(PVertices.size(), 0.), vec_PVZ(PVertices.size(), 0.);
    vector<double> vec_PVXerr(PVertices.size(), 0.), vec_PVYerr(PVertices.size(), 0.), vec_PVZerr(PVertices.size(), 0.), vec_PVXNoRefit(PVertices.size(), 0.), vec_PVYNoRefit(PVertices.size(), 0.);
    vector<double> vec_PVZNoRefit(PVertices.size(), 0.), vec_PVXerrNoRefit(PVertices.size(), 0.), vec_PVYerrNoRefit(PVertices.size(), 0.), vec_PVZerrNoRefit(PVertices.size(), 0.);
    vector<bool> isInPV(PVertices.size(), false);
    int index_minIP=0;
    double TestIP=1e8;
    
    //Loop over PVs
    //std::cout << "track " << _itrack << " loop over pvs" << std::endl ;
    for(unsigned int kpv=0; kpv<PVertices.size(); kpv++)
    {
      
      // refit PV removing current track
      const Track* pvTrack = matchTrack(*m_track, *PVertices[kpv]) ;
      bool temp_isInPV = (pvTrack != NULL) ;
      isInPV[kpv] =  temp_isInPV ;
      
      //std::cout << "track " << _itrack << " pv " << kpv << " check isinPV " << temp_isInPV <<  std::endl ;

      if( m_refitPVs && isInPV[kpv] ){
        
        RecVertex* newVertex = new RecVertex( *PVertices[kpv] ) ;
        vector< const Track* > trackToRemove( 1, pvTrack ) ;
        StatusCode scPVfit = 
          m_pvtool->reDoSinglePV( Gaudi::XYZPoint( newVertex->position() ), trackToRemove, *newVertex ) ;
        if( scPVfit.isFailure() ){
          delete newVertex ;
	  isInPV[kpv] = false ;
	  //std::cout << "track " << _itrack << " pv " << kpv << "refit failed" << std::endl; 
          continue ;
        }
        else
          //m_pv = newVertex ;//Does this need to change? 
          PVertices[kpv] = newVertex;
      }             

    
      //std::cout << "track " << _itrack << " pv " << kpv << " calcIPs" << std::endl ;

      StatusCode scCalcIPs = calculateIPs( PVertices[kpv], m_track, temp_ip3d, temp_ip3dsigma, temp_ipx, temp_ipxsigma, temp_ipy, 
                                           temp_ipysigma, temp_stateAtPVZ, temp_POCAtoPVstate );
      if( scCalcIPs.isFailure() ) continue ;
      
      //fill vectors for PV variables
      // *** Remember you need to check which PV gives the smallest IP and store its index.
      ip3d[kpv] = temp_ip3d;
      ip3dsigma[kpv] = temp_ip3dsigma;
      ipx[kpv] = temp_ipx;
      ipxsigma[kpv] = temp_ipxsigma;
      ipy[kpv] = temp_ipy;
      ipysigma[kpv] = temp_ipysigma;
    
      // Record the index of smallest IP PV
      if(temp_ip3d < TestIP){
        TestIP = temp_ip3d;
        index_minIP = kpv;
        stateAtPVZ = temp_stateAtPVZ;
        POCAtoPVstate = temp_POCAtoPVstate;
      }
      
      //stateAtPVZ.push_back( temp_stateAtPVZ );
      //POCAtoPVstate.push_back( temp_POCAtoPVstate );
     
      //std::cout << "track " << _itrack << " pv " << kpv << " fill vectors" << std::endl ;
 
      vec_PVNTracks[kpv] = PVertices[kpv]->tracks().size();
      vec_PVChi2[kpv] = PVertices[kpv]->chi2();
      vec_PVNDOF[kpv] = PVertices[kpv]->nDoF();
      vec_PVX[kpv] = PVertices[kpv]->position().x();
      vec_PVY[kpv] = PVertices[kpv]->position().y();
      vec_PVZ[kpv] = PVertices[kpv]->position().z();
      vec_PVXerr[kpv] = sqrt(PVertices[kpv]->covMatrix()(0,0));
      vec_PVYerr[kpv] = sqrt(PVertices[kpv]->covMatrix()(1,1));
      vec_PVZerr[kpv] = sqrt(PVertices[kpv]->covMatrix()(2,2));
      vec_PVXNoRefit[kpv] = pv_noRefit[kpv]->position().x();
      vec_PVYNoRefit[kpv] = pv_noRefit[kpv]->position().y();
      vec_PVZNoRefit[kpv] = pv_noRefit[kpv]->position().z();
      vec_PVXerrNoRefit[kpv] = sqrt(pv_noRefit[kpv]->covMatrix()(0,0));
      vec_PVYerrNoRefit[kpv] = sqrt(pv_noRefit[kpv]->covMatrix()(1,1));
      vec_PVZerrNoRefit[kpv] = sqrt(pv_noRefit[kpv]->covMatrix()(2,2));
    
    }
  
  
    counter("Tracks selected")++;
    
    double inversePT = 1./(m_track->pt()/GeV);
    
    //std::cout << "track " << _itrack << " fill tuple" << endl ;

    Tuple tuple = nTuple( "IPResolutionsTuple" );
    //Track variables
    tuple->column( "P", m_track->p() );
    tuple->column( "Eta", m_track->pseudoRapidity() );
    tuple->column( "Phi", m_track->phi() );
    tuple->column( "TrackTx", m_track->slopes().x() );
    tuple->column( "TrackTy", m_track->slopes().y() );
    tuple->column( "TrackCharge", m_track->charge() );
    tuple->column( "TrackChi2", m_track->chi2() );
    tuple->column( "TrackNDOF", m_track->nDoF() );
    tuple->column( "TrackType", m_track->type() );
    tuple->column( "IsTrackForward", m_track->flag() != Track::Backward ) ;
    tuple->column( "MinIPindex", index_minIP );
    tuple->column( "InversePT", inversePT );
    
    //fill tuple arrays
    tuple->farray( "isInPV", isInPV, "nPVs", 50 );
    tuple->farray( "IPRes3D", ip3d , "nPVs", 50);
    tuple->farray( "IPRes3Dsigma", ip3dsigma , "nPVs", 50);
    tuple->farray( "IPRes_X", ipx , "nPVs", 50);
    tuple->farray( "IPRes_Xsigma", ipxsigma , "nPVs", 50);
    tuple->farray( "IPRes_Y", ipy , "nPVs", 50);
    tuple->farray( "IPRes_Ysigma", ipysigma , "nPVs", 50);

    tuple->farray( "PVNVeloTracks", nVeloTracks , "nPVs", 50);

    tuple->farray( "PVNTracks", vec_PVNTracks , "nPVs", 50);
    tuple->farray( "PVChi2", vec_PVChi2 , "nPVs", 50);
    tuple->farray( "PVNDOF", vec_PVNDOF , "nPVs", 50);
    tuple->farray( "PVX", vec_PVX , "nPVs", 50);
    tuple->farray( "PVY", vec_PVY , "nPVs", 50);
    tuple->farray( "PVZ", vec_PVZ , "nPVs", 50);
    tuple->farray( "PVXerr", vec_PVXerr , "nPVs", 50);
    tuple->farray( "PVYerr", vec_PVYerr , "nPVs", 50);
    tuple->farray( "PVZerr", vec_PVZerr , "nPVs", 50);
    tuple->farray( "PVXNoRefit", vec_PVXNoRefit , "nPVs", 50);
    tuple->farray( "PVYNoRefit", vec_PVYNoRefit , "nPVs", 50);
    tuple->farray( "PVZNoRefit", vec_PVZNoRefit , "nPVs", 50);
    tuple->farray( "PVXerrNoRefit", vec_PVXerrNoRefit , "nPVs", 50);
    tuple->farray( "PVYerrNoRefit", vec_PVYerrNoRefit , "nPVs", 50);
    tuple->farray( "PVZerrNoRefit", vec_PVZerrNoRefit , "nPVs", 50);

    //std::cout << "track " << _itrack << " ip info filled" << endl ;
    
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
    tuple->farray( "materialDistances", distances, "dummyNIntersections", 200 ) ;
    tuple->farray( "materialX0s", X0s, "dummyNIntersections", 200 ) ;
    //tuple->farray( "materialNames", names, "dummyNIntersections", 200 ) ;
    tuple->farray( "materialZ1s", z1s, "dummyNIntersections", 200 ) ;
    tuple->farray( "materialZ2s", z2s, "dummyNIntersections", 200 ) ;
    
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
      checkMCAssoc( m_track, PVertices[index_minIP], mcpv, mcInversePT, mctype );
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
    
    for(unsigned int lpv = 0 ; lpv < PVertices.size() ; lpv++)
    {
      if(m_refitPVs && isInPV[lpv]) {
	//std::cout << "track " << _itrack << " deleting pv " << lpv << std::endl ;
	delete PVertices[lpv] ;
      }
    }
    PVertices = pv_noRefit ;

    //std::cout << "track " << _itrack << " tuple filled" << endl ;
    
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

//========================================================
// Check if the given track is used in the given PV.
//========================================================
const Track* Velo::VeloIPResolutionMonitorNT::matchTrack(const Track& track, 
							 const RecVertex& pv) const {
  // First do a pointer comparison using RecVertex::trackWeight
  pair<bool, float> trWeight = pv.trackWeight(&track) ;
  if(trWeight.first){
    // The track is contained in the PV and has non-zero weight.
    if(trWeight.second != 0.)
      return &track ;
    // The track is contained in the PV but has zero weight.
    return NULL ;
  }
  if(!m_checkIDs)
    return NULL ;

  // Compare LHCb IDs if requested.
  const unsigned int nIDs = track.nLHCbIDs() ;
  // Loop over tracks in the PV & their weights.
  const RecVertex::TrackWithWeightVector& tracksAndWeights = pv.tracksWithWeights() ;
  for(RecVertex::TrackWithWeightVector::const_iterator iTrW = tracksAndWeights.begin() ;
      iTrW != tracksAndWeights.end() ; ++iTrW){
    const Track* pvtr = iTrW->first ;
    // Check PV track is not NULL and has non-zero weight.
    if(NULL == iTrW->first || 0. == iTrW->second)
      continue ;
    const unsigned int nPVTrIDs = pvtr->nLHCbIDs() ;
    const float nCommon = float(track.nCommonLhcbIDs(*pvtr)) ;
    // If the PV track shares a certain fraction of LHCbIDs then they match.
    if(nCommon/min(nIDs, nPVTrIDs) > m_checkFrac)
      return pvtr ;
  }
  // No matching tracks with non zero weight were found in the PV.
  return NULL ;
}

