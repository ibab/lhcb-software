// Include files
#include "VeloIPResolutionMonitor.h"

// from Gaudi
#include <GaudiKernel/SystemOfUnits.h>
#include <GaudiUtils/Aida2ROOT.h>
#include <GaudiKernel/PhysicalConstants.h>

#include <Event/Track.h>

#include <TF1.h> 
#include <TFile.h>

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
// Implementation file for class : VeloIPResolutionMonitor
//
// 2009-05-15 : Michael Thomas Alexander
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo
{
  DECLARE_ALGORITHM_FACTORY( VeloIPResolutionMonitor )
}


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloIPResolutionMonitor::VeloIPResolutionMonitor( const string& name,
                                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator ),

    m_h_IPXVsInversePT(0),
    m_h_IPYVsInversePT(0), 
                               
    m_h_IPXVsPhi(0),
    m_h_IPYVsPhi(0),
    m_h_IPXVsPhi_HighPT(0),
    m_h_IPYVsPhi_HighPT(0), 
                               
    m_h_IPXVsPhi_Eta45To50(0),
    m_h_IPYVsPhi_Eta45To50(0), 
    m_h_IPXVsPhi_Eta25To30(0),
    m_h_IPYVsPhi_Eta25To30(0), 
    m_h_IPXVsPhi_EtaLT0(0),
    m_h_IPYVsPhi_EtaLT0(0), 
                                
    m_h_IPXVsEta(0), 
    m_h_IPYVsEta(0), 
                         
    m_h_TrackMultiplicity(0), 

    m_vertexLocation(RecVertexLocation::Primary),
    m_trackLocation(TrackLocation::Default),

    m_pv(0),
    m_track(0),

    m_InversePTMin(0.),
    m_InversePTMax(3.),
    m_nBins(20),
    m_nBinsPhi(32),
    m_nBinsEta(50),
    
    m_minPVnTracks(26),
    m_maxTrackChi2PerNDOF(4.),
    m_maxP(500.),
    m_minNRHits(6),
    m_minNTTHits(1),
    
    m_limitIntercept1D(16.e-03F),
    m_limitGradient1D(26.7e-03F),
    m_limitFactor(10.),

    m_refitPVs(false),
    m_makePlotsVsPhiInBinsOfEta(false),

    m_save2DHistos(false),
    
    m_trackExtrapolator(0),
    m_pvtool(0)

