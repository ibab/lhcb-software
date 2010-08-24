// $Id: VeloIPResolutionMonitor.cpp,v 1.18 2010/04/02 16:09:45 malexand Exp $
// Include files
#include "VeloIPResolutionMonitor.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "LHCbMath/MatrixTransforms.h"

#include <string>
#include <sstream>

#include <Event/State.h>
#include "Event/Track.h"
#include "Event/L0DUReport.h"
#include "Event/ODIN.h"

#include "Event/Node.h"
#include "Event/Measurement.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/VeloRMeasurement.h"

#include <VeloDet/DeVeloPhiType.h>
#include <VeloDet/DeVeloRType.h>

#include "TFitResultPtr.h"
#include "TFitResult.h"

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
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_vertexer("TrackVertexer"),
    m_tuple(0)
{

  // set input locations
  declareProperty("TrackLocation", m_trackLocation = TrackLocation::Default ) ;
  declareProperty("VertexLocation", m_vertexLocation = RecVertexLocation::Primary );

  // set whether to write the ntuple
  declareProperty("WriteTuple",m_writeTuple=false);

  // set whether to do comparison to MC
  declareProperty("WithMC",m_withMC=false);

  // Set the binning options
  declareProperty("StatOverflows", m_statOverflows = false );
  declareProperty("UseLogScale",m_useLogScale = false );
  declareProperty("InversePTMin",m_InversePTMin = 0.0 );
  declareProperty("InversePTMax",m_InversePTMax = 3.0 );
  declareProperty("NBins",m_nBins = 20 );
  declareProperty("MinPVnTracks", m_minPVnTracks = 8 );

  // Set whether to save the underlying histograms used to make the plots of fit results
  declareProperty("SaveUnderlyingHistos",m_saveUnderlyingHistos=true);
  // set the values of the linear parametrisation used to determine the limits of the underlying histograms
  declareProperty("UnderlyingHistosLimitYIntercept1D", m_limitIntercept1D = 16.e-03F );
  declareProperty("UnderlyingHistosLimitGradient1D", m_limitGradient1D = 26.7e-03F );
  declareProperty("UnderlyingHistosLimitYIntercept3D", m_limitIntercept3D = 19.5e-03F );
  declareProperty("UnderlyingHistosLimitGradient3D", m_limitGradient3D = 30.9e-03F );
  declareProperty("UnderlyingHistosLimitFactor", m_limitFactor = 10. );

  // Set whether to calculate residuals as a fn. of eta and phi compared to the 1/PT parametrisation
  // and which gradient & y intercept values to use
  declareProperty("CalcResiduals", m_calcResiduals = true );
  declareProperty("ResidualsYIntercept3D", m_res3DyIntercept = 19.5e-03F );
  declareProperty("ResidualsLinear3D", m_res3Dgrad = 30.9e-03F );
  declareProperty("ResidualsQuad3D", m_res3Dquad = 0. );

  // Set whether to refit PVs without the track for which IP is being calculated
  declareProperty("RefitPVs", m_refitPVs = false );

  // Set fit option for output profiles
  declareProperty("FitOption", m_fitOption = "FitSingle");
  
}
//=============================================================================
// Destructor
//=============================================================================
Velo::VeloIPResolutionMonitor::~VeloIPResolutionMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitor::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if(m_writeTuple) m_tuple = nTuple( "IPResolutionsTuple" );

  // Set histo dir
  setHistoTopDir( "Velo/" );

  // Get vertexer if required
  if( m_refitPVs ) sc = m_vertexer.retrieve();
  m_pvtool =tool<IPVOfflineTool>( "PVOfflineTool" );

  // get the track extrapolator used in calculating the IPs
  m_trackExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator","Extrapolator",this);

  // if useVariableBins=false (default) and no bin values have been defined in the options file, 
  // m_nBins equal sized bins between m_InversePTMin and m_InversePTMax are used
  double binWidth ;
  if( !m_useLogScale ) {
    binWidth = (m_InversePTMax - m_InversePTMin)/(1.*m_nBins);
    for( int i = 0; i<m_nBins+1; i++ ) m_bins.push_back( i*binWidth + m_InversePTMin );
  }
  else {
    if( m_InversePTMin <= 0 ) m_InversePTMin = 0.1;
    binWidth = (log10(m_InversePTMax) - log10(m_InversePTMin))/(1.*m_nBins);
    for( int i = 0; i<m_nBins+1; i++ ) m_bins.push_back( i*binWidth + log10(m_InversePTMin) );
  }  

  // book all histograms
  for( int i = 0; i<(int)m_bins.size()-1; i++ ){

    // make histogram titles and IDs from the bin values
    ostringstream tempID;
    tempID << m_bins[i] << "to" << m_bins[i+1];
    m_histoIDs.push_back( tempID.str() );
    
    ostringstream tempTitle;
    if( !m_useLogScale ) tempTitle << m_bins[i] << " < 1/p_{T} (GeV^{-1}) < " << m_bins[i+1] ;
    else tempTitle << m_bins[i] << " < log_{10}( 1/p_{T} (GeV^{-1}) )" << m_bins[i+1] ;
    m_histoTitles.push_back( tempTitle.str() );

    float limit1D;
    float limit3D;
    if ( !m_useLogScale ){
      limit1D = m_limitFactor * ( m_limitGradient1D * (float)m_bins[ i+1 ] + m_limitIntercept1D );
      limit3D = m_limitFactor * ( m_limitGradient3D * (float)m_bins[ i+1 ] + m_limitIntercept3D );
    }
    else{
      limit1D = m_limitFactor * ( m_limitGradient1D * pow( (float)10., (float)m_bins[ i+1 ] ) + m_limitIntercept1D );
      limit3D = m_limitFactor * ( m_limitGradient3D * pow( (float)10., (float)m_bins[ i+1 ] ) + m_limitIntercept3D );
    }

    // if underlying histograms are to be saved, the histograms for each bin are booked and pointers to the underlying 
    // ROOT histograms are stored in member vectors
    if( m_saveUnderlyingHistos ){
      
      m_IPres_X_histos.push_back( Aida2ROOT::aida2root( book( "IPres_X_"+m_histoIDs[i], 
                                                              "IP_{X} for tracks with "+m_histoTitles[i], 
                                                              -limit1D*mm, limit1D*mm, 500 ) ) );
      m_IPres_Y_histos.push_back( Aida2ROOT::aida2root( book( "IPres_Y_"+m_histoIDs[i], 
                                                              "IP_{Y} for tracks with "+m_histoTitles[i], 
                                                              -limit1D*mm, limit1D*mm, 500 ) ) );
      m_IPres_unsigned3D_histos.push_back( Aida2ROOT::aida2root( book( "IPres_unsigned3D_"+m_histoIDs[i], 
                                                                       "Unsigned IP_{3D} for tracks with "
                                                                       +m_histoTitles[i],
                                                                       0.0*mm, limit3D*mm, 500 ) ) );
      m_IPres_X_histos[i]->SetXTitle("IP (mm)");
      m_IPres_Y_histos[i]->SetXTitle("IP (mm)");
      m_IPres_unsigned3D_histos[i]->SetXTitle("IP (mm)");

    }
    // otherwise the underlying histograms are created as ROOT histograms, and pointers to them stored in member vectors
    else{
      string strID = "IPres_X_"+m_histoIDs[i];
      string strTitle = "IP_{X} for tracks with "+m_histoTitles[i];
      m_IPres_X_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, -limit1D*mm, limit1D*mm ) );

      strID = "IPres_Y_"+m_histoIDs[i];
      strTitle = "IP_{Y} for tracks with "+m_histoTitles[i];
      m_IPres_Y_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, -limit1D*mm, limit1D*mm ) );

      strID = "IPres_unsigned3D_"+m_histoIDs[i];
      strTitle = "Unsigned IP_{3D} for tracks with "+m_histoTitles[i];
      m_IPres_unsigned3D_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, 0.0*mm, limit3D*mm ) );
      
    }
    
  }  

  if( m_statOverflows ){
    for( int i = 0; i<(int)m_bins.size()-1; i++ ){
      m_IPres_X_histos[i]->StatOverflows();
      m_IPres_Y_histos[i]->StatOverflows();
      m_IPres_unsigned3D_histos[i]->StatOverflows();
    }
  }
  
  // book the histograms of fit results against 1/PT using the defined bins
  string XaxisTitle;
  if( !m_useLogScale ) XaxisTitle = "1/p_{T} (GeV^{-1})";
  else XaxisTitle = "log_{10}(1/p_{T}) (GeV^{-1})";

  string XprofileTitle, YprofileTitle, threeDprofileTitle;
  if( m_fitOption == "FitDouble" ){
    XprofileTitle = "Width of core of double Gaussian fit to IP_{X} resolution Vs 1/p_{T}";
    YprofileTitle = "Width of core of double Gaussian fit to IP_{Y} resolution Vs 1/p_{T}";
    threeDprofileTitle = "Mean of core of double '2D Gaussian' fit to IP_{3D} resolution Vs 1/p_{T}";
  } 
  else if( m_fitOption =="FitSingle" ){
    XprofileTitle = "Width of Gaussian fit to IP_{X} resolution Vs 1/p_{T}";
    YprofileTitle = "Width of Gaussian fit to IP_{Y} resolution Vs 1/p_{T}";
    threeDprofileTitle = "Mean of '2D Gaussian' fit to IP_{3D} resolution Vs 1/p_{T}";
  } 
  else{
    XprofileTitle = "Sample RMS of IP_{X} resolution Vs 1/p_{T}";
    YprofileTitle = "Sample RMS of IP_{Y} resolution Vs 1/p_{T}";
    threeDprofileTitle = "Sample Mean of IP_{3D} resolution Vs 1/p_{T}";
  }
  
  m_h_GaussWidthVsInversePT_X = Aida2ROOT::aida2root( book( "GaussWidth_IP_X_Vs_InversePT", 
                                                            XprofileTitle,
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_GaussWidthVsInversePT_X->SetXTitle(XaxisTitle.c_str());
  m_h_GaussWidthVsInversePT_X->SetYTitle("mm");

  m_h_GaussWidthVsInversePT_Y = Aida2ROOT::aida2root( book( "GaussWidth_IP_Y_Vs_InversePT", 
                                                            YprofileTitle,
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_GaussWidthVsInversePT_Y->SetXTitle(XaxisTitle.c_str());
  m_h_GaussWidthVsInversePT_Y->SetYTitle("mm");
  
  m_h_MeanVsInversePT_unsigned3D = 
    Aida2ROOT::aida2root( book( "Mean_unsigned3DIP_Vs_InversePT", 
                                threeDprofileTitle,
                                m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                (int)m_bins.size() - 1 ) );
  m_h_MeanVsInversePT_unsigned3D->SetXTitle(XaxisTitle.c_str());
  m_h_MeanVsInversePT_unsigned3D->SetYTitle("mm");
  
  // book additional histograms of track multiplicity and frequency of 1/PT
  m_h_TrackMultiplicity = Aida2ROOT::aida2root( book( "TrackMultiplicity", "PV Track Multiplicity", 0.0, 150.0, 75 ));
  m_h_TrackMultiplicity->SetXTitle("Number of tracks");

  m_h_InversePTFreq = Aida2ROOT::aida2root( book( "NTracks_Vs_InversePT", "Number of tracks found in each bin of 1/p_{T}", 
                                                  m_bins[0], m_bins[ (int)m_bins.size() - 1 ], (int)m_bins.size() -1 ));
  m_h_InversePTFreq->SetXTitle(XaxisTitle.c_str());
  m_h_InversePTFreq->SetYTitle("Number of tracks");

  // book plots of residuals wrt 1/PT parametrisation against eta and phi
  if( m_calcResiduals ){
    m_p_3DphiResiduals = Aida2ROOT::aida2root( bookProfile1D( "3DphiResiduals", 
                                                              "Residuals of 3D IP res. wrt. 1/p_{T} parametrisation Vs #phi",
                                                              -pi, pi ) );
    m_p_3DphiResiduals->SetXTitle("Track #phi");
    m_p_3DphiResiduals->SetYTitle("mm");
    
    m_p_3DetaResiduals = Aida2ROOT::aida2root( bookProfile1D( "3DetaResiduals", 
                                                              "Residuals of 3D IP res. wrt. 1/p_{T} parametrisation Vs #eta",
                                                              1.5, 5.5 ) );
    m_p_3DetaResiduals->SetXTitle("Track #eta");
    m_p_3DetaResiduals->SetYTitle("mm");

  }


  // plots vs phi
  float limit1D = m_limitFactor * ( m_limitGradient1D * (float)(*(m_bins.rbegin())) + m_limitIntercept1D );

  m_h_ipXVsPhi = Aida2ROOT::aida2root( book2D( "IPXVsPhi","IP_{X} Vs #phi", -pi, pi, 32, -limit1D, limit1D, 500 ) ) ;
  m_h_ipXVsPhi->SetXTitle("#phi (rad)");
	m_h_ipXVsPhi->SetYTitle("IP_{X} (mm)");
	m_h_ipXVsPhi->Sumw2();

	m_h_ipXVsPhiSigma = Aida2ROOT::aida2root( book("IPXResVsPhi","Resolution of IP_{X} Vs #phi", -pi, pi, 32 ) );
	m_h_ipXVsPhiSigma->SetXTitle("#phi (rad)");
	m_h_ipXVsPhiSigma->SetYTitle("mm");
	m_h_ipXVsPhiSigma->Sumw2();

	m_h_ipXVsPhiMean = Aida2ROOT::aida2root( book("IPXMeanVsPhi","Mean of IP_{X} Vs #phi", -pi, pi, 32 ) );
	m_h_ipXVsPhiMean->SetXTitle("#phi (rad)");
	m_h_ipXVsPhiMean->SetYTitle("mm");
	m_h_ipXVsPhiMean->Sumw2();

  m_h_ipYVsPhi = Aida2ROOT::aida2root( book2D( "IPYVsPhi","IP_{Y} Vs #phi", -pi, pi, 32, -limit1D, limit1D, 500 ) ) ;
  m_h_ipYVsPhi->SetXTitle("#phi (rad)");
	m_h_ipYVsPhi->SetYTitle("IP_{Y} (mm)");
	m_h_ipYVsPhi->Sumw2();

	m_h_ipYVsPhiSigma = Aida2ROOT::aida2root( book("IPYResVsPhi","Resolution of IP_{Y} Vs #phi", -pi, pi, 32 ) );
	m_h_ipYVsPhiSigma->SetXTitle("#phi (rad)");
	m_h_ipYVsPhiSigma->SetYTitle("mm");
	m_h_ipYVsPhiSigma->Sumw2();

	m_h_ipYVsPhiMean = Aida2ROOT::aida2root( book("IPYMeanVsPhi","Mean of IP_{Y} Vs #phi", -pi, pi, 32 ) );
	m_h_ipYVsPhiMean->SetXTitle("#phi (rad)");
	m_h_ipYVsPhiMean->SetYTitle("mm");
	m_h_ipYVsPhiMean->Sumw2();


  // plots vs eta
  m_h_ipXVsEta = Aida2ROOT::aida2root( book2D( "IPXVsEta","IP_{X} Vs #eta", 0, 6, 50, -limit1D, limit1D, 500 ) ) ;
  m_h_ipXVsEta->SetXTitle("#eta");
	m_h_ipXVsEta->SetYTitle("IP_{X} (mm)");
	m_h_ipXVsEta->Sumw2();

	m_h_ipXVsEtaSigma = Aida2ROOT::aida2root( book("IPXResVsEta","Resolution of IP_{X} Vs #eta", 0, 6, 50 ) );
	m_h_ipXVsEtaSigma->SetXTitle("#eta");
	m_h_ipXVsEtaSigma->SetYTitle("mm");
	m_h_ipXVsEtaSigma->Sumw2();

	m_h_ipXVsEtaMean = Aida2ROOT::aida2root( book("IPXMeanVsEta","Mean of IP_{X} Vs #eta", 0, 6, 50 ) );
	m_h_ipXVsEtaMean->SetXTitle("#eta");
	m_h_ipXVsEtaMean->SetYTitle("mm");
	m_h_ipXVsEtaMean->Sumw2();

  m_h_ipYVsEta = Aida2ROOT::aida2root( book2D( "IPYVsEta","IP_{Y} Vs #eta", 0, 6, 50, -limit1D, limit1D, 500 ) ) ;
  m_h_ipYVsEta->SetXTitle("#eta");
	m_h_ipYVsEta->SetYTitle("IP_{Y} (mm)");
	m_h_ipYVsEta->Sumw2();

	m_h_ipYVsEtaSigma = Aida2ROOT::aida2root( book("IPYResVsEta","Resolution of IP_{Y} Vs #eta", 0, 6, 50 ) );
	m_h_ipYVsEtaSigma->SetXTitle("#eta");
	m_h_ipYVsEtaSigma->SetYTitle("mm");
	m_h_ipYVsEtaSigma->Sumw2();

	m_h_ipYVsEtaMean = Aida2ROOT::aida2root( book("IPYMeanVsEta","Mean of IP_{Y} Vs #eta", 0, 6, 50 ) );
	m_h_ipYVsEtaMean->SetXTitle("#eta");
	m_h_ipYVsEtaMean->SetYTitle("mm");
	m_h_ipYVsEtaMean->Sumw2();

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
    counter( string("No data at ") + m_vertexLocation )++;
    debug() << "No data at " << m_vertexLocation << endmsg;
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
    if( nVeloTracks < m_minPVnTracks && !m_writeTuple ) continue;
    counter("PVs Selected")++;
        
    m_h_TrackMultiplicity->Fill( PVtracks.size() );    

    // loop over tracks
    for( Tracks::const_iterator tr = tracks->begin(); tr != tracks->end(); ++tr ){
      
      if( (*tr)->type() != Track::Velo && (*tr)->type() != Track::Upstream && (*tr)->type() != Track::Long ) continue;      

      m_track = *tr;
      
      double inversePT;
      if( !m_useLogScale ) inversePT = 1./(m_track->pt()/GeV);
      else inversePT = log10( 1./(m_track->pt()/GeV) );

      // only select Long tracks for reliable PT measurement, unless tracks are to be written to tuple
      // Skip tracks outwith the bin range, unless tuple is to be written, then all tracks are taken
      if( (*tr)->type() != Track::Long 
          && !m_writeTuple ) continue;


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
      
      double ip3d, ip3dsigma, ipx, ipxsigma, ipy, ipysigma;
      calculateIPs( m_pv, m_track, ip3d, ip3dsigma, ipx, ipxsigma, ipy, ipysigma );

      // only make histos for Long tracks in given PT range
      if( inversePT < *(m_bins.rbegin()) && *(m_bins.begin()) < inversePT && m_track->type() == Track::Long )
      {
        
        m_h_InversePTFreq->Fill( inversePT );
          
        // fill histograms
        fillHistos( ip3d, ipx, ipy, inversePT );
          
        // fill histos of residuals of the 1/PT parametrisation as a fn. of eta and phi 
        if( m_calcResiduals ){
          if( m_useLogScale ) inversePT = pow( (double)10., inversePT );
          m_p_3DphiResiduals->Fill( m_track->phi(), 
                                    ip3d - m_res3DyIntercept - m_res3Dgrad * inversePT
                                    - m_res3Dquad * inversePT * inversePT );
          m_p_3DetaResiduals->Fill( m_track->pseudoRapidity(), 
                                    ip3d - m_res3DyIntercept - m_res3Dgrad * inversePT 
                                    - m_res3Dquad * inversePT * inversePT );
        }
      }
      if( m_track->type() == Track::Long ){
        m_h_ipXVsPhi->Fill( m_track->phi(), ipx ) ;
        m_h_ipYVsPhi->Fill( m_track->phi(), ipy ) ;
        m_h_ipXVsEta->Fill( m_track->pseudoRapidity(), ipx ) ;
        m_h_ipYVsEta->Fill( m_track->pseudoRapidity(), ipy ) ;
      }

      if( m_writeTuple ){
        
        m_tuple->column( "isInPV", isInPV );
        m_tuple->column( "TrackType", m_track->type() );
        m_tuple->column( "IsTrackForward", m_track->flag() != Track::Backward ) ;
        m_tuple->column( "IPRes3D", ip3d );
        m_tuple->column( "IPRes3Dsigma", ip3dsigma );
        m_tuple->column( "IPRes_X", ipx );
        m_tuple->column( "IPRes_Xsigma", ipxsigma );
        m_tuple->column( "IPRes_Y", ipy );
        m_tuple->column( "IPRes_Ysigma", ipysigma );
        m_tuple->column( "InversePT", inversePT );
        m_tuple->column( "P", m_track->p() );
        m_tuple->column( "Eta", m_track->pseudoRapidity() );
        m_tuple->column( "Phi", m_track->phi() );
        m_tuple->column( "TrackTx", m_track->slopes().x() );
        m_tuple->column( "TrackTy", m_track->slopes().y() );
        m_tuple->column( "TrackCharge", m_track->charge() );
        m_tuple->column( "PVX", m_pv->position().x() );
        m_tuple->column( "PVY", m_pv->position().y() );
        m_tuple->column( "PVZ", m_pv->position().z() );
        m_tuple->column( "TrackChi2", m_track->chi2() );
        m_tuple->column( "TrackNDOF", m_track->nDoF() );
        m_tuple->column( "PVNTracks", m_pv->tracks().size() );
        m_tuple->column( "PVNVeloTracks", nVeloTracks );
        m_tuple->column( "PVChi2", m_pv->chi2() );
        m_tuple->column( "PVNDOF", m_pv->nDoF() );
        m_tuple->column( "PVXerr", sqrt( m_pv->covMatrix()(0,0) ) );
        m_tuple->column( "PVYerr", sqrt( m_pv->covMatrix()(1,1) ) );
        m_tuple->column( "PVZerr", sqrt( m_pv->covMatrix()(2,2) ) );
        m_tuple->column( "PVXNoRefit", (*ipv)->position().x() );
        m_tuple->column( "PVYNoRefit", (*ipv)->position().y() );
        m_tuple->column( "PVZNoRefit", (*ipv)->position().z() );
        m_tuple->column( "PVXerrNoRefit", sqrt( (*ipv)->covMatrix()(0,0) ) );
        m_tuple->column( "PVYerrNoRefit", sqrt( (*ipv)->covMatrix()(1,1) ) );
        m_tuple->column( "PVZerrNoRefit", sqrt( (*ipv)->covMatrix()(2,2) ) );
        
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
        m_tuple->farray( "VeloStates_X", statesX, "nMeasurements", 42 );
        m_tuple->farray( "VeloStates_Y", statesY, "nMeasurements", 42 );
        m_tuple->farray( "VeloStates_Z", statesZ, "nMeasurements", 42 );
        m_tuple->farray( "MeasStationNos", stationNos, "nMeasurements", 42 );
        m_tuple->farray( "MeasSensorNos", sensorNos, "nMeasurements", 42 );
        m_tuple->farray( "MeasIsR", isR, "nMeasurements", 42 );

        vector< unsigned int > rSens ;
        vector< unsigned int > pSens ;
        
        for( vector< LHCbID >::const_iterator idit = m_track->lhcbIDs().begin() ; idit != m_track->lhcbIDs().end(); ++idit ){
          if( idit->isVeloR() ) rSens.push_back( idit->veloID().sensor() ) ;
          else if( idit->isVeloPhi() ) pSens.push_back( idit->veloID().sensor() ) ;
        }
        m_tuple->farray( "rHitSensors", rSens, "nVeloRIDs", 23 ) ;
        m_tuple->farray( "phiHitSensors", pSens, "nVeloPhiIDs", 21 ) ;

        if( m_withMC ){
          MCVertex* mcpv = new MCVertex();
          mcpv->setPosition( Gaudi::XYZPoint( -999, -999, -999 ) );
          unsigned int mctype;
          double mcInversePT;
          checkMCAssoc( m_track, m_pv, mcpv, mcInversePT, mctype );
          RecVertex dummyPV = RecVertex( mcpv->position() );
          vector<float> matrixValues( 6, 0. );
          dummyPV.setCovMatrix( Gaudi::SymMatrix3x3( matrixValues.begin(), matrixValues.end() ) );
          double mcip3d, mcip3dsigma, mcipx, mcipxsigma, mcipy, mcipysigma;
          calculateIPs( (const RecVertex*)(&dummyPV), m_track, mcip3d, mcip3dsigma, mcipx, mcipxsigma, mcipy, mcipysigma );

          m_tuple->column( "MCType", mctype );
          m_tuple->column( "MCIPRes3D", mcip3d );
          m_tuple->column( "MCIPRes3Dsigma", mcip3dsigma );
          m_tuple->column( "MCIPRes_X", mcipx );
          m_tuple->column( "MCIPRes_Xsigma", mcipxsigma );
          m_tuple->column( "MCIPRes_Y", mcipy );
          m_tuple->column( "MCIPRes_Ysigma", mcipysigma );
          m_tuple->column( "MCInversePT", mcInversePT );
          m_tuple->column( "MCPVX", mcpv->position().x() );
          m_tuple->column( "MCPVY", mcpv->position().y() );
          m_tuple->column( "MCPVZ", mcpv->position().z() );
          delete mcpv;
        }

        m_tuple->write();
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

  StatusCode sc( StatusCode::SUCCESS );

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  TH1D* m_ipXPhiHistos[32] ;
  TH1D* m_ipYPhiHistos[32] ;
  TH1D* m_ipXEtaHistos[50] ;
  TH1D* m_ipYEtaHistos[50] ;

	getBinsFromTH2D( m_h_ipXVsPhi, "IPX_phi", "IP_{X}", "#phi (rad)", &m_ipXPhiHistos[0] ) ;
  rebinHistos( m_ipXPhiHistos, 32 ) ;
	getBinsFromTH2D( m_h_ipYVsPhi, "IPY_phi", "IP_{Y}", "#phi (rad)", &m_ipYPhiHistos[0] ) ;
  rebinHistos( m_ipYPhiHistos, 32 ) ;

	getBinsFromTH2D( m_h_ipXVsEta, "IPX_eta", "IP_{X}", "#eta", &m_ipXEtaHistos[0] ) ;
  rebinHistos( m_ipXEtaHistos, 32 ) ;
	getBinsFromTH2D( m_h_ipYVsEta, "IPY_eta", "IP_{Y}", "#eta", &m_ipYEtaHistos[0] ) ;
  rebinHistos( m_ipYEtaHistos, 32 ) ;

  int option = m_fitOption == "FitDouble" ? 9 :
    m_fitOption == "FitSingle" ? 4 : 
    0 ;

  fillMeanAndProfile( m_h_ipXVsEtaMean, m_h_ipXVsEtaSigma, m_ipXEtaHistos, option ) ;
  fillMeanAndProfile( m_h_ipYVsEtaMean, m_h_ipYVsEtaSigma, m_ipYEtaHistos, option ) ;
	
  fillMeanAndProfile( m_h_ipXVsPhiMean, m_h_ipXVsPhiSigma, m_ipXPhiHistos, option ) ;
  fillMeanAndProfile( m_h_ipYVsPhiMean, m_h_ipYVsPhiSigma, m_ipYPhiHistos, option ) ;

  rebinHistos( &m_IPres_unsigned3D_histos[0], m_nBins ) ;
  rebinHistos( &m_IPres_X_histos[0], m_nBins ) ;
  rebinHistos( &m_IPres_Y_histos[0], m_nBins ) ;

  if( m_fitOption == "FitDouble" ) {
    fitDbl2DGausAndPlotMean( m_IPres_unsigned3D_histos, m_h_MeanVsInversePT_unsigned3D );
    fitDblGaussAndPlotWidth( m_IPres_X_histos, m_h_GaussWidthVsInversePT_X );    
    fitDblGaussAndPlotWidth( m_IPres_Y_histos, m_h_GaussWidthVsInversePT_Y );
  }
  else if( m_fitOption == "FitSingle" ) {
    fit2DGausAndPlotMean( m_IPres_unsigned3D_histos, m_h_MeanVsInversePT_unsigned3D );
    fitGaussAndPlotWidth( m_IPres_X_histos, m_h_GaussWidthVsInversePT_X );    
    fitGaussAndPlotWidth( m_IPres_Y_histos, m_h_GaussWidthVsInversePT_Y );
  }
  else{
    plotMean( m_IPres_unsigned3D_histos, m_h_MeanVsInversePT_unsigned3D );
    plotRMS( m_IPres_X_histos, m_h_GaussWidthVsInversePT_X );    
    plotRMS( m_IPres_Y_histos, m_h_GaussWidthVsInversePT_Y );
  }
    
  if( !m_saveUnderlyingHistos ){
    for( int i=0; i<(int)m_bins.size()-1; i++ ){
      delete m_IPres_X_histos[i];
      delete m_IPres_Y_histos[i];
      delete m_IPres_unsigned3D_histos[i];
    }
  }

  if( m_refitPVs ) sc = m_vertexer.release();
  
  return sc && GaudiTupleAlg::finalize();  // must be called after all other actions
}

//====================================================================
// Calculate ip_3D, ip_X and ip_y & their associated chi2
//====================================================================

StatusCode Velo::VeloIPResolutionMonitor::calculateIPs( const RecVertex* pv, const Track* track, 
                                                        double& ip3d, double& ip3dsigma, double& ipx, double& ipxsigma,
                                                        double& ipy, double& ipysigma ){
  
  StatusCode isSuccess;

  // extrapolate the current track to its point of closest approach to the PV 
  // to get the 3D IP
  State POCAtoPVstate;
  isSuccess = m_trackExtrapolator->propagate( *track, pv->position(), POCAtoPVstate );
  //State POCAtoPVstate( StateVector( extrapolateToPOCA( track, pv->position() ), track->slopes() ) ) ;
  isSuccess = distance( pv, POCAtoPVstate, ip3d, ip3dsigma, 0 );
  
  // extrapolate the current track to the same Z position as the PV to get X & Y IP
  State stateAtPVZ = track->firstState();
  isSuccess = m_trackExtrapolator->propagate( stateAtPVZ, pv->position().z() );
  //State stateAtPVZ( StateVector( extrapolateToZ( track, pv->position().z() ), track->slopes() ) ) ; 

  isSuccess = distance( pv, stateAtPVZ, ipx, ipxsigma, 1 );
  //isSuccess = distance( pv, POCAtoPVstate, ipx, ipxsigma, 1 );

  isSuccess = distance( pv, stateAtPVZ, ipy, ipysigma, 2 );
  //isSuccess = distance( pv, POCAtoPVstate, ipy, ipysigma, 2 );

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
  
  Gaudi::SymMatrix3x3 covpv ( pv->covMatrix() ) ;
  const Gaudi::SymMatrix3x3 covpos( state.errPosition() );
  
  if( type==0 ){
    dist = sqrt( delta.mag2() );
    sigma = sqrt( covpv(0,0) + covpos(0,0) + covpv(1,1) + covpos(1,1) + covpv(2,2) + covpos(2,2) );
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

  /*if ( !cov.Invert() ){
    chi2 = -999;
    return Error ( "Error in matrix inversion" , 905 ) ;
    }
    chi2 = Gaudi::Math::Similarity ( delta , cov ) ;*/
  

  return StatusCode::SUCCESS ;
}

//====================================================================
// fill histograms for a given set of track IPs and PT
//====================================================================
StatusCode Velo::VeloIPResolutionMonitor::fillHistos( double& ip3d, double& ipx, double& ipy, double& inversePT )
{
  // select the bin of 1/PT to which the track belongs
  for( int i = 0; i<(int)m_bins.size()-1; i++ ){
    if( m_bins[i] < inversePT && inversePT < m_bins[i+1] ){
      plotInBin( ip3d, ipx, ipy, i );
    }
  }
  
  return StatusCode::SUCCESS;
}

//====================================================================
// fill histograms in a given bin of 1/PT for the given IPs
//====================================================================
StatusCode Velo::VeloIPResolutionMonitor::plotInBin( double& ip3d, double& ipx, double& ipy, int& binNo )
{

  m_IPres_X_histos[binNo]->Fill( ipx/mm );
  m_IPres_Y_histos[binNo]->Fill( ipy/mm );
  m_IPres_unsigned3D_histos[binNo]->Fill( ip3d/mm );
  
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Take the RMS of each of a set of input histos & plot it in an output histo
//=========================================================================
StatusCode Velo::VeloIPResolutionMonitor::plotRMS ( vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making RMS histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;  
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){
    
    if( sourceHistos[i]->GetEntries() == 0 ) continue;

    outHisto->SetBinContent( i+1, sourceHistos[i]->GetRMS() );
    outHisto->SetBinError( i+1, sourceHistos[i]->GetRMSError()  );
    
  }
  
  return StatusCode::SUCCESS;
  
}

//=========================================================================
//  Fit gaussian to a set of histograms, and plot the fitted gaus width in an output histo
//=========================================================================
StatusCode Velo::VeloIPResolutionMonitor::fitGaussAndPlotWidth ( vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making Gauss fit histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;
  
  TF1* Gauss = new TF1("Gauss","gaus(0)");
  int fitResult;
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){
    
    if( sourceHistos[i]->GetEntries() == 0 ) continue;
    
    Gauss->SetParameters( sourceHistos[i]->GetMaximum(), sourceHistos[i]->GetMean(), sourceHistos[i]->GetRMS() );
    fitResult = sourceHistos[i]->Fit( Gauss, "QO" );
    if( fitResult!=0 ){
      if( msgLevel(MSG::DEBUG) ) debug() << "Fit failed for histo '" << sourceHistos[i]->GetTitle() << "'" << endmsg;} 
    else{
      outHisto->SetBinContent( i+1, fabs( Gauss->GetParameter(2) ) );
      outHisto->SetBinError( i+1, Gauss->GetParError(2) );
      if( Gauss->GetNDF()!= 0 && m_saveUnderlyingHistos ) 
        plot( Gauss->GetChisquare()/Gauss->GetNDF(), "FitChi2PerNDOF",
              "Chi^{2}/NDOF of fits performed in making final histograms", 0., 10. );
    }    
  }
  
  return StatusCode::SUCCESS;
  
}

//=========================================================================
//  Fit double gaussian to a set of histograms, and plot the fitted core gaus width in an output histo
//=========================================================================
StatusCode Velo::VeloIPResolutionMonitor::fitDblGaussAndPlotWidth ( vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making double Gauss fit histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;
  
  TF1* Gauss = new TF1("Gauss","gaus(0)+gaus(3)");
  int fitResult;
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){
    
    if( sourceHistos[i]->GetEntries() == 0 ) continue;
    
    Gauss->SetParameters( sourceHistos[i]->GetMaximum(), sourceHistos[i]->GetMean(), sourceHistos[i]->GetRMS(),
                          sourceHistos[i]->GetMaximum()/10., sourceHistos[i]->GetMean(), sourceHistos[i]->GetRMS()*10. );
    fitResult = sourceHistos[i]->Fit( Gauss, "QO" );
    if( fitResult!=0 ){
      if( msgLevel(MSG::DEBUG) ) debug() << "Fit failed for histo '" << sourceHistos[i]->GetTitle() << "'" << endmsg;} 
    else{
      if( Gauss->GetParameter(0) > Gauss->GetParameter(3) ){
        outHisto->SetBinContent( i+1, fabs( Gauss->GetParameter(2) ) );
        outHisto->SetBinError( i+1, Gauss->GetParError(2) );
      }
      else{
        outHisto->SetBinContent( i+1, fabs( Gauss->GetParameter(5) ) );
        outHisto->SetBinError( i+1, Gauss->GetParError(5) );
      }
      if( Gauss->GetNDF()!= 0 && m_saveUnderlyingHistos ) 
        plot( Gauss->GetChisquare()/Gauss->GetNDF(), "FitChi2PerNDOF",
              "Chi^{2}/NDOF of fits performed in making final histograms", 0., 10. );
    }    
  }
  
  return StatusCode::SUCCESS;
  
}

//=========================================================================
//  Fit landau to a set of histograms, and plot the fitted MPV in an output histo  
//=========================================================================
StatusCode Velo::VeloIPResolutionMonitor::fitLandauAndPlotMPV ( vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making Landau fit histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;
  
  TF1* Landau = new TF1("Landau","landau(0)");
  int fitResult;
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){

    if( sourceHistos[i]->GetEntries() == 0 ) continue;

    Landau->SetParameters( sourceHistos[i]->GetMaximum(), sourceHistos[i]->GetMean(), sourceHistos[i]->GetRMS() );
    fitResult = sourceHistos[i]->Fit( Landau, "QO" );
    if( fitResult!=0 ){
      if( msgLevel(MSG::DEBUG) ) debug() << "Fit failed for histo '" << sourceHistos[i]->GetTitle() << "'" << endmsg;} 
    else{
      outHisto->SetBinContent( i+1, fabs( Landau->GetParameter(1) ) );
      outHisto->SetBinError( i+1, Landau->GetParError(1) );
      if( Landau->GetNDF()!=0 && m_saveUnderlyingHistos ) 
        plot( Landau->GetChisquare()/Landau->GetNDF(), "FitChi2PerNDOF", 
              "Chi^{2}/NDOF of fits performed in making final histograms", 0., 10. );

    }    
  }
  
  return StatusCode::SUCCESS;
  
}

//=========================================================================
//  Fit double 2D Gaussian to a set of histograms, and plot the fitted mean of the core in an output histo  
//=========================================================================
StatusCode Velo::VeloIPResolutionMonitor::fit2DGausAndPlotMean ( vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making 2D Gaussian fit histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;
  
  TF1* Gaus2D = new TF1("Gaus2D","[0]*x*exp( (-0.5*x^2) / ([1]^2) )");
  int fitResult;
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){

    if( sourceHistos[i]->GetEntries() == 0 ) continue;

    Gaus2D->SetParameters( sourceHistos[i]->GetMaximum(), sqrt( 2./pi )*sourceHistos[i]->GetMean() );
    
    fitResult = sourceHistos[i]->Fit( Gaus2D, "QO" );
    if( fitResult!=0 ){
      if( msgLevel(MSG::DEBUG) ) debug() << "Fit failed for histo '" << sourceHistos[i]->GetTitle() << "'" << endmsg;} 
    else{
      outHisto->SetBinContent( i+1, sqrt( pi/2. )*fabs( Gaus2D->GetParameter(1) ) );
      outHisto->SetBinError( i+1, sqrt( pi/2. )*Gaus2D->GetParError(1) );
      if( Gaus2D->GetNDF()!=0 && m_saveUnderlyingHistos ) 
        plot( Gaus2D->GetChisquare()/Gaus2D->GetNDF(), "FitChi2PerNDOF", 
              "Chi^{2}/NDOF of fits performed in making final histograms", 0., 10. );
    }    
  }
  
  return StatusCode::SUCCESS;
  
}

//=========================================================================
//  Fit double 2D Gaussian to a set of histograms, and plot the fitted mean of the core in an output histo  
//=========================================================================
StatusCode Velo::VeloIPResolutionMonitor::fitDbl2DGausAndPlotMean ( vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making double 2D Gaussian fit histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;
  
  TF1* Gaus2D = new TF1("Gaus2D","[0]*x*exp( (-0.5*x^2) / ([1]^2) ) + [2]*x*exp( (-0.5*x^2) / ([3]^2) )");
  int fitResult;
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){

    if( sourceHistos[i]->GetEntries() == 0 ) continue;

    Gaus2D->SetParameters( sourceHistos[i]->GetMaximum(), sqrt( 2./pi )*sourceHistos[i]->GetMean(), 
                           sourceHistos[i]->GetMaximum()/100., sqrt( 2./pi )*sourceHistos[i]->GetMean()/10. );
    fitResult = sourceHistos[i]->Fit( Gaus2D, "QO" );
    if( fitResult!=0 ){
      if( msgLevel(MSG::DEBUG) ) debug() << "Fit failed for histo '" << sourceHistos[i]->GetTitle() << "'" << endmsg;} 
    else{
      if( Gaus2D->GetParameter(0) > Gaus2D->GetParameter(2) ){
        outHisto->SetBinContent( i+1, sqrt( pi/2. )*fabs( Gaus2D->GetParameter(1) ) );
        outHisto->SetBinError( i+1, sqrt( pi/2. )*Gaus2D->GetParError(1) );
      }
      else{
        outHisto->SetBinContent( i+1, sqrt( pi/2. )*fabs( Gaus2D->GetParameter(3) ) );
        outHisto->SetBinError( i+1, sqrt( pi/2. )*Gaus2D->GetParError(3) );
      }
      if( Gaus2D->GetNDF()!=0 && m_saveUnderlyingHistos ) 
        plot( Gaus2D->GetChisquare()/Gaus2D->GetNDF(), "FitChi2PerNDOF", 
              "Chi^{2}/NDOF of fits performed in making final histograms", 0., 10. );

    }    
  }
  
  return StatusCode::SUCCESS;
  
}

//=========================================================================
//  plot the mean of a set of input histos in an output histo  
//=========================================================================
StatusCode Velo::VeloIPResolutionMonitor::plotMean ( vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making mean histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){
    
    if( sourceHistos[i]->GetEntries() == 0 ) continue;
    
    outHisto->SetBinContent( i+1, sourceHistos[i]->GetMean() );
    outHisto->SetBinError( i+1, sourceHistos[i]->GetMeanError() );
  }
  
  return StatusCode::SUCCESS;
  
}

