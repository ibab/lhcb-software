// $Id: VeloIPResolutionMonitor.cpp,v 1.13 2009-12-08 01:24:42 malexand Exp $
// Include files
#include "VeloIPResolutionMonitor.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <string>
#include <sstream>

#include <Event/RecVertex.h>
#include <Event/State.h>
#include "Event/Track.h"
#include "Event/L0DUReport.h"

#include "Event/Node.h"
#include "Event/Measurement.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/VeloRMeasurement.h"

#include <VeloDet/DeVeloPhiType.h>
#include <VeloDet/DeVeloRType.h>

#include <TF1.h>

using namespace LHCb;
using namespace Gaudi::Utils;
using namespace Gaudi::Units;

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
Velo::VeloIPResolutionMonitor::VeloIPResolutionMonitor( const std::string& name,
                                                        ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_vertexer("TrackVertexer"),
    m_tuple(0)
{
  // set whether to write the ntuple
  declareProperty("WriteTuple",m_writeTuple=false);

  // Set the binning options
  declareProperty("UseLogScale",m_useLogScale = false );
  declareProperty("InversePTMin",m_InversePTMin = 0.0 );
  declareProperty("InversePTMax",m_InversePTMax = 4.0 );
  declareProperty("NBins",m_nBins = 20 );

  // Set whether to save the underlying histograms used to make the plots of fit results
  declareProperty("SaveUnderlyingHistos",m_saveUnderlyingHistos=true);
  declareProperty("UnderlyingHistosLimitYIntercept1D", m_limitIntercept1D = 6.525937e-03F );
  declareProperty("UnderlyingHistosLimitGradient1D", m_limitGradient1D = 1.947187e-02F );
  declareProperty("UnderlyingHistosLimitYIntercept3D", m_limitIntercept3D = 8.524237e-03F );
  declareProperty("UnderlyingHistosLimitGradient3D", m_limitGradient3D = 2.690075e-02F );
  declareProperty("UnderlyingHistosLimitFactor", m_limitFactor = 10. );

  // Set whether to calculate residuals as a fn. of eta and phi compared to the 1/PT parametrisation
  // and which gradient & y intercept values to use
  declareProperty("CalcResiduals", m_calcResiduals = true );
  declareProperty("ResidualsYIntercept3D", m_res3DyIntercept = 8.680847e-03F );
  declareProperty("ResidualsLinear3D", m_res3Dgrad = 2.656325e-02F );
  declareProperty("ResidualsQuad3D", m_res3Dquad = 1.135474e-04F );

  // Set whether to check if each event has passed L0
  declareProperty("RequireL0", m_requireL0 = false );

  // Set whether to refit PVs without the track for which IP is being calculated
  declareProperty("RefitPVs", m_refitPVs = false );

  // Set fit options for the unsigned 3D output profile
  declareProperty("TakeMean3D",m_takeMean3D = false );
  declareProperty("FitSglGaus3D",m_fitSglGaus3D = false );
  declareProperty("FitDblGaus3D",m_fitDblGaus3D = false );
  declareProperty("FitOption3D", m_fitOption3D = "TakeMean");
  
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
    std::ostringstream tempID;
    tempID << m_bins[i] << "to" << m_bins[i+1];
    m_histoIDs.push_back( tempID.str() );
    
    std::ostringstream tempTitle;
    if( !m_useLogScale ) tempTitle << m_bins[i] << " < 1/PT < " << m_bins[i+1] << " (GeV^{-1})";
    else tempTitle << m_bins[i] << " < log_{10}(1/PT) < " << m_bins[i+1] << " (GeV^{-1})";
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
                                                              "Resolution of IP_X for tracks with "+m_histoTitles[i], 
                                                              -limit1D*mm, limit1D*mm, 500 ) ) );
      m_IPres_Y_histos.push_back( Aida2ROOT::aida2root( book( "IPres_Y_"+m_histoIDs[i], 
                                                              "Resolution of IP_Y for tracks with "+m_histoTitles[i], 
                                                              -limit1D*mm, limit1D*mm, 500 ) ) );
      m_IPres_unsigned3D_histos.push_back( Aida2ROOT::aida2root( book( "IPres_unsigned3D_"+m_histoIDs[i], 
                                                                       "Resolution of unsigned 3D IP for tracks with "
                                                                       +m_histoTitles[i],
                                                                       0.0*mm, limit3D*mm, 500 ) ) );
      m_IPres_X_histos[i]->SetXTitle("IP Resolution (mm)");
      m_IPres_Y_histos[i]->SetXTitle("IP Resolution (mm)");
      m_IPres_unsigned3D_histos[i]->SetXTitle("IP Resolution (mm)");

    }
    // otherwise the underlying histograms are created as ROOT histograms, and pointers to them stored in member vectors
    else{
      std::string strID = "IPres_X_"+m_histoIDs[i];
      std::string strTitle = "Resolution of IP_X for tracks with "+m_histoTitles[i];
      m_IPres_X_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, -limit1D*mm, limit1D*mm ) );

      strID = "IPres_Y_"+m_histoIDs[i];
      strTitle = "Resolution of IP_Y for tracks with "+m_histoTitles[i];
      m_IPres_Y_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, -limit1D*mm, limit1D*mm ) );

      strID = "IPres_unsigned3D_"+m_histoIDs[i];
      strTitle = "Resolution of unsigned 3D IP for tracks with "+m_histoTitles[i];
      m_IPres_unsigned3D_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, 0.0*mm, limit3D*mm ) );
      
    }
    
  }  

  // book the histograms of fit results against 1/PT using the defined bins
  std::string XTitle;
  if( !m_useLogScale ) XTitle = "1/PT (GeV^{-1})";
  else XTitle = "log_{10}(1/PT) (GeV^{-1})";
  
  m_h_GaussWidthVsInversePT_X = Aida2ROOT::aida2root( book( "GaussWidth_IP_X_Vs_InversePT", 
                                                            "Width of Gaussian fit to IP_X resolution Vs 1/PT", 
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_GaussWidthVsInversePT_X->SetXTitle(XTitle.c_str());
  m_h_GaussWidthVsInversePT_X->SetYTitle("Width of Gaussian (mm)");

  m_h_GaussWidthVsInversePT_Y = Aida2ROOT::aida2root( book( "GaussWidth_IP_Y_Vs_InversePT", 
                                                            "Width of Gaussian fit to IP_Y resolution Vs 1/PT", 
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_GaussWidthVsInversePT_Y->SetXTitle(XTitle.c_str());
  m_h_GaussWidthVsInversePT_Y->SetYTitle("Width of Gaussian (mm)");  

  if( m_fitOption3D == "TakeMean" || m_takeMean3D ){
    std::string id;
    if( m_fitOption3D == "TakeMean" ) id = "Mean_unsigned3DIP_Vs_InversePT";
    else id = "SampleMean_unsigned3DIP_Vs_InversePT";
    m_h_SampleMeanVsInversePT_unsigned3D = 
      Aida2ROOT::aida2root( book( id, 
                                  "Sample Mean of absolute unsigned 3D IP resolution Vs 1/PT",
                                  m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                  (int)m_bins.size() - 1 ) );
    m_h_SampleMeanVsInversePT_unsigned3D->SetXTitle(XTitle.c_str());
    m_h_SampleMeanVsInversePT_unsigned3D->SetYTitle("<IP_3D> (mm)");
  }
  
  if( m_fitOption3D == "FitSglGaus" || m_fitSglGaus3D ){
    std::string id;
    if( m_fitOption3D == "FitSglGaus" ) id = "Mean_unsigned3DIP_Vs_InversePT";
    else id = "SglGausMean_unsigned3DIP_Vs_InversePT";
    m_h_SglGausMeanVsInversePT_unsigned3D = 
      Aida2ROOT::aida2root( book( id, 
                                  "Mean of single 2D Gaussian fit to absolute unsigned 3D IP resolution Vs 1/PT",
                                  m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                  (int)m_bins.size() - 1 ) );
    m_h_SglGausMeanVsInversePT_unsigned3D->SetXTitle(XTitle.c_str());
    m_h_SglGausMeanVsInversePT_unsigned3D->SetYTitle("<IP_3D> (mm)");
  }

  if( m_fitOption3D == "FitDblGaus" || m_fitDblGaus3D ){
    std::string id;
    if( m_fitOption3D == "FitDblGaus" ) id = "Mean_unsigned3DIP_Vs_InversePT";
    else id = "DblGausMean_unsigned3DIP_Vs_InversePT";
    m_h_DblGausMeanVsInversePT_unsigned3D = 
      Aida2ROOT::aida2root( book( id, 
                                  "Mean of core of double 2D Gaussian fit to absolute unsigned 3D IP resolution Vs 1/PT",
                                  m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                  (int)m_bins.size() - 1 ) );
    m_h_DblGausMeanVsInversePT_unsigned3D->SetXTitle(XTitle.c_str());
    m_h_DblGausMeanVsInversePT_unsigned3D->SetYTitle("<IP_3D> (mm)");
  }

  if( m_fitOption3D == "TakeMean" ) m_h_MeanVsInversePT_unsigned3D = m_h_SampleMeanVsInversePT_unsigned3D;
  else if ( m_fitOption3D == "FitSglGaus" ) m_h_MeanVsInversePT_unsigned3D = m_h_SglGausMeanVsInversePT_unsigned3D;
  else if ( m_fitOption3D == "FitDblGaus" ) m_h_MeanVsInversePT_unsigned3D = m_h_DblGausMeanVsInversePT_unsigned3D;
  else{
    Warning( "'FitOption3D' parameter must take values 'TakeMean', 'FitSglGaus', or 'FitDblGaus'" );
    return StatusCode::FAILURE;
  }

  // book additional histograms of track multiplicity and frequency of 1/PT
  m_h_TrackMultiplicity = Aida2ROOT::aida2root( book( "TrackMultiplicity", "PV Track Multiplicity", 0.0, 150.0, 75 ));
  m_h_TrackMultiplicity->SetXTitle("Number of tracks");

  m_h_InversePTFreq = Aida2ROOT::aida2root( book( "NTracks_Vs_InversePT", "Number of tracks found in each bin of 1/PT", 
                                                       m_bins[0], m_bins[ (int)m_bins.size() - 1 ], (int)m_bins.size() -1 ));
  m_h_InversePTFreq->SetXTitle(XTitle.c_str());
  m_h_InversePTFreq->SetYTitle("Number of tracks");

  // book plots of residuals wrt 1/PT parametrisation against eta and phi
  if( m_calcResiduals ){
    m_p_3DphiResiduals = Aida2ROOT::aida2root( bookProfile1D( "3DphiResiduals", 
                                                              "Residuals of 3D IP res. wrt. 1/PT parametrisation Vs Phi",
                                                              -pi, pi ) );
    m_p_3DphiResiduals->SetXTitle("Track phi");
    m_p_3DphiResiduals->SetYTitle("Residuals (mm)");
    
    m_p_3DetaResiduals = Aida2ROOT::aida2root( bookProfile1D( "3DetaResiduals", 
                                                              "Residuals of 3D IP res. wrt. 1/PT parametrisation Vs Eta",
                                                              1.5, 5.5 ) );
    m_p_3DetaResiduals->SetXTitle("Track eta");
    m_p_3DetaResiduals->SetYTitle("Residuals (mm)");

  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get L0DU report to check L0 decision, if required
  LHCb::L0DUReport* report = new L0DUReport();
  if( m_requireL0 && exist<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default) ){
    report = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
  }
  else if( m_requireL0 ) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "Could not find L0DU report!" << endmsg;
    return StatusCode::FAILURE;
  }

  if( msgLevel(MSG::DEBUG)) info() << "L0 decision: " << report->decision() << endmsg;
    
  if( !m_requireL0 || report->decision() ){
      
    // Get PVs
    if( !exist<LHCb::RecVertices>(LHCb::RecVertexLocation::Primary) ){
      counter("No data at 'Rec/Vertex/Primary'")++;
      debug() << "No data at 'Rec/Vertex/Primary'!" << endmsg;
      return StatusCode::SUCCESS;
    }
    const LHCb::RecVertices* pvs = get<LHCb::RecVertices>(LHCb::RecVertexLocation::Primary);

    counter("Events Selected")++;
    
    // Loop over PVs
    for ( LHCb::RecVertices::const_iterator ipv = pvs->begin() ;
          ipv != pvs->end() ; ++ipv ){
      if( !(*ipv)->isPrimary() ) continue;
        
      const LHCb::RecVertex* currentPV = *ipv;
      Gaudi::XYZPoint PVpos = currentPV->position();
        
      // Get tracks from current PV & loop
      const SmartRefVector< LHCb::Track > & PVtracks = currentPV->tracks();

      // can't refit a PV with only one track!
      if( m_refitPVs && PVtracks.size() < 3 ) continue;
        
      m_h_TrackMultiplicity->Fill( PVtracks.size() );
        
      for ( SmartRefVector< LHCb::Track >::const_iterator tr = PVtracks.begin(); 
            tr != PVtracks.end() ; tr++ ){

        // skip 'Velo' type tracks as they have no PT measurement, unless tracks are to be written to tuple
        if( (*tr)->type() == Track::Velo && !m_writeTuple ) continue;

        const LHCb::Track* track = &(**tr);

        double inversePT;
        if( !m_useLogScale ) inversePT = 1./(track->pt()/GeV);
        else inversePT = log10( 1./(track->pt()/GeV) );

        // Skip tracks outwith the bin range, unless tuple is to be written, then all tracks are taken
        if( (inversePT > *(m_bins.rbegin()) || *(m_bins.begin()) > inversePT) && !m_writeTuple ) continue;

        counter("Tracks selected")++;
        
        // refit PV removing current track
        if( m_refitPVs ){
          
          std::vector< const LHCb::Track* > newTracks;
          for ( SmartRefVector< LHCb::Track >::const_iterator trackIt = PVtracks.begin(); 
                trackIt != PVtracks.end() ; trackIt++ ){
            
            if( *trackIt == *tr ) continue;
            newTracks.push_back( &(**trackIt) );
          }
          
          LHCb::RecVertex* newVertex  = m_vertexer->fit( newTracks );
          if( newVertex ){
            PVpos = newVertex->position();
            delete newVertex;
          }
          else continue;
        }

        // extrapolate the current track to its point of closest approach to the PV 
        // to get the 3D IP
        LHCb::State POCAtoPVstate;
        m_trackExtrapolator->propagate( *track, PVpos, POCAtoPVstate );
        Gaudi::XYZVector IP3D( POCAtoPVstate.position() - PVpos );

        // extrapolate the current track to the same Z position as the PV to get X & Y IP
        Gaudi::XYZPoint positionAtPVZ;
        m_trackExtrapolator->position( *track, PVpos.z(), positionAtPVZ );
        Gaudi::XYZVector XYIP( positionAtPVZ - PVpos );

        // only make histos for tracks in given PT range
        if( inversePT > *(m_bins.rbegin()) || *(m_bins.begin()) > inversePT || track->type() != LHCb::Track::Velo )
        {

          m_h_InversePTFreq->Fill( inversePT );
          
          // fill histograms
          fillHistos( IP3D, XYIP, inversePT );
          
          // fill histos of residuals of the 1/PT parametrisation as a fn. of eta and phi 
          if( m_calcResiduals ){
            if( m_useLogScale ) inversePT = pow( (double)10., inversePT );
            m_p_3DphiResiduals->Fill( track->phi(), sqrt( IP3D.mag2() ) - m_res3DyIntercept - m_res3Dgrad * inversePT
                                      - m_res3Dquad * inversePT * inversePT );
            m_p_3DetaResiduals->Fill( track->pseudoRapidity(), sqrt( IP3D.mag2() ) - m_res3DyIntercept - m_res3Dgrad * inversePT 
                                      - m_res3Dquad * inversePT * inversePT );
          }
        }

        if( m_writeTuple ){
          m_tuple->column( "TrackType", track->type() );
          m_tuple->column( "IPRes3D", sqrt( IP3D.mag2() ) );
          m_tuple->column( "IPRes_X", XYIP.x() );
          m_tuple->column( "IPRes_Y", XYIP.y() );
          m_tuple->column( "InversePT", inversePT );
          m_tuple->column( "P", track->p() );
          m_tuple->column( "Eta", track->pseudoRapidity() );
          m_tuple->column( "Phi", track->phi() );
          m_tuple->column( "TrackTx", track->slopes().x() );
          m_tuple->column( "TrackTy", track->slopes().y() );
          m_tuple->column( "TrackCharge", track->charge() );
          m_tuple->column( "PVX", PVpos.x() );
          m_tuple->column( "PVY", PVpos.y() );
          m_tuple->column( "PVZ", PVpos.z() );
          m_tuple->column( "TrackChi2", track->chi2() );
          m_tuple->column( "TrackNDOF", track->nDoF() );
          m_tuple->column( "PVNTracks", currentPV->tracks().size() );
          m_tuple->column( "PVChi2", currentPV->chi2() );
          m_tuple->column( "PVNDOF", currentPV->nDoF() );
          m_tuple->column( "PVXerr", sqrt( currentPV->covMatrix()(0,0) ) );
          m_tuple->column( "PVYerr", sqrt( currentPV->covMatrix()(1,1) ) );
          m_tuple->column( "PVZerr", sqrt( currentPV->covMatrix()(2,2) ) );
          std::vector<double> statesX;
          std::vector<double> statesY;
          std::vector<double> statesZ;
          std::vector<unsigned int> stationNos;
          std::vector<bool> isR;
          std::vector<unsigned int> sensorNos;
          for( LHCb::Track::ConstNodeRange::const_iterator inode = track->nodes().begin();
               inode != track->nodes().end(); ++inode){
            
            if( (*inode)->type() != LHCb::Node::HitOnTrack ) continue;
            
            if( (*inode)->measurement().lhcbID().isVelo() ){
              
              statesX.push_back( (*inode)->state().position().x() );
              statesY.push_back( (*inode)->state().position().y() );
              statesZ.push_back( (*inode)->state().position().z() );
              if( (*inode)->measurement().type() == LHCb::Measurement::VeloPhi ){
                stationNos.push_back( ((LHCb::VeloPhiMeasurement&)((*inode)->measurement())).sensor().station() );
                sensorNos.push_back( ((LHCb::VeloPhiMeasurement&)((*inode)->measurement())).sensor().sensorNumber() );
                isR.push_back( false );
              }
              else{
                stationNos.push_back( ((LHCb::VeloRMeasurement&)((*inode)->measurement())).sensor().station() );
                sensorNos.push_back( ((LHCb::VeloRMeasurement&)((*inode)->measurement())).sensor().sensorNumber() );
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
          m_tuple->write();
        }
        
      }
    }
  }  

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitor::finalize() {

  StatusCode sc( StatusCode::SUCCESS );

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  // fit Gaussian to the histogram of X IP resolution for this bin of 1/PT and plot width of the Gaussian 
  // against 1/PT, if the fit fails, the result is not plotted
  fitGaussAndPlotWidth( m_IPres_X_histos, m_h_GaussWidthVsInversePT_X );

  // do the same for IP Y resolution
  fitGaussAndPlotWidth( m_IPres_Y_histos, m_h_GaussWidthVsInversePT_Y );

  // use selected fit method for IP3D distributions
  if( m_fitOption3D == "TakeMean" || m_takeMean3D ) 
    plotMean( m_IPres_unsigned3D_histos, m_h_SampleMeanVsInversePT_unsigned3D );
  if( m_fitOption3D == "FitSglGaus" || m_fitSglGaus3D ) 
    fit2DGausAndPlotMean( m_IPres_unsigned3D_histos, m_h_SglGausMeanVsInversePT_unsigned3D );
  if( m_fitOption3D == "FitDblGaus" || m_fitDblGaus3D ) 
    fitDbl2DGausAndPlotMean( m_IPres_unsigned3D_histos, m_h_DblGausMeanVsInversePT_unsigned3D );
    
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
// fill histograms for a given set of track IPs and PT
//====================================================================
StatusCode Velo::VeloIPResolutionMonitor::fillHistos( Gaudi::XYZVector IP3D, 
                                                      Gaudi::XYZVector XYIP, double inversePT )
{
  // select the bin of 1/PT to which the track belongs
  for( int i = 0; i<(int)m_bins.size()-1; i++ ){
    if( m_bins[i] < inversePT && inversePT < m_bins[i+1] ){
      plotInBin( IP3D, XYIP, i );
    }
  }
  
  return StatusCode::SUCCESS;
}

//====================================================================
// fill histograms in a given bin of 1/PT for the given IPs
//====================================================================
StatusCode Velo::VeloIPResolutionMonitor::plotInBin( Gaudi::XYZVector IP3D, 
                                                     Gaudi::XYZVector XYIP, int binNo )
{

  m_IPres_X_histos[binNo]->Fill( XYIP.x()/mm );
  m_IPres_Y_histos[binNo]->Fill( XYIP.y()/mm );
  m_IPres_unsigned3D_histos[binNo]->Fill( sqrt( IP3D.Mag2() )/mm );
  
  return StatusCode::SUCCESS;
}


//=========================================================================
//  Fit gaussian to a set of histograms, and plot the fitted gaus width in an output histo
//=========================================================================
StatusCode Velo::VeloIPResolutionMonitor::fitGaussAndPlotWidth ( std::vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making Gauss fit histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;
  
  TF1* Gauss = new TF1("Gauss","gaus(0)");
  int fitResult;
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){
    
    if( sourceHistos[i]->GetEntries() == 0 ) continue;
    
    Gauss->SetParameters( sourceHistos[i]->GetMaximum(), sourceHistos[i]->GetMean(), sourceHistos[i]->GetRMS() );
    fitResult = sourceHistos[i]->Fit( Gauss, "QN" );
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
//  Fit landau to a set of histograms, and plot the fitted MPV in an output histo  
//=========================================================================
StatusCode Velo::VeloIPResolutionMonitor::fitLandauAndPlotMPV ( std::vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making Landau fit histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;
  
  TF1* Landau = new TF1("Landau","landau(0)");
  int fitResult;
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){

    if( sourceHistos[i]->GetEntries() == 0 ) continue;

    Landau->SetParameters( sourceHistos[i]->GetMaximum(), sourceHistos[i]->GetMean(), sourceHistos[i]->GetRMS() );
    fitResult = sourceHistos[i]->Fit( Landau, "QN" );
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
StatusCode Velo::VeloIPResolutionMonitor::fit2DGausAndPlotMean ( std::vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making 2D Gaussian fit histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;
  
  TF1* Gaus2D = new TF1("Gaus2D","[0]*x*exp( (-0.5*x^2) / ([1]^2) )");
  int fitResult;
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){

    if( sourceHistos[i]->GetEntries() == 0 ) continue;

    Gaus2D->SetParameters( sourceHistos[i]->GetMaximum(), sqrt( 2./pi )*sourceHistos[i]->GetMean() );
    
    fitResult = sourceHistos[i]->Fit( Gaus2D, "QN" );
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
StatusCode Velo::VeloIPResolutionMonitor::fitDbl2DGausAndPlotMean ( std::vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making double 2D Gaussian fit histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;
  
  TF1* Gaus2D = new TF1("Gaus2D","[0]*x*exp( (-0.5*x^2) / ([1]^2) ) + [2]*x*exp( (-0.5*x^2) / ([3]^2) )");
  int fitResult;
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){

    if( sourceHistos[i]->GetEntries() == 0 ) continue;

    Gaus2D->SetParameters( sourceHistos[i]->GetMaximum(), sqrt( 2./pi )*sourceHistos[i]->GetMean(), 
                           sourceHistos[i]->GetMaximum()/100., sqrt( 2./pi )*sourceHistos[i]->GetMean()/10. );
    fitResult = sourceHistos[i]->Fit( Gaus2D, "QN" );
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
StatusCode Velo::VeloIPResolutionMonitor::plotMean ( std::vector< TH1D* > sourceHistos, TH1D* outHisto ) {

  debug() << "Making mean histo" << endmsg;
  
  if( (int)sourceHistos.size() != outHisto->GetNbinsX() ) return StatusCode::SUCCESS;
  
  for( int i=0; i<(int)sourceHistos.size(); i++ ){
    
    if( sourceHistos[i]->GetEntries() == 0 ) continue;
    
    outHisto->SetBinContent( i+1, sourceHistos[i]->GetMean() );
    outHisto->SetBinError( i+1, sourceHistos[i]->GetMeanError() );
  }
  
  return StatusCode::SUCCESS;
  
}
