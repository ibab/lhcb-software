// $Id: VeloIPResolutionMonitor.cpp,v 1.1 2009-06-16 11:50:24 malexand Exp $
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

    double binWidth = (m_InversePTMax - m_InversePTMin)/(1.*m_nBins);
    for( int i = 0; i<m_nBins+1; i++ ) m_bins.push_back( i*binWidth );

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
    tempTitle << m_bins[i] << " < 1/PT < " << m_bins[i+1] << " (GeV^-1)";
    m_histoTitles.push_back( tempTitle.str() );

    // if underlying histograms are to be saved, the histograms for each bin are booked and pointers to the underlying 
    // ROOT histograms are stored in member vectors
    if( m_saveUnderlyingHistos ){
      
      m_IPres_X_histos.push_back( Aida2ROOT::aida2root( book( "IPres_X_"+m_histoIDs[i], 
                                                              "Resolution of IP_X for tracks with "+m_histoTitles[i], 
                                                              -0.5, 0.5, 100 ) ) );
      m_IPres_Y_histos.push_back( Aida2ROOT::aida2root( book( "IPres_Y_"+m_histoIDs[i], 
                                                              "Resolution of IP_Y for tracks with "+m_histoTitles[i], 
                                                              -0.5, 0.5, 100 ) ) );
      m_IPres_Z_histos.push_back( Aida2ROOT::aida2root( book( "IPres_Z_"+m_histoIDs[i], 
                                                              "Resolution of IP_Z for tracks with "+m_histoTitles[i], 
                                                              -150.0, 150.0, 100 ) ) );
      m_IPres_abs_histos.push_back( Aida2ROOT::aida2root( book( "IPres_abs_"+m_histoIDs[i], 
                                                                "Resolution of absolute IP for tracks with "+m_histoTitles[i], 
                                                                0.0, 0.75, 100 ) ) );
    }
    // otherwise the underlying histograms are created as ROOT histograms, and pointers to them stored in member vectors
    else{
      std::string strID = "IPres_X_"+m_histoIDs[i];
      std::string strTitle = "Resolution of IP_X for tracks with "+m_histoTitles[i];
      m_IPres_X_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 100, -0.5, 0.5 ) );

      strID = "IPres_Y_"+m_histoIDs[i];
      strTitle = "Resolution of IP_Y for tracks with "+m_histoTitles[i];
      m_IPres_Y_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 100, -0.5, 0.5 ) );

      strID = "IPres_Z_"+m_histoIDs[i];
      strTitle = "Resolution of IP_Z for tracks with "+m_histoTitles[i];
      m_IPres_Z_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 100, -150.0, 150.0 ) );

      strID = "IPres_abs_"+m_histoIDs[i];
      strTitle = "Resolution of absolute IP for tracks with "+m_histoTitles[i];
      m_IPres_abs_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 100, 0.0, 0.75 ) );
    }
    
    m_IPres_X_histos[i]->SetXTitle("IP Resolution (mm)");
    m_IPres_Y_histos[i]->SetXTitle("IP Resolution (mm)");
    m_IPres_Z_histos[i]->SetXTitle("IP Resolution (mm)");
    m_IPres_abs_histos[i]->SetXTitle("IP Resolution (mm)");
    
  }  

  // book the histograms of fit results against 1/PT using the defined bins
  const double* bins = &m_bins[0];

  m_h_InversePTVsGaussWidth_X = Aida2ROOT::aida2root( book( "InversePTVsGaussWidth_X", 
                                                            "Width of Gaussian fit to IP_X resolution Vs 1/PT", 
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_InversePTVsGaussWidth_X->SetXTitle("1/PT (GeV^-1)");
  m_h_InversePTVsGaussWidth_X->SetYTitle("Width of Gaussian (mm)");
  m_h_InversePTVsGaussWidth_X->SetBins( (int)m_bins.size() - 1, bins );

  m_h_InversePTVsGaussWidth_Y = Aida2ROOT::aida2root( book( "InversePTVsGaussWidth_Y", 
                                                            "Width of Gaussian fit to IP_Y resolution Vs 1/PT", 
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_InversePTVsGaussWidth_Y->SetXTitle("1/PT (GeV^-1)");
  m_h_InversePTVsGaussWidth_Y->SetYTitle("Width of Gaussian (mm)");
  m_h_InversePTVsGaussWidth_Y->SetBins( (int)m_bins.size() - 1, bins );
  
  m_h_InversePTVsGaussWidth_Z = Aida2ROOT::aida2root( book( "InversePTVsGaussWidth_Z", 
                                                            "Width of Gaussian fit to IP_Z resolution Vs 1/PT", 
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_InversePTVsGaussWidth_Z->SetXTitle("1/PT (GeV^-1)");
  m_h_InversePTVsGaussWidth_Z->SetYTitle("Width of Gaussian (mm)");
  m_h_InversePTVsGaussWidth_Z->SetBins( (int)m_bins.size() - 1, bins );

  m_h_InversePTVsMPVofLandau_abs = Aida2ROOT::aida2root( book( "InversePTVsMPVofLandau_abs", 
                                                               "1/PT Vs MPV of Landau fit to absolute IP resolution", 
                                                               m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                               (int)m_bins.size() - 1 ) );
  m_h_InversePTVsMPVofLandau_abs->SetXTitle("1/PT (GeV^-1)");
  m_h_InversePTVsMPVofLandau_abs->SetYTitle("MPV of IP Resolution (mm)");
  m_h_InversePTVsMPVofLandau_abs->SetBins( (int)m_bins.size() - 1, bins );

  // book additional histograms of track multiplicity and frequency of 1/PT
  m_h_TrackMultiplicity = Aida2ROOT::aida2root( book( "TrackMultiplicity", "PV Track Multiplicity", 0.0, 150.0, 100 ));
  m_h_TrackMultiplicity->SetXTitle("Number of tracks");

  m_h_InversePTVsNTracks = Aida2ROOT::aida2root( book( "InversePTVsNTracks", "Number of tracks found in each bin of 1/PT", 
                                                       m_bins[0], m_bins[ (int)m_bins.size() - 1 ], (int)m_bins.size() -1 ));
  m_h_InversePTVsNTracks->SetXTitle("1/PT (GeV^-1)");
  m_h_InversePTVsNTracks->SetYTitle("Number of tracks");
  m_h_InversePTVsNTracks->SetBins( (int)m_bins.size() - 1, bins );

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
    Warning( "Could not find L0DU report!" );
    return StatusCode::FAILURE;
  }

  if( msgLevel(MSG::DEBUG)) info() << "L0 decision: " << report->decision() << endmsg;
    
  if( !m_requireL0 || report->decision() ){
      
    // Get PVs
    const LHCb::RecVertices* pvs = get<LHCb::RecVertices>(LHCb::RecVertexLocation::Primary);
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

        double PT = (*tr)->pt()/Gaudi::Units::GeV;

        // extrapolate the current track to its point of closest approach to the PV 
        // to get the absolute 3D IP
        LHCb::State POCAtoPVstate;
        m_trackExtrapolator->propagate( **tr, PVpos, POCAtoPVstate );
        Gaudi::XYZVector absIP( POCAtoPVstate.position() - PVpos );      

        // extrapolate the current track to the same Z position as the PV to get X & Y IP
        Gaudi::XYZPoint positionAtPVZ;
        m_trackExtrapolator->position( **tr, PVpos.z(), positionAtPVZ );
        Gaudi::XYZVector XYIP( positionAtPVZ - PVpos );      

        // extrapolate the current track to its point of intersection with the horizontal plane
        // containing the PV to get the Z IP
        Gaudi::Plane3D horizontalPVplane( Gaudi::XYZVector(0,1,0), PVpos );
        LHCb::State horizontalIntersectState;
        m_trackExtrapolator->propagate( **tr, horizontalPVplane, horizontalIntersectState );
        double ZIP = horizontalIntersectState.position().z();

        // fill histograms
        fillHistos( absIP, XYIP, ZIP, PT );
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

  // declare TFormulae to fit to the relevant histograms
  TF1* Gauss = new TF1("Gauss","gaus(0)");
  TF1* Landau = new TF1("Landau","landau(0)");
  int fitResult;

  // loop over the bins of 1/PT
  for( int i = 0; i<(int)m_bins.size()-1; i++ ){

    // fit Gaussian to the histogram of X IP resolution for this bin of 1/PT and plot width of the Gaussian 
    // against 1/PT, if the fit fails, the result is not plotted
    Gauss->SetParameters( m_IPres_X_histos[i]->GetMaximum(), m_IPres_X_histos[i]->GetMean(), m_IPres_X_histos[i]->GetRMS() );
    fitResult = m_IPres_X_histos[i]->Fit( Gauss, "QN" );
    if( fitResult!=0 )
      Warning( "Fit to IP_X failed for bin "+m_histoTitles[i] ).ignore();
    else{
      m_h_InversePTVsGaussWidth_X->SetBinContent( i+1, Gauss->GetParameter(2) );
      m_h_InversePTVsGaussWidth_X->SetBinError( i+1, Gauss->GetParError(2) );
    }    

    // do the same for IP Y resolution
    Gauss->SetParameters( m_IPres_Y_histos[i]->GetMaximum(), m_IPres_Y_histos[i]->GetMean(), m_IPres_Y_histos[i]->GetRMS() );
    fitResult = m_IPres_Y_histos[i]->Fit( Gauss, "QN" );
    if( fitResult!=0 )
      Warning( "Fit to IP_Y failed for bin "+m_histoTitles[i] ).ignore();
    else{
      m_h_InversePTVsGaussWidth_Y->SetBinContent( i+1, Gauss->GetParameter(2) );
      m_h_InversePTVsGaussWidth_Y->SetBinError( i+1, Gauss->GetParError(2) );
    }    

    // do the same for IP Z resolution
    Gauss->SetParameters( m_IPres_Z_histos[i]->GetMaximum(), m_IPres_Z_histos[i]->GetMean(), m_IPres_Z_histos[i]->GetRMS() );
    fitResult = m_IPres_Z_histos[i]->Fit( Gauss, "QN" );
    if( fitResult!=0 )
      Warning( "Fit to IP_Z failed for bin "+m_histoTitles[i] ).ignore();
    else{
      m_h_InversePTVsGaussWidth_Z->SetBinContent( i+1, Gauss->GetParameter(2) );
      m_h_InversePTVsGaussWidth_Z->SetBinError( i+1, Gauss->GetParError(2) );
    }

    // fit a Landau to the absolute 3D IP resolution for this bin of 1/PT and plot the MPV of the fit
    // if the fit succeeds
    Landau->SetParameters( m_IPres_abs_histos[i]->GetMaximum(), m_IPres_abs_histos[i]->GetMean(), 
                           m_IPres_abs_histos[i]->GetRMS() );
    fitResult = m_IPres_abs_histos[i]->Fit( Landau, "QN" );
    if( fitResult != 0 )
      Warning( "Fit to abs. IP failed for bin "+m_histoTitles[i] ).ignore();
    else{
      m_h_InversePTVsMPVofLandau_abs->SetBinContent( i+1, Landau->GetParameter(1) );
      m_h_InversePTVsMPVofLandau_abs->SetBinError( i+1, Landau->GetParError(1) );
    }

    // plot the number of tracks in this bin of 1/PT
    double nEntries = m_IPres_abs_histos[i]->GetEntries();
    m_h_InversePTVsNTracks->SetBinContent( i+1, nEntries );
    m_h_InversePTVsNTracks->SetBinError( i+1, sqrt( nEntries ) );
    
  }
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

// fill histograms for a given set of track IPs and PT
StatusCode Velo::VeloIPResolutionMonitor::fillHistos( Gaudi::XYZVector absIP, Gaudi::XYZVector XYIP, double ZIP, double PT )
{
  // select the bin of 1/PT to which the track belongs
  for( int i = 0; i<(int)m_bins.size()-1; i++ ){
    if( m_bins[i] < 1./PT && 1./PT < m_bins[i+1] ){
      plotInBin( absIP, XYIP, ZIP, i );
    }
  }
  
  return StatusCode::SUCCESS;
}

// fill histograms in a given bin of 1/PT for the given IPs
StatusCode Velo::VeloIPResolutionMonitor::plotInBin( Gaudi::XYZVector absIP, Gaudi::XYZVector XYIP, double ZIP, int binNo )
{

  m_IPres_X_histos[binNo]->Fill( XYIP.x()/Gaudi::Units::mm );
  m_IPres_Y_histos[binNo]->Fill( XYIP.y()/Gaudi::Units::mm );
  m_IPres_Z_histos[binNo]->Fill( ZIP/Gaudi::Units::mm );
  m_IPres_abs_histos[binNo]->Fill( sqrt( absIP.Mag2() )/Gaudi::Units::mm );
  
  return StatusCode::SUCCESS;
}
