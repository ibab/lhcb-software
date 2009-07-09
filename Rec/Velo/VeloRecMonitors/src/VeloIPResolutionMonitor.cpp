// $Id: VeloIPResolutionMonitor.cpp,v 1.3 2009-07-09 12:09:08 malexand Exp $
// Include files
#include "VeloIPResolutionMonitor.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiUtils/Aida2ROOT.h"

#include <string>
#include <sstream>

#include <Event/RecVertex.h>
#include <Event/State.h>
#include "Event/Track.h"
#include "Event/L0DUReport.h"

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
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty("BinValues", m_bins );
  declareProperty("UseVariableBins",m_useVariableBins = false );
  declareProperty("UseLogScale",m_useLogScale = false );
  declareProperty("InversePTMin",m_InversePTMin = 0.0 );
  declareProperty("InversePTMax",m_InversePTMax = 10.0 );
  declareProperty("NBins",m_nBins = 20 );

  // Set whether to save the underlying histograms used to make the plots of fit results
  declareProperty("SaveUnderlyingHistos",m_saveUnderlyingHistos=false);
  
  // Set whether to check if each event has passed L0
  declareProperty("RequireL0", m_requireL0 = false );

}
//=============================================================================
// Destructor
//=============================================================================
Velo::VeloIPResolutionMonitor::~VeloIPResolutionMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitor::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  setHistoTopDir( "Velo/" );

  // get the track extrapolator used in calculating the IPs
  m_trackExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator","Extrapolator",this);

  // if useVariableBins=false (default) and no bin values have been defined in the options file, 
  // m_nBins equal sized bins between m_InversePTMin and m_InversePTMax are used
  if( !m_useVariableBins && m_bins.size()==0 ){

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
    
  }
  // if useVariableBins=true but the bin values are not defined in the options file, a default set of bins is used
  else if( m_bins.size()==0 ){
    // Default bins of 1/PT in GeV-1
    double defaultBins[] = { 0.000,
                             0.125, 
                             0.250, 
                             0.375, 
                             0.500, 
                             0.625, 
                             0.750, 
                             0.875, 
                             1.000, 
                             1.125, 
                             1.250, 
                             1.425, 
                             1.600, 
                             1.800, 
                             2.000, 
                             2.250, 
                             2.500,
                             2.900, 
                             3.300, 
                             4.650, 
                             6.000,
                             8.000,
                             10.00 };
    m_bins.assign( &defaultBins[0], &defaultBins[23] );
  }    

  // book all histograms
  for( int i = 0; i<(int)m_bins.size()-1; i++ ){

    // make histogram titles and IDs from the bin values
    std::ostringstream tempID;
    tempID << m_bins[i] << "to" << m_bins[i+1];
    m_histoIDs.push_back( tempID.str() );
    
    std::ostringstream tempTitle;
    tempTitle << m_bins[i] << " < 1/PT < " << m_bins[i+1] << " (GeV^{-1})";
    m_histoTitles.push_back( tempTitle.str() );

    // if underlying histograms are to be saved, the histograms for each bin are booked and pointers to the underlying 
    // ROOT histograms are stored in member vectors
    if( m_saveUnderlyingHistos ){
      
      m_IPres_X_histos.push_back( Aida2ROOT::aida2root( book( "IPres_X_"+m_histoIDs[i], 
                                                              "Resolution of IP_X for tracks with "+m_histoTitles[i], 
                                                              -0.5/mm, 0.5/mm, 100 ) ) );
      m_IPres_Y_histos.push_back( Aida2ROOT::aida2root( book( "IPres_Y_"+m_histoIDs[i], 
                                                              "Resolution of IP_Y for tracks with "+m_histoTitles[i], 
                                                              -0.5/mm, 0.5/mm, 100 ) ) );
      m_IPres_Z_histos.push_back( Aida2ROOT::aida2root( book( "IPres_Z_"+m_histoIDs[i], 
                                                              "Resolution of IP_Z for tracks with "+m_histoTitles[i], 
                                                              -50./mm, 50./mm, 100 ) ) );
      m_IPres_unsigned3D_histos.push_back( Aida2ROOT::aida2root( book( "IPres_unsigned3D_"+m_histoIDs[i], 
                                                                "Resolution of unsigned 3D IP for tracks with "+m_histoTitles[i],
                                                                0.0/mm, 0.75/mm, 100 ) ) );
      m_IPres_signed3D_histos.push_back( Aida2ROOT::aida2root( book( "IPres_signed3D_"+m_histoIDs[i], 
                                                                "Resolution of signed 3D IP for tracks with "+m_histoTitles[i], 
                                                                -0.75/mm, 0.75/mm, 100 ) ) );

      m_IPres_signed3DPhSpCorrect_histos.push_back( Aida2ROOT::aida2root( book( "IPres_signed3DPhSpCorrect_"+m_histoIDs[i], 
                                                                           "Resolution of signed 3D IP for tracks with "
                                                                           +m_histoTitles[i]+" with phase space correction", 
                                                                           -0.75/mm, 0.75/mm, 100 ) ) );
      m_IPres_X_histos[i]->SetXTitle("IP Resolution (mm)");
      m_IPres_Y_histos[i]->SetXTitle("IP Resolution (mm)");
      m_IPres_Z_histos[i]->SetXTitle("IP Resolution (mm)");
      m_IPres_unsigned3D_histos[i]->SetXTitle("IP Resolution (mm)");
      m_IPres_signed3D_histos[i]->SetXTitle("IP Resolution (mm)");
      m_IPres_signed3DPhSpCorrect_histos[i]->SetXTitle("IP Resolution (mm)");

    }
    // otherwise the underlying histograms are created as ROOT histograms, and pointers to them stored in member vectors
    else{
      std::string strID = "IPres_X_"+m_histoIDs[i];
      std::string strTitle = "Resolution of IP_X for tracks with "+m_histoTitles[i];
      m_IPres_X_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 100, -0.5/mm, 0.5/mm ) );

      strID = "IPres_Y_"+m_histoIDs[i];
      strTitle = "Resolution of IP_Y for tracks with "+m_histoTitles[i];
      m_IPres_Y_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 100, -0.5/mm, 0.5/mm ) );

      strID = "IPres_Z_"+m_histoIDs[i];
      strTitle = "Resolution of IP_Z for tracks with "+m_histoTitles[i];
      m_IPres_Z_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 100, -50.0/mm, 50.0/mm ) );

      strID = "IPres_unsigned3D_"+m_histoIDs[i];
      strTitle = "Resolution of unsigned 3D IP for tracks with "+m_histoTitles[i];
      m_IPres_unsigned3D_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 100, 0.0/mm, 0.75/mm ) );

      strID = "IPres_signed3D_"+m_histoIDs[i];
      strTitle = "Resolution of signed 3D IP for tracks with "+m_histoTitles[i];
      m_IPres_signed3D_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 100, -0.75/mm, 0.75/mm ) );

      strID = "IPres_signed3DPhSpCorrect_"+m_histoIDs[i];
      strTitle = "Resolution of signed 3D IP for tracks with "+m_histoTitles[i]+" with phase space correction";
      m_IPres_signed3DPhSpCorrect_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 100, -0.75/mm, 0.75/mm ) );
      m_IPres_signed3DPhSpCorrect_histos[i]->Sumw2();
      
    }    
    
  }  

  // book the histograms of fit results against 1/PT using the defined bins
  char* XTitle;
  if( !m_useLogScale ) XTitle = "1/PT (GeV^{-1})";
  else XTitle = "log_{10}(1/PT) (GeV^{-1})";
  
  m_h_GaussWidthVsInversePT_X = Aida2ROOT::aida2root( book( "GaussWidth_IP_X_Vs_InversePT", 
                                                            "Width of Gaussian fit to IP_X resolution Vs 1/PT", 
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_GaussWidthVsInversePT_X->SetXTitle(XTitle);
  m_h_GaussWidthVsInversePT_X->SetYTitle("Width of Gaussian (mm)");
  if( m_useVariableBins ) m_h_GaussWidthVsInversePT_X->SetBins( (int)m_bins.size() - 1, &m_bins[0] );

  m_h_GaussWidthVsInversePT_Y = Aida2ROOT::aida2root( book( "GaussWidth_IP_Y_Vs_InversePT", 
                                                            "Width of Gaussian fit to IP_Y resolution Vs 1/PT", 
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_GaussWidthVsInversePT_Y->SetXTitle(XTitle);
  m_h_GaussWidthVsInversePT_Y->SetYTitle("Width of Gaussian (mm)");
  if( m_useVariableBins ) m_h_GaussWidthVsInversePT_Y->SetBins( (int)m_bins.size() - 1, &m_bins[0] );
  
  m_h_GaussWidthVsInversePT_Z = Aida2ROOT::aida2root( book( "GaussWidth_IP_Z_Vs_InversePT", 
                                                            "Width of Gaussian fit to IP_Z resolution Vs 1/PT", 
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_GaussWidthVsInversePT_Z->SetXTitle(XTitle);
  m_h_GaussWidthVsInversePT_Z->SetYTitle("Width of Gaussian (mm)");
  if( m_useVariableBins ) m_h_GaussWidthVsInversePT_Z->SetBins( (int)m_bins.size() - 1, &m_bins[0] );

  m_h_MPVofLandauVsInversePT_unsigned3D = Aida2ROOT::aida2root( book( "MPVofLandau_unsigned3DIP_Vs_InversePT", 
                                                               "1/PT Vs MPV of Landau fit to absolute unsigned 3D IP resolution", 
                                                               m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                               (int)m_bins.size() - 1 ) );
  m_h_MPVofLandauVsInversePT_unsigned3D->SetXTitle(XTitle);
  m_h_MPVofLandauVsInversePT_unsigned3D->SetYTitle("MPV of IP Resolution (mm)");
  if( m_useVariableBins ) m_h_MPVofLandauVsInversePT_unsigned3D->SetBins( (int)m_bins.size() - 1, &m_bins[0] );

  m_h_GaussWidthVsInversePT_signed3D = Aida2ROOT::aida2root( book( "GaussWidth_signed3DIP_Vs_InversePT",
                                                              "1/PT Vs width of Gaussian fit to absolute signed 3D IP resolution",
                                                                   m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                                   (int)m_bins.size() - 1 ) );
  m_h_GaussWidthVsInversePT_signed3D->SetXTitle(XTitle);
  m_h_GaussWidthVsInversePT_signed3D->SetYTitle("Width of Gaussian (mm)");
  if( m_useVariableBins ) m_h_GaussWidthVsInversePT_signed3D->SetBins( (int)m_bins.size() - 1, &m_bins[0] );

  m_h_GaussWidthVsInversePT_signed3DPhSpCorrect = Aida2ROOT::aida2root( book( "GaussWidth_signed3DIP_PhSpCorrect_Vs_InversePT",
                                "1/PT Vs width of Gaussian fit to signed absolute 3D IP resolution with phase space correction", 
                                                                        m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                                        (int)m_bins.size() - 1 ) );
  m_h_GaussWidthVsInversePT_signed3DPhSpCorrect->SetXTitle(XTitle);
  m_h_GaussWidthVsInversePT_signed3DPhSpCorrect->SetYTitle("Width of Gaussian (mm)");
  if( m_useVariableBins ) m_h_GaussWidthVsInversePT_signed3DPhSpCorrect->SetBins( (int)m_bins.size() - 1, &m_bins[0] );

  // book additional histograms of track multiplicity and frequency of 1/PT
  m_h_TrackMultiplicity = Aida2ROOT::aida2root( book( "TrackMultiplicity", "PV Track Multiplicity", 0.0, 150.0, 75 ));
  m_h_TrackMultiplicity->SetXTitle("Number of tracks");

  m_h_InversePTVsNTracks = Aida2ROOT::aida2root( book( "NTracks_Vs_InversePT", "Number of tracks found in each bin of 1/PT", 
                                                       m_bins[0], m_bins[ (int)m_bins.size() - 1 ], (int)m_bins.size() -1 ));
  m_h_InversePTVsNTracks->SetXTitle(XTitle);
  m_h_InversePTVsNTracks->SetYTitle("Number of tracks");
  if( m_useVariableBins ) m_h_InversePTVsNTracks->SetBins( (int)m_bins.size() - 1, &m_bins[0] );

  return StatusCode::SUCCESS;
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
        
      m_h_TrackMultiplicity->Fill( PVtracks.size() );
        
      for ( SmartRefVector< LHCb::Track >::const_iterator tr = PVtracks.begin(); 
            tr != PVtracks.end() ; tr++ ){

        // skip 'Velo' type tracks as they have no PT measurement
        if( (*tr)->type() == Track::Velo ) continue;

        double inversePT;
        if( !m_useLogScale ) inversePT = 1./((*tr)->pt()/GeV);
        else inversePT = log10( 1./((*tr)->pt()/GeV) );

        // extrapolate the current track to its point of closest approach to the PV 
        // to get the 3D IP
        LHCb::State POCAtoPVstate;
        m_trackExtrapolator->propagate( **tr, PVpos, POCAtoPVstate );
        Gaudi::XYZVector IP3D( POCAtoPVstate.position() - PVpos );      

        // extrapolate the current track to the same Z position as the PV to get X & Y IP
        Gaudi::XYZPoint positionAtPVZ;
        m_trackExtrapolator->position( **tr, PVpos.z(), positionAtPVZ );
        Gaudi::XYZVector XYIP( positionAtPVZ - PVpos );      

        // extrapolate the current track to its point of intersection with the horizontal plane
        // containing the PV to get the Z IP
        Gaudi::Plane3D horizontalPVplane( Gaudi::XYZVector(0,1,0), PVpos );
        LHCb::State horizontalIntersectState;
        m_trackExtrapolator->propagate( **tr, horizontalPVplane, horizontalIntersectState );
        double ZIP = horizontalIntersectState.position().z() - PVpos.z();

        // fill histograms
        fillHistos( IP3D, XYIP, ZIP, inversePT );
      }
    }
      
    
  }  

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloIPResolutionMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  // fit Gaussian to the histogram of X IP resolution for this bin of 1/PT and plot width of the Gaussian 
  // against 1/PT, if the fit fails, the result is not plotted
  fitGaussAndPlotWidth( m_IPres_X_histos, m_h_GaussWidthVsInversePT_X );

  // do the same for IP Y resolution
  fitGaussAndPlotWidth( m_IPres_Y_histos, m_h_GaussWidthVsInversePT_Y );

  // do the same for IP Z resolution
  fitGaussAndPlotWidth( m_IPres_Z_histos, m_h_GaussWidthVsInversePT_Z );    

  // do the same for unsigned 3D IP
  fitGaussAndPlotWidth( m_IPres_signed3D_histos, m_h_GaussWidthVsInversePT_signed3D );

  // do the same for the signed 3D IP with phase space correction
  fitGaussAndPlotWidth( m_IPres_signed3DPhSpCorrect_histos, m_h_GaussWidthVsInversePT_signed3DPhSpCorrect );

  // fit a Landau to the unsigned 3D IP resolution for this bin of 1/PT and plot the MPV of the fit
  // if the fit succeeds
  fitLandauAndPlotMPV( m_IPres_unsigned3D_histos, m_h_MPVofLandauVsInversePT_unsigned3D );

  for( int i = 0; i<(int)m_bins.size()-1; i++ ){  
    // plot the number of tracks in this bin of 1/PT
    double nEntries = m_IPres_unsigned3D_histos[i]->GetEntries();
    m_h_InversePTVsNTracks->SetBinContent( i+1, nEntries );
    m_h_InversePTVsNTracks->SetBinError( i+1, sqrt( nEntries ) );
  }
    
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


//====================================================================
// fill histograms for a given set of track IPs and PT
//====================================================================
StatusCode Velo::VeloIPResolutionMonitor::fillHistos( Gaudi::XYZVector IP3D, Gaudi::XYZVector XYIP, double ZIP, double inversePT )
{
  // select the bin of 1/PT to which the track belongs
  for( int i = 0; i<(int)m_bins.size()-1; i++ ){
    if( m_bins[i] < inversePT && inversePT < m_bins[i+1] ){
      plotInBin( IP3D, XYIP, ZIP, i );
    }
  }
  
  return StatusCode::SUCCESS;
}

//====================================================================
// fill histograms in a given bin of 1/PT for the given IPs
//====================================================================
StatusCode Velo::VeloIPResolutionMonitor::plotInBin( Gaudi::XYZVector IP3D, Gaudi::XYZVector XYIP, double ZIP, int binNo )
{

  m_IPres_X_histos[binNo]->Fill( XYIP.x()/mm );
  m_IPres_Y_histos[binNo]->Fill( XYIP.y()/mm );
  m_IPres_Z_histos[binNo]->Fill( ZIP/mm );

  m_IPres_unsigned3D_histos[binNo]->Fill( sqrt( IP3D.Mag2() )/mm );
  double signedIP3D = sqrt( IP3D.Mag2() )*IP3D.z()/fabs(IP3D.z())/mm;
  m_IPres_signed3D_histos[binNo]->Fill( signedIP3D );
  m_IPres_signed3DPhSpCorrect_histos[binNo]->Fill( signedIP3D, 1./signedIP3D/signedIP3D );
  
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
      if( Gauss->GetNDF()!= 0 && m_saveUnderlyingHistos ) plot( Gauss->GetChisquare()/Gauss->GetNDF(), "FitChi2PerNDOF",
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
      if( Landau->GetNDF()!=0 && m_saveUnderlyingHistos ) plot( Landau->GetChisquare()/Landau->GetNDF(), "FitChi2PerNDOF", 
                                                          "Chi^{2}/NDOF of fits performed in making final histograms", 0., 10. );

    }    
  }
  
  return StatusCode::SUCCESS;
  
}