{

  // set input locations
  declareProperty("TrackLocation", m_trackLocation = TrackLocation::Default ) ;
  declareProperty("VertexLocation", m_vertexLocation = RecVertexLocation::Primary );
  
  // Set the binning options
  declareProperty("InversePTMin",m_InversePTMin = 0.0 );
  declareProperty("InversePTMax",m_InversePTMax = 3.0 );
  declareProperty("NBins",m_nBins = 20 );
  declareProperty("NBinsPhi", m_nBinsPhi = 32 ) ;
  declareProperty("NBinsEta", m_nBinsEta = 50 ) ;
  
  declareProperty("MinPVnTracks", m_minPVnTracks = 26 );
  declareProperty("MaxTrackChi2PerNDOF", m_maxTrackChi2PerNDOF = 4. ) ;
  declareProperty("MaxP", m_maxP = 500. ) ;
  declareProperty("MinNRHits", m_minNRHits = 6 ) ;
  declareProperty("MinNTTHits", m_minNTTHits = 1 ) ;

  // set the values of the linear parametrisation used to determine the limits 
  // of the underlying histograms
  declareProperty("ParametrisationYIntercept1D", m_limitIntercept1D = 16.e-03F );
  declareProperty("ParametrisationGradient1D", m_limitGradient1D = 26.7e-03F );
  declareProperty("HistoLimitNSigma", m_limitFactor = 10. );

  // Set whether to refit PVs without the track for which IP is being calculated
  declareProperty("RefitPVs", m_refitPVs = false );

  declareProperty("MakePlotsVsPhiInBinsOfEta", m_makePlotsVsPhiInBinsOfEta = false ) ;

  declareProperty("Save2DHistos", m_save2DHistos = false ) ;
  
}
//=============================================================================
// Destructor
//=============================================================================
Velo::VeloIPResolutionMonitor::~VeloIPResolutionMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;

  // Set histo dir
  setHistoTopDir( "Velo/" );

  // Get vertexer if required
  if( m_refitPVs ) m_pvtool = tool<IPVOfflineTool>( "PVOfflineTool" );

  // get the track extrapolator used in calculating the IPs
  m_trackExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator","Extrapolator",this);

  float limit1D = m_limitFactor * ( m_limitGradient1D * m_InversePTMax + m_limitIntercept1D );
  m_h_IPXVsInversePT = book2DHisto( "IPX-Vs-InversePT-LongTracks", "IPX-Vs-InversePT-LongTracks", 
                                    m_nBins, m_InversePTMin, m_InversePTMax, 500, -limit1D, limit1D, m_save2DHistos ) ;
  m_h_IPYVsInversePT = book2DHisto( "IPY-Vs-InversePT-LongTracks", "IPY-Vs-InversePT-LongTracks", 
                                    m_nBins, m_InversePTMin, m_InversePTMax, 500, -limit1D, limit1D, m_save2DHistos ) ;
  
  m_h_IPXVsPhi = book2DHisto( "IPX-Vs-Phi-LongTracks", "IPX-Vs-Phi-LongTracks", m_nBinsPhi, -pi, pi,
                              500, -1.5*limit1D, 1.5*limit1D, m_save2DHistos ) ;
  m_h_IPYVsPhi = book2DHisto( "IPY-Vs-Phi-LongTracks", "IPY-Vs-Phi-LongTracks", m_nBinsPhi, -pi, pi, 
                              500, -1.5*limit1D, 1.5*limit1D, m_save2DHistos ) ;
  m_h_IPXVsPhi_HighPT = book2DHisto( "IPX-Vs-Phi-LongTracks-PTGreaterThan1GeV", "IPX-Vs-Phi-LongTracks-PTGreaterThan1GeV", 
                                     m_nBinsPhi, -pi, pi, 500, -limit1D, limit1D, m_save2DHistos ) ;
  m_h_IPYVsPhi_HighPT = book2DHisto( "IPY-Vs-Phi-LongTracks-PTGreaterThan1GeV", "IPY-Vs-Phi-LongTracks-PTGreaterThan1GeV", 
                                     m_nBinsPhi, -pi, pi, 500, -limit1D, limit1D, m_save2DHistos ) ;
  
  if( m_makePlotsVsPhiInBinsOfEta ){
    
    m_h_IPXVsPhi_Eta45To50 = book2DHisto( "IPX-Vs-Phi-AllTracks-Eta4.5To5.0", "IPX-Vs-Phi-AllTracks-Eta4.5To5.0", 
                                          m_nBinsPhi, -pi, pi, 500, -2.*limit1D, 2.*limit1D, m_save2DHistos ) ;
    m_h_IPYVsPhi_Eta45To50 = book2DHisto( "IPY-Vs-Phi-AllTracks-Eta4.5To5.0", "IPY-Vs-Phi-AllTracks-Eta4.5To5.0", 
                                          m_nBinsPhi, -pi, pi, 500, -2.*limit1D, 2.*limit1D, m_save2DHistos ) ;
    m_h_IPXVsPhi_Eta25To30 = book2DHisto( "IPX-Vs-Phi-AllTracks-Eta2.5To3.0", "IPX-Vs-Phi-AllTracks-Eta2.5To3.0", 
                                          m_nBinsPhi, -pi, pi, 500, -2.*limit1D, 2.*limit1D, m_save2DHistos ) ;
    m_h_IPYVsPhi_Eta25To30 = book2DHisto( "IPY-Vs-Phi-AllTracks-Eta2.5To3.0", "IPY-Vs-Phi-AllTracks-Eta2.5To3.0", 
                                          m_nBinsPhi, -pi, pi, 500, -2.*limit1D, 2.*limit1D, m_save2DHistos ) ;
    m_h_IPXVsPhi_EtaLT0 = book2DHisto( "IPX-Vs-Phi-AllTracks-EtaLessThan0", "IPX-Vs-Phi-AllTracks-EtaLessThan0", 
                                       m_nBinsPhi, -pi, pi, 500, -2.*limit1D, 2.*limit1D, m_save2DHistos ) ;
    m_h_IPYVsPhi_EtaLT0 = book2DHisto( "IPY-Vs-Phi-AllTracks-EtaLessThan0", "IPY-Vs-Phi-AllTracks-EtaLessThan0", 
                                       m_nBinsPhi, -pi, pi, 500, -2.*limit1D, 2.*limit1D, m_save2DHistos ) ;
  }
  
  m_h_IPXVsEta = book2DHisto( "IPX-Vs-Eta-LongTracks", "IPX-Vs-Eta-LongTracks", 
                              m_nBinsEta, 1.5, 5.5, 500, -1.5*limit1D, 1.5*limit1D, m_save2DHistos ) ;
  m_h_IPYVsEta = book2DHisto( "IPY-Vs-Eta-LongTracks", "IPY-Vs-Eta-LongTracks", 
                              m_nBinsEta, 1.5, 5.5, 500, -1.5*limit1D, 1.5*limit1D, m_save2DHistos ) ;
  
  // book additional histogram of PV track multiplicity
  m_h_TrackMultiplicity = Aida2ROOT::aida2root( book( "PVTrackMultiplicity", "PV Track Multiplicity", 0.0, 150.0, 75 ));
  m_h_TrackMultiplicity->SetXTitle("Number of tracks");
  

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitor::execute() {

  if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;

  counter( "Events Analysed" )++;
      
  // Get PVs
  const RecVertices* pvs = getIfExists<RecVertices>( m_vertexLocation );  
  if( NULL == pvs ) {
    string counterName = string("No vertices at ") + m_vertexLocation ;
    counter( counterName )++;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "No vertices at " << m_vertexLocation << endmsg;
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

  // apply ntracks cut
  if( nVeloTracks < m_minPVnTracks ) return StatusCode::SUCCESS ;

  // can't refit a PV with only one track!
  if( m_refitPVs && PVtracks.size() < 3 ) return StatusCode::SUCCESS ;

  counter("PVs Selected")++;
        
  m_h_TrackMultiplicity->Fill( nVeloTracks );    

  // loop over tracks
  for( Tracks::const_iterator tr = tracks->begin(); tr != tracks->end(); ++tr ){

    counter( "Tracks Analysed" )++ ;
      
    const vector<LHCb::LHCbID>& ids = (*tr)->lhcbIDs() ;
    const unsigned int nRHits = count_if( ids.begin(), ids.end(), bind(&LHCbID::isVeloR,_1) ) ;
    const unsigned int nTTHits = count_if( ids.begin(), ids.end(), bind(&LHCbID::isTT,_1) ) ;

    if( ((*tr)->type() != Track::Velo && (*tr)->type() != Track::Upstream && (*tr)->type() != Track::Long ) 
        || (*tr)->chi2()/(*tr)->nDoF() > m_maxTrackChi2PerNDOF || (*tr)->p()/GeV > m_maxP 
        || nRHits < m_minNRHits || nTTHits < m_minNTTHits ) continue;
    
    counter( "Tracks selected" )++;
    
    m_track = *tr;
    
    // refit PV removing current track
    bool isInPV = find( PVtracks.begin(), PVtracks.end(), m_track ) != PVtracks.end() ;        
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
    StatusCode scCalcIPs = 
      calculateIPs( m_pv, m_track, ip3d, ip3dsigma, ipx, ipxsigma, ipy, ipysigma, stateAtPVZ, POCAtoPVstate );
    if( scCalcIPs.isFailure() ) continue ;
    
    double inversePT = 1./(m_track->pt()/GeV);
    double phi = m_track->phi()/radian ;
    double eta = m_track->pseudoRapidity() ;
    
    if( m_makePlotsVsPhiInBinsOfEta ){
      if( m_track->flag() == Track::Backward ){
        m_h_IPXVsPhi_EtaLT0->Fill( phi, ipx ) ;
        m_h_IPYVsPhi_EtaLT0->Fill( phi, ipy ) ;
      }
      else if( 2.5 < eta && eta < 3.0 ){
        m_h_IPXVsPhi_Eta25To30->Fill( phi, ipx ) ;
        m_h_IPYVsPhi_Eta25To30->Fill( phi, ipy ) ;
      } 
      else if( 4.5 < eta && eta < 5.0 ){
        m_h_IPXVsPhi_Eta45To50->Fill( phi, ipx ) ;
        m_h_IPYVsPhi_Eta45To50->Fill( phi, ipy ) ;
      } 
    }
    
    if( m_track->type() == Track::Long ){
      
      m_h_IPXVsInversePT->Fill( inversePT, ipx ) ;
      m_h_IPYVsInversePT->Fill( inversePT, ipy ) ;
      
      m_h_IPXVsPhi->Fill( phi, ipx ) ;
      m_h_IPYVsPhi->Fill( phi, ipy ) ;
      if( inversePT < 1 ){
        m_h_IPXVsPhi_HighPT->Fill( phi, ipx ) ;
        m_h_IPYVsPhi_HighPT->Fill( phi, ipy ) ;
      }
      
      m_h_IPXVsEta->Fill( eta, ipx ) ;
      m_h_IPYVsEta->Fill( eta, ipy ) ;
      
    }
    
    if( m_refitPVs && isInPV ) delete m_pv;
    
  } // close loop over tracks

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  saveMeanAndSigmaProfilesAndXProjection( m_h_IPXVsInversePT, string("InversePTFrequency-LongTracks") ) ;
  
  saveMeanAndSigmaProfiles( m_h_IPYVsInversePT ) ;

  saveMeanAndSigmaProfilesAndXProjection( m_h_IPXVsPhi, string("PhiFrequency-LongTracks") ) ;
  
  saveMeanAndSigmaProfiles( m_h_IPYVsPhi ) ;
  
  saveMeanAndSigmaProfilesAndXProjection( m_h_IPXVsPhi_HighPT, string("PhiFrequency-LongTracks-PTGreaterThan1GeV") ) ;
  
  saveMeanAndSigmaProfiles( m_h_IPYVsPhi_HighPT ) ;
  
  if( m_makePlotsVsPhiInBinsOfEta ){
    
    saveMeanAndSigmaProfilesAndXProjection( m_h_IPXVsPhi_Eta45To50, string("PhiFrequency-AllTracks-Eta4.5To5.0") ) ;
    
    saveMeanAndSigmaProfiles( m_h_IPYVsPhi_Eta45To50 ) ;
    
    saveMeanAndSigmaProfilesAndXProjection( m_h_IPXVsPhi_Eta25To30, string("PhiFrequency-AllTracks-Eta2.5To3.0") ) ;
    
    saveMeanAndSigmaProfiles( m_h_IPYVsPhi_Eta25To30 ) ;
    
    saveMeanAndSigmaProfilesAndXProjection( m_h_IPXVsPhi_EtaLT0, string("PhiFrequency-AllTracks-EtaLessThan0") ) ;
    
    saveMeanAndSigmaProfiles( m_h_IPYVsPhi_EtaLT0 ) ;
    
  }
  
  saveMeanAndSigmaProfilesAndXProjection( m_h_IPXVsEta, string("EtaFrequency-LongTracks") ) ;
  
  saveMeanAndSigmaProfiles( m_h_IPYVsEta ) ;

  if( !m_save2DHistos ){
    
    delete m_h_IPXVsInversePT;
    delete m_h_IPYVsInversePT ;
    delete m_h_IPXVsPhi ;
    delete m_h_IPYVsPhi ;
    delete m_h_IPXVsPhi_HighPT ;
    delete m_h_IPYVsPhi_HighPT ;
    
    if( m_makePlotsVsPhiInBinsOfEta ){
      delete m_h_IPXVsPhi_Eta45To50;
      delete m_h_IPYVsPhi_Eta45To50 ;
      delete m_h_IPXVsPhi_Eta25To30 ;
      delete m_h_IPYVsPhi_Eta25To30 ;
      delete m_h_IPXVsPhi_EtaLT0 ;
      delete m_h_IPYVsPhi_EtaLT0 ;
    }

    delete m_h_IPXVsEta ;    
    delete m_h_IPYVsEta ;
  }
  
  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//====================================================================
// Calculate ip_3D, ip_X and ip_y & their associated chi2
//====================================================================

StatusCode Velo::VeloIPResolutionMonitor::calculateIPs( const RecVertex* pv, const Track* track, 
                                                        double& ip3d, double& ip3dsigma, double& ipx, double& ipxsigma,
                                                        double& ipy, double& ipysigma, State& stateAtPVZ, State& POCAtoPVstate ){
  
  StatusCode isSuccess;

  // extrapolate the current track to its point of closest approach to the PV 
  // to get the 3D IP
  isSuccess = m_trackExtrapolator->propagate( *track, pv->position(), POCAtoPVstate );
  if( isSuccess.isFailure() ) return isSuccess ;
  distance( pv, POCAtoPVstate, ip3d, ip3dsigma, 0 );
  
  // extrapolate the current track to the same Z position as the PV to get X & Y IP
  stateAtPVZ = track->firstState();
  isSuccess = m_trackExtrapolator->propagate( stateAtPVZ, pv->position().z() );
  
  distance( pv, stateAtPVZ, ipx, ipxsigma, 1 );
  distance( pv, stateAtPVZ, ipy, ipysigma, 2 );

  return isSuccess;

}

//====================================================================
// calculate the distance & distance error between a vertex & a track state
// error is calculated assuming the PV position & state position are uncorrelated
//====================================================================

void Velo::VeloIPResolutionMonitor::distance( const RecVertex* pv, State& state, 
                                                    double& dist, double& sigma, int type=0 )
{
  const Gaudi::XYZVector delta = pv->position() - state.position() ;
  double theta = state.slopes().theta()/rad ;
  
  Gaudi::SymMatrix3x3 covpv ( pv->covMatrix() ) ;
  const Gaudi::SymMatrix3x3 covpos( state.errPosition() );
  
  if( type==0 ){
    dist = sqrt( delta.mag2() )/mm ;
    sigma = sqrt( pow( cos(theta), 2 )*( covpv(0,0) + covpos(0,0) + covpv(1,1) + covpos(1,1) ) + 
                  pow( sin(theta), 2 )*( covpv(2,2) + covpos(2,2) ) )/mm ;
  }
  else if( type==1 ){ 
    dist = delta.x()/mm ;
    sigma = sqrt( covpv(0,0) + covpos(0,0) )/mm ;
  }
  else if( type==2 ){ 
    dist = delta.y()/mm ;
    sigma = sqrt( covpv(1,1) + covpos(1,1) )/mm ; 
  }
  else{
    dist = -999;
    sigma = -999;
  }

}

//====================================================================
// rebin histos to roughly optimal values
//====================================================================
void Velo::VeloIPResolutionMonitor::rebinHisto( TH1D* h, int nbins )
{
  int nbinsX = h->GetNbinsX() ;
  if( nbins != 0 ){
    while( (1.*nbinsX)/nbins != floor( (1.*nbinsX)/nbins ) && nbins > 1 ) nbins -= 1 ;
    if( nbins > 1 ) h->Rebin( nbins ) ;
  }
  else{
    double nentries = h->GetEntries() ;
    double idealBinWidth = nentries > 0 ? 3.*h->GetRMS() / sqrt( nentries ) : 0. ;
    nbins = (int)floor( idealBinWidth / h->GetXaxis()->GetBinWidth(1) ) ;
    while( (1.*nbinsX)/nbins != floor( (1.*nbinsX)/nbins ) && nbins > 1 ) nbins -= 1 ;
    if( nbins > 1 ) h->Rebin( nbins ) ;
  }
}

void Velo::VeloIPResolutionMonitor::rebinHistos( vector<TH1D*> histos, int nbins )
{
  for( unsigned int i=0; i<histos.size(); ++i ) rebinHisto( histos[i], nbins ) ;
}

//====================================================================
// get histos in individual bins from a th2d
//====================================================================
void Velo::VeloIPResolutionMonitor::getBinsFromTH2D( TH2D* h, string id, string title, string unit, vector<TH1D*>& out ){

	const int nbins = h->GetNbinsX() ;
  if( out.size() != (unsigned int)nbins ){
    out.clear() ;
    out.reserve( nbins ) ;
  }
  
	for( int i=0 ; i<nbins ; ++i ){
		double low = h->GetXaxis()->GetBinLowEdge( i+1 ) ;
		double high = h->GetXaxis()->GetBinUpEdge( i+1 ) ;
		ostringstream idX ;
		idX << id << "_" << low << "to" << high ;
		out[i] = h->ProjectionY( idX.str().c_str(), i+1, i+1 ) ;
		ostringstream titleX ;
		titleX << title << " for " << low << " < " << unit	<< " < " << high ;
		out[i]->SetTitle( titleX.str().c_str() ) ;
		out[i]->SetXTitle( "mm" ) ;

	}

}

//====================================================================
// save mean and sigma profiles for gaussian fits to the bins of a TH2D
//====================================================================

void Velo::VeloIPResolutionMonitor::saveMeanAndSigmaProfiles( TH2D* h )
{
  const int nbins( h->GetNbinsX() ) ;
  vector<TH1D*> binHistos(nbins) ;
  getBinsFromTH2D( h, string(h->GetName()), string(h->GetName()), string(""), binHistos ) ;
  //rebinHistos( binHistos, nbins ) ;
  
  TH1D* mean  = Aida2ROOT::aida2root( book( string(h->GetName())+string("-Mean"), string(h->GetName())+string("-Mean"), 
                                            h->GetXaxis()->GetBinLowEdge(1), h->GetXaxis()->GetBinUpEdge(nbins), nbins )) ;
  TH1D* sigma = Aida2ROOT::aida2root( book( string(h->GetName())+string("-Sigma"), string(h->GetName())+string("-Sigma"), 
                                             h->GetXaxis()->GetBinLowEdge(1), h->GetXaxis()->GetBinUpEdge(nbins), nbins )) ;

  TF1 gaus("fun", "gausn" ) ;
  
  for( int i=0 ; i<nbins ; ++i ){
    
    if( binHistos[i]->GetEntries() < 100 ){
      delete binHistos[i] ;
      continue ;
    }
    
    gaus.SetParameters( binHistos[i]->GetEntries() * binHistos[i]->GetXaxis()->GetBinWidth(1), 
                        binHistos[i]->GetMean(), binHistos[i]->GetRMS() ) ;
    double rangeMin = max( binHistos[i]->GetXaxis()->GetXmin(), 
                           binHistos[i]->GetMean() - 3. * binHistos[i]->GetRMS() ) ;
    double rangeMax = min( binHistos[i]->GetXaxis()->GetXmax(), 
                           binHistos[i]->GetMean() + 3. * binHistos[i]->GetRMS() ) ;
    gaus.SetRange( rangeMin, rangeMax ) ;
    
    int result = binHistos[i]->Fit( &gaus, "QNR" ) ;
    if( result == 0 ){
      mean->SetBinContent( i+1, gaus.GetParameter(1) ) ;
      mean->SetBinError( i+1, gaus.GetParError(1) ) ;
      sigma->SetBinContent( i+1, gaus.GetParameter(2) ) ;
      sigma->SetBinError( i+1, gaus.GetParError(2) ) ;
    }
    
    delete binHistos[i] ;
  } 
 
}

void Velo::VeloIPResolutionMonitor::saveMeanAndSigmaProfilesAndXProjection( TH2D* h, string projTitle )
{
  int nbins = h->GetNbinsX() ;
  TH1D* xProjection  = Aida2ROOT::aida2root( book( projTitle, projTitle, 
                                                   h->GetXaxis()->GetBinLowEdge(1), 
                                                   h->GetXaxis()->GetBinUpEdge(nbins), nbins ) ) ;
  TH1D* tempProjection = h->ProjectionX() ;
  for( int i=0 ; i<nbins ; ++i ){
    xProjection->SetBinContent( i+1, tempProjection->GetBinContent(i+1) ) ;
    xProjection->SetBinError( i+1, tempProjection->GetBinError(i+1) ) ;
  }
  delete tempProjection ;

  saveMeanAndSigmaProfiles( h ) ;
}

TH2D* Velo::VeloIPResolutionMonitor::book2DHisto( string name, string title, unsigned int nBinsX, 
                                             double xMin, double xMax, unsigned int nBinsY,
                                             double yMin, double yMax, bool isPersistent ) 
{
  if( !isPersistent )
    return new TH2D( name.c_str(), title.c_str(), nBinsX, xMin, xMax, nBinsY, yMin, yMax ) ;
  else
    return Aida2ROOT::aida2root( book2D( name, title, xMin, xMax, nBinsX, yMin, yMax, nBinsY ) ) ;
}