//====================================================================
// get mean and sigma and their errors from a given histo with different fit options
//====================================================================
std::pair< pair< Double_t, Double_t >, pair< Double_t, Double_t > > Velo::VeloIPResolutionMonitor::result1D( TH1D* h, int opt ){

  pair< pair< Double_t, Double_t >, pair< Double_t, Double_t > > 
    out( pair<Double_t,Double_t>( 0,0 ), pair<Double_t,Double_t>( 0, 0 ) ) ;

	TF1* f = h->GetFunction("fun");
	if( f ) delete f ;

	if( h->GetEntries() < 100 ) return out ;

	else if( opt==0 ){
    out.first.first = h->GetMean() ;
    out.first.second = h->GetMeanError() ;
    out.second.first = h->GetRMS() ;
    out.second.second = h->GetRMSError() ;
  }
  
	else if( opt==1 ){
		TF1 fun = TF1("fun","gaus(0)");
		fun.FixParameter(1,0);
		fun.SetParameters( h->GetEntries(), 0, h->GetRMS() );
		Int_t result = h->Fit( &fun, "QO" );
		if( result==0 ){ 
      out.first.first = 0 ;
      out.first.second = 0 ;
      out.second.first = fabs(fun.GetParameter(2)) ;
      out.second.second = fun.GetParError(2) ;
    }
	}

	else if( opt==4 ){
		TF1 fun = TF1("fun","gaus(0)");
		fun.SetParameters( h->GetEntries(), 0, h->GetRMS() );
		Int_t result = h->Fit( &fun, "QO" );
		if( result==0 ){ 
      out.first.first = fun.GetParameter(1) ;
      out.first.second = fun.GetParError(1) ;
      out.second.first = fabs(fun.GetParameter(2)) ;
      out.second.second = fun.GetParError(2) ;
    }
	}

	else if( opt==5 ){
		Double_t rms = sqrt( pow( h->GetRMS(), 2 ) + pow( h->GetMean(), 2 ) ) ;
		Double_t rmserr = rms / sqrt( h->GetEntries() ) ;
    out.first.first = h->GetMean() ;
    out.first.second = h->GetMeanError() ;
		out.second.first = rms ;
    out.second.second = rmserr ;
	}

	else if( opt==6 ){
		h->GetXaxis()->SetRangeUser( h->GetMean() - 8.*h->GetRMS(), h->GetMean() + 8.*h->GetRMS() ) ;
		h->GetXaxis()->SetRangeUser( h->GetMean() - 7.*h->GetRMS(), h->GetMean() + 7.*h->GetRMS() ) ;

		TF1 fun( "fun", "gausn(0)" ) ;
		fun.SetRange( h->GetMean() - h->GetRMS(), h->GetMean() + h->GetRMS() ) ;
		fun.SetParameters( h->GetEntries(), h->GetMean(), h->GetRMS() ) ;
		h->Fit( &fun, "QNOR" ) ;

		//h->GetXaxis()->SetRangeUser( fun.GetParameter(1) - 6.*fabs(fun.GetParameter(2)), 
		//fun.GetParameter(1) + 6.*fabs(fun.GetParameter(2)) ) ;
		//fun.SetRange( h->GetMean() - h->GetRMS(), h->GetMean() + h->GetRMS() ) ;
		fun.SetRange( fun.GetParameter(1) - 6.*fabs(fun.GetParameter(2)), fun.GetParameter(1) + 6.*fabs(fun.GetParameter(2)) ) ;
		h->Fit( &fun, "QNOR" ) ;

		//h->GetXaxis()->SetRangeUser( fun.GetParameter(1) - 5.*fabs(fun.GetParameter(2)), 
		//fun.GetParameter(1) + 5.*fabs(fun.GetParameter(2)) ) ;
		//fun.SetRange( h->GetMean() - h->GetRMS(), h->GetMean() + h->GetRMS() ) ;
		fun.SetRange( fun.GetParameter(1) - 5.*fabs(fun.GetParameter(2)), fun.GetParameter(1) + 5.*fabs(fun.GetParameter(2)) ) ;
		h->Fit( &fun, "QNOR" ) ;

		//h->GetXaxis()->SetRangeUser( fun.GetParameter(1) - 4.*fabs(fun.GetParameter(2)), 
		//fun.GetParameter(1) + 4.*fabs(fun.GetParameter(2)) ) ;
		//fun.SetRange( h->GetMean() - h->GetRMS(), h->GetMean() + h->GetRMS() ) ;
		fun.SetRange( fun.GetParameter(1) - 4.*fabs(fun.GetParameter(2)), fun.GetParameter(1) + 4.*fabs(fun.GetParameter(2)) ) ;
		h->Fit( &fun, "QNOR" ) ;

		fun.SetRange( fun.GetParameter(1) - 3.*fabs(fun.GetParameter(2)), fun.GetParameter(1) + 3.*fabs(fun.GetParameter(2)) ) ;
		int result = h->Fit( &fun, "QOR" ) ;
		if( result==0 ) {
      out.first.first = fun.GetParameter(1) ;
      out.first.second = fun.GetParError(1) ;
      out.second.first = fabs(fun.GetParameter(2)) ;
      out.second.second = fun.GetParError(2) ;
    }
	}

	else if( opt == 7 ){
		TF1 fun = TF1("fun","gausn(0)+gausn(3)+[6]" );
		TF1 gaus = TF1("gaus","gausn") ;
		gaus.SetParameters( h->GetEntries(), h->GetMean(), h->GetRMS() ) ;
		//h->Fit( &gaus, "QN" ) ;
		fun.SetParameters( h->GetEntries(), h->GetMean(), h->GetRMS()*0.1, h->GetEntries()/20. , h->GetMean(), h->GetRMS(), h->GetEntries()/1000. ) ;
		fun.SetParLimits( 0, 0, 2.*h->GetEntries() ) ;
		fun.SetParLimits( 2, 0, 2.*h->GetRMS() ) ;
		fun.SetParLimits( 3, 0, 2.*h->GetEntries() ) ;
		fun.SetParLimits( 5, 0, 2.*h->GetRMS() ) ;
		fun.SetParLimits( 6, 0, 2.*h->GetEntries() ) ;
		//fun.FixParameter( 1, gaus.GetParameter(1) ) ;
		//fun.FixParameter( 4, gaus.GetParameter(1) ) ;
		fun.FixParameter( 1, h->GetMean() ) ;
		fun.FixParameter( 4, h->GetMean() ) ;
		Int_t result = h->Fit( &fun, "QO" ) ;
    if( result ==0 ){
      out.first.first = h->GetMean() ;
      out.first.second = h->GetMeanError() ;
      if( fun.GetParameter(0) > fun.GetParameter(2) ) {
        out.second.first = fabs(fun.GetParameter(2)) ;
        out.second.second = fun.GetParError(2) ;
      }
      
      else{
        out.second.first = fabs(fun.GetParameter(5)) ;
        out.second.second = fun.GetParError(5) ;
      } 
    }
	}
	/*else if( opt == 8 ){
		TF1 fun = TF1("fun","gausn(0)+gausn(3)+[6]" );
		TF1 gaus = TF1("gaus","gausn") ;
		gaus.SetParameters( h->GetEntries(), h->GetMean(), h->GetRMS() ) ;
		//h->Fit( &gaus, "QN" ) ;
		fun.SetParameters( h->GetEntries(), h->GetMean(), h->GetRMS()*0.1, h->GetEntries()/20. , h->GetMean(), h->GetRMS(), h->GetEntries()/1000. ) ;
		fun.SetParLimits( 0, 0, 2.*h->GetEntries() ) ;
		fun.SetParLimits( 2, 0, 2.*h->GetRMS() ) ;
		fun.SetParLimits( 3, 0, 2.*h->GetEntries() ) ;
		fun.SetParLimits( 5, 0, 2.*h->GetRMS() ) ;
		fun.SetParLimits( 6, 0, 2.*h->GetEntries() ) ;
		//fun.FixParameter( 1, gaus.GetParameter(1) ) ;
		//fun.FixParameter( 4, gaus.GetParameter(1) ) ;
		fun.FixParameter( 1, h->GetMean() ) ;
		fun.FixParameter( 4, h->GetMean() ) ;
		TFitResultPtr result = h->Fit( &fun, "QOS" ) ;
		if( result == 0 ) { 
			// take weighted sum in quadrature of sigmas
			double N1 = fun.GetParameter(0) ;
			double sigma1 = fun.GetParameter( 2 ) ;
			double sigma1err = fun.GetParError( 2 ) ;

			double N2 = fun.GetParameter(3) ;
			double sigma2 = fun.GetParameter( 5 ) ;
			double sigma2err = fun.GetParError( 5 ) ;

			double cov = result->GetCovarianceMatrix()(2,5) ;

			double sigma = sqrt( ( pow( N1*sigma1, 2) + pow( N2*sigma2, 2) + 2*N1*N2*cov ) ) /
								 ( N1 + N2 ) 
								  ;
			double sigmaerr = sqrt( ( pow( N1*N1*sigma1*sigma1err, 2 ) + pow( N2*N2*sigma2*sigma2err, 2 ) )/
									( pow( N1*sigma1, 2 ) + pow( N2*sigma2, 2 ) )/
									( pow( N1, 2 ) + pow( N2, 2 ) )
									) ;
      out.first.first = h->GetMean() ;
      out.first.second = h->GetMeanError() ;
      out.second.first =  sigma ;
      out.second.second = sigmaerr ;
		}
    }*/
  
	else{
		TF1 fun = TF1("fun","gaus(0)+gaus(3)");
		fun.FixParameter(1,0);
		fun.FixParameter(4,0);
		fun.SetParameters( h->GetEntries(), 0, h->GetRMS(), h->GetEntries()/5., 0, h->GetRMS()*5 );
		Int_t result = h->Fit( &fun, "QO" );
    if( result ==0 ){
      out.first.first = 0 ;
      out.first.second = 0 ;
      if( fun.GetParameter(0) > fun.GetParameter(2) ) {
        out.second.first = fabs(fun.GetParameter(2)) ;
        out.second.second = fun.GetParError(2) ;
      }
      else{
        out.second.first = fabs(fun.GetParameter(5)) ;
        out.first.first = fun.GetParError(5) ;
      }
    }

	}

  return out ;
  
}

