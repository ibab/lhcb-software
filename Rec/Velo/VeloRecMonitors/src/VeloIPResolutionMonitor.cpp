// $Id: VeloIPResolutionMonitor.cpp,v 1.18 2010/04/02 16:09:45 malexand Exp $
// Include files
#include "VeloIPResolutionMonitor.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <string>
#include <sstream>

#include "Event/Track.h"

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
  DECLARE_ALGORITHM_FACTORY( VeloIPResolutionMonitor );
}


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloIPResolutionMonitor::VeloIPResolutionMonitor( const string& name,
                                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
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
  
  declareProperty("MinPVnTracks", m_minPVnTracks = 25 );
  declareProperty("MaxTrackChi2PerNDOF", m_maxTrackChi2PerNDOF = 9. ) ;
  
  // set the values of the linear parametrisation used to determine the limits of the underlying histograms
  declareProperty("ParametrisationYIntercept1D", m_limitIntercept1D = 16.e-03F );
  declareProperty("ParametrisationGradient1D", m_limitGradient1D = 26.7e-03F );
  declareProperty("HistoLimitNSigma", m_limitFactor = 10. );

  // Set whether to refit PVs without the track for which IP is being calculated
  declareProperty("RefitPVs", m_refitPVs = false );

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

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // Set histo dir
  setHistoTopDir( "Velo/" );

  // Get vertexer if required
  if( m_refitPVs ) m_pvtool = tool<IPVOfflineTool>( "PVOfflineTool" );

  // get the track extrapolator used in calculating the IPs
  m_trackExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator","Extrapolator",this);

  float limit1D = m_limitFactor * ( m_limitGradient1D * m_InversePTMax + m_limitIntercept1D );
  m_h_IPXVsInversePT = Aida2ROOT::aida2root( book2D( "IPX-Vs-InversePT-LongTracks", m_InversePTMin, m_InversePTMax, m_nBins, 
                                                     -limit1D, limit1D, 500 ) ) ;
  m_h_IPYVsInversePT = Aida2ROOT::aida2root( book2D( "IPY-Vs-InversePT-LongTracks", m_InversePTMin, m_InversePTMax, m_nBins, 
                                                     -limit1D, limit1D, 500 ) ) ;

  m_h_IPXVsPhi = Aida2ROOT::aida2root( book2D( "IPX-Vs-Phi-LongTracks", -pi, pi, m_nBinsPhi, 
                                               -1.5*limit1D, 1.5*limit1D, 500 ) ) ;
  m_h_IPYVsPhi = Aida2ROOT::aida2root( book2D( "IPY-Vs-Phi-LongTracks", -pi, pi, m_nBinsPhi, 
                                               -1.5*limit1D, 1.5*limit1D, 500 ) ) ;
  m_h_IPXVsPhi_HighPT = Aida2ROOT::aida2root( book2D( "IPX-Vs-Phi-LongTracks-PTGreaterThan1GeV", -pi, pi, m_nBinsPhi, 
                                               -limit1D, limit1D, 500 ) ) ;
  m_h_IPYVsPhi_HighPT = Aida2ROOT::aida2root( book2D( "IPY-Vs-Phi-LongTracks-PTGreaterThan1GeV", -pi, pi, m_nBinsPhi, 
                                               -limit1D, limit1D, 500 ) ) ;

  m_h_IPXVsPhi_Eta45To50 = Aida2ROOT::aida2root( book2D( "IPX-Vs-Phi-AllTracks-Eta4.5To5.0", -pi, pi, m_nBinsPhi, 
                                               -2.*limit1D, 2.*limit1D, 500 ) ) ;
  m_h_IPYVsPhi_Eta45To50 = Aida2ROOT::aida2root( book2D( "IPY-Vs-Phi-AllTracks-Eta4.5To5.0", -pi, pi, m_nBinsPhi, 
                                               -2.*limit1D, 2.*limit1D, 500 ) ) ;
  m_h_IPXVsPhi_Eta25To30 = Aida2ROOT::aida2root( book2D( "IPX-Vs-Phi-AllTracks-Eta2.5To3.0", -pi, pi, m_nBinsPhi, 
                                               -2.*limit1D, 2.*limit1D, 500 ) ) ;
  m_h_IPYVsPhi_Eta25To30 = Aida2ROOT::aida2root( book2D( "IPY-Vs-Phi-AllTracks-Eta2.5To3.0", -pi, pi, m_nBinsPhi, 
                                               -2.*limit1D, 2.*limit1D, 500 ) ) ;
  m_h_IPXVsPhi_EtaLT0 = Aida2ROOT::aida2root( book2D( "IPX-Vs-Phi-AllTracks-EtaLessThan0", -pi, pi, m_nBinsPhi, 
                                               -2.*limit1D, 2.*limit1D, 500 ) ) ;
  m_h_IPYVsPhi_EtaLT0 = Aida2ROOT::aida2root( book2D( "IPY-Vs-Phi-AllTracks-EtaLessThan0", -pi, pi, m_nBinsPhi, 
                                               -2.*limit1D, 2.*limit1D, 500 ) ) ;

  m_h_IPXVsEta = Aida2ROOT::aida2root( book2D( "IPX-Vs-Eta-LongTracks", 1.5, 5.5, m_nBinsEta, 
                                               -1.5*limit1D, 1.5*limit1D, 500 ) ) ;
  m_h_IPYVsEta = Aida2ROOT::aida2root( book2D( "IPY-Vs-Eta-LongTracks", 1.5, 5.5, m_nBinsEta, 
                                               -1.5*limit1D, 1.5*limit1D, 500 ) ) ;

  // book additional histogram of PV track multiplicity
  m_h_TrackMultiplicity = Aida2ROOT::aida2root( book( "PVTrackMultiplicity", "PV Track Multiplicity", 0.0, 150.0, 75 ));
  m_h_TrackMultiplicity->SetXTitle("Number of tracks");


  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  counter( "Events Analysed" )++;
      
  // Get PVs
  if( !exist<RecVertices>( m_vertexLocation ) ){
    counter( string("No vertices at ") + m_vertexLocation )++;
    debug() << "No vertices at " << m_vertexLocation << endmsg;
    return StatusCode::SUCCESS;
  }
  const RecVertices* pvs = get<RecVertices>( m_vertexLocation );  

  // select only events with 1 reconstructed PV
  if( pvs->size() != 1 ) return StatusCode::SUCCESS;

  // get the tracks
  if( !exist<Tracks>( m_trackLocation ) )
  {
    counter( string("No tracks at ") + m_trackLocation )++;
    return StatusCode::SUCCESS;
  }
  
  Tracks* tracks = get<Tracks>(m_trackLocation);
  
  counter("Events Selected")++;
    
  // Loop over PVs
  for ( RecVertices::const_iterator ipv = pvs->begin() ;
        ipv != pvs->end() ; ++ipv ){
    if( !(*ipv)->isPrimary() ) continue;
        
    m_pv = *ipv;
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
    if( m_refitPVs && PVtracks.size() < 3 ) continue;
    // apply ntracks cut
    if( nVeloTracks < m_minPVnTracks ) continue;
    counter("PVs Selected")++;
        
    m_h_TrackMultiplicity->Fill( nVeloTracks );    

    // loop over tracks
    for( Tracks::const_iterator tr = tracks->begin(); tr != tracks->end(); ++tr ){
      
      if( ((*tr)->type() != Track::Velo && (*tr)->type() != Track::Upstream && (*tr)->type() != Track::Long ) 
          || (*tr)->chi2()/(*tr)->nDoF() > m_maxTrackChi2PerNDOF ) continue;

      m_track = *tr;
      
      double inversePT = 1./(m_track->pt()/GeV);
      double phi = m_track->phi()/radian ;
      double eta = m_track->pseudoRapidity() ;
      
      counter("Tracks selected")++;
        
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
      calculateIPs( m_pv, m_track, ip3d, ip3dsigma, ipx, ipxsigma, ipy, ipysigma, stateAtPVZ, POCAtoPVstate );

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
      
  } // close loop over pvs

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

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
  isSuccess = distance( pv, POCAtoPVstate, ip3d, ip3dsigma, 0 );
  
  // extrapolate the current track to the same Z position as the PV to get X & Y IP
  stateAtPVZ = track->firstState();
  isSuccess = m_trackExtrapolator->propagate( stateAtPVZ, pv->position().z() );
  
  isSuccess = distance( pv, stateAtPVZ, ipx, ipxsigma, 1 );
  isSuccess = distance( pv, stateAtPVZ, ipy, ipysigma, 2 );

  return isSuccess;

}

//====================================================================
// calculate the distance & distance error between a vertex & a track state
// error is calculated assuming the PV position & state position are uncorrelated
//====================================================================

StatusCode Velo::VeloIPResolutionMonitor::distance( const RecVertex* pv, State& state, 
                                                    double& dist, double& sigma, int type=0 )
{
  const Gaudi::XYZVector delta ( pv->position() - state.position() ) ;
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

  return StatusCode::SUCCESS ;
}