//====================================================================
// fill mean and sigma profiles from a set of histos in bins
//====================================================================
void Velo::VeloIPResolutionMonitor::fillMeanAndProfile( TH1D* mean, TH1D* sigma, TH1D** histos, int option ) {
	int nbins = (int)mean->GetNbinsX() ;
  
  for( int i=0; i<nbins; ++i ){
    pair< pair<Double_t,Double_t>, pair<Double_t,Double_t> > 
      result = result1D( histos[i], option ) ;
    mean->SetBinContent( i+1, result.first.first ) ;
    mean->SetBinError( i+1, result.first.second ) ;
    sigma->SetBinContent( i+1, result.second.first ) ;
    sigma->SetBinError( i+1, result.second.second ) ;
  }
  
}

//====================================================================
// get histos in individual bins from a th2d
//====================================================================
void Velo::VeloIPResolutionMonitor::getBinsFromTH2D( TH2D* h, string id, string title, string unit, TH1D** out ){

	int nbins = h->GetNbinsX() ;

	for( int i=0 ; i<nbins ; ++i ){
		float low = h->GetXaxis()->GetBinLowEdge( i+1 ) ;
		float high = h->GetXaxis()->GetBinUpEdge( i+1 ) ;
		ostringstream idX ;
		idX << id << "_" << low << "to" << high ;
		out[i] = h->ProjectionY( idX.str().c_str(), i+1, i+1 ) ;
		ostringstream titleX ;
		titleX << title << " for " << low << " < " << unit	<< " < " << high ;
		out[i]->SetTitle( titleX.str().c_str() ) ;
		out[i]->SetXTitle( "mm" ) ;
		if( -5.*out[i]->GetRMS() > out[i]->GetXaxis()->GetXmin() && 
			5.*out[i]->GetRMS() < out[i]->GetXaxis()->GetXmax() )
			out[i]->GetXaxis()->SetRangeUser( -5.*out[i]->GetRMS(), 5.*out[i]->GetRMS() ) ;

	}

}

//=========================================================================
//  Check MC association of tracks, rejecting non-prompt and/or using MC PV position if selected
//=========================================================================
StatusCode Velo::VeloIPResolutionMonitor::checkMCAssoc( const Track* track, const RecVertex* pv,
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
Gaudi::XYZPoint Velo::VeloIPResolutionMonitor::extrapolateToZ(const Track *track, double toZ)
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
Gaudi::XYZPoint Velo::VeloIPResolutionMonitor::extrapolateToPOCA(const Track *track, Gaudi::XYZPoint point )
{
  // get state parameters
  Gaudi::XYZPoint coord = track->position();
  Gaudi::XYZVector slope = track->slopes();
  Gaudi::XYZVector delta = coord - point ;
  Gaudi::XYZPoint result = coord - slope*(delta.Dot( slope )/slope.mag2() ) ;
  
  return result;
}

//====================================================================
// rebin histos to roughly optimal values
//====================================================================
void Velo::VeloIPResolutionMonitor::rebinHisto( TH1D* h, int nbins )
{
  if( nbins != 0 ){
    h->Rebin( nbins ) ;
  }
  else{
    double nentries = h->GetEntries() ;
    int nbins = nentries > 0 ? (int)floor( h->GetXaxis()->GetNbins()/sqrt( nentries )/3. ) : 0 ;
    if( nbins > 1 ) h->Rebin( nbins ) ;
  }
}

void Velo::VeloIPResolutionMonitor::rebinHistos( TH1D** histos, int nHistos, int nbins )
{
  for( int i=0; i<nHistos; ++i ) rebinHisto( histos[i], nbins ) ;
}
