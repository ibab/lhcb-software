// $Id: VeloIPResolutionMonitor.cpp,v 1.9 2009-10-22 16:26:46 malexand Exp $
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

// These are needed if you want to employ any of the MC association related tasks
// to do so uncomment any relevant pieces of code
// it is also requires to add the line
// use   DaVinciMCKernel    v*    Phys
// to the requirements file
/*#include "Event/MCHeader.h"
#include "Event/MCParticle.h"
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"*/

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
  : GaudiHistoAlg ( name , pSvcLocator ),
    m_vertexer("TrackVertexer")
{
  // Set the binning options
  declareProperty("BinValues", m_bins );
  declareProperty("UseVariableBins",m_useVariableBins = false );
  declareProperty("UseLogScale",m_useLogScale = false );
  declareProperty("InversePTMin",m_InversePTMin = 0.0 );
  declareProperty("InversePTMax",m_InversePTMax = 10.0 );
  declareProperty("NBins",m_nBins = 20 );

  // Set whether to save the underlying histograms used to make the plots of fit results
  declareProperty("SaveUnderlyingHistos",m_saveUnderlyingHistos=false);
  declareProperty("UnderlyingHistosLimitYIntercept1D", m_limitIntercept1D = 6.525937e-03 );
  declareProperty("UnderlyingHistosLimitGradient1D", m_limitGradient1D = 1.947187e-02 );
  declareProperty("UnderlyingHistosLimitYIntercept3D", m_limitIntercept3D = 8.524237e-03 );
  declareProperty("UnderlyingHistosLimitGradient3D", m_limitGradient3D = 2.690075e-02 );
  declareProperty("UnderlyingHistosLimitFactor", m_limitFactor = 10. );

  // Set whether to calculate residuals as a fn. of eta and phi compared to the 1/PT parametrisation
  // and which gradient & y intercept values to use
  declareProperty("CalcResiduals", m_calcResiduals = true );
  declareProperty("ResidualsYIntercept1D", m_res1DyIntercept = 6.160529e-03 );
  declareProperty("ResidualsGradient1D",m_res1Dgrad = 2.018678e-02 );
  declareProperty("ResidualsQuad1D",m_res1Dquad = -2.288513e-04 );
  declareProperty("ResidualsYIntercept3D", m_res3DyIntercept = 8.680847e-03 );
  declareProperty("ResidualsLinear3D", m_res3Dgrad = 2.656325e-02 );
  declareProperty("ResidualsQuad3D", m_res3Dquad = 1.135474e-04 );

  // Set whether to use the MC PV associated to each track instead of the rec. PV
  /*declareProperty("CheckTrackMCAssociation", m_checkAssoc = false );
  declareProperty("UseOnlyPromptTracks", m_useOnlyPrompt = false );
  declareProperty("UseMCPV",m_useMCPV = false );*/

  // Set whether to smear the tracks used and by how much (in microns)
  declareProperty("SmearConstant",m_smearConstant = 0. );

  // Set whether to check if each event has passed L0
  declareProperty("RequireL0", m_requireL0 = false );

  // Set whether to refit PVs without the track for which IP is being calculated
  declareProperty("RefitPVs", m_refitPVs = false );

  // Set whether to fit parabolas to the output profiles and display the fit results
  declareProperty("FitOutputProfiles",m_fitOutputProfiles = false );

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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // If UseMCPV or UseOnlyPrompt is set, CheckTrackMCAssociation is set
  //if( (m_useMCPV || m_useOnlyPrompt) && !m_checkAssoc ) m_checkAssoc = true;

  // Set histo dir
  setHistoTopDir( "Velo/" );

  // Get vertexer if required
  if( m_refitPVs ) sc = m_vertexer.retrieve();

  // get the track extrapolator used in calculating the IPs
  m_trackExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator","Extrapolator",this);

  // If tracks are to be smeared get the track fitter and measurement provider
  if( m_smearConstant != 0 ){
    m_smearConstant *= micrometer;
    m_trackFitter = tool<ITrackFitter>("TrackMasterFitter","Fitter",this);
    m_measurementProvider = tool<IMeasurementProvider>("MeasurementProvider","MeasProvider", this );
  }

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
  //if useVariableBins=true but the bin values are not defined in the options file, a default set of bins is used
  else if( m_bins.size()==0 ){
    // Default bins of 1/PT in GeV-1
    double defaultBins[23] = { 0.000,
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
    if( m_useLogScale ) {
      defaultBins[0] = log10(0.1);
      for( int i=1; i<23; i++ ) {
        double bin = defaultBins[i];
        defaultBins[i] = log10(bin);
      }
    }
    
    m_bins.assign( &defaultBins[0], &defaultBins[23] );
  }
  // If bin values are defined in the options file, and UseLogScale = True, the log of each bin value is taken
  else if( m_useLogScale ){
    for( int i = 0; i<(int)m_bins.size(); i++ ){
      double bin = m_bins[i];
      m_bins[i] = log10(bin);
    }
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
      limit1D = m_limitFactor * ( m_limitGradient1D * m_bins[ i+1 ] + m_limitIntercept1D );
      limit3D = m_limitFactor * ( m_limitGradient3D * m_bins[ i+1 ] + m_limitIntercept3D );
    }
    else{
      limit1D = m_limitFactor * ( m_limitGradient1D * pow( 10, m_bins[ i+1 ] ) + m_limitIntercept1D );
      limit3D = m_limitFactor * ( m_limitGradient3D * pow( 10, m_bins[ i+1 ] ) + m_limitIntercept3D );
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
      m_IPres_Z_histos.push_back( Aida2ROOT::aida2root( book( "IPres_Z_"+m_histoIDs[i], 
                                                              "Resolution of IP_Z for tracks with "+m_histoTitles[i], 
                                                              -20.*limit1D*mm, 20.*limit1D*mm, 500 ) ) );
      m_IPres_unsigned3D_histos.push_back( Aida2ROOT::aida2root( book( "IPres_unsigned3D_"+m_histoIDs[i], 
                                                                       "Resolution of unsigned 3D IP for tracks with "
                                                                       +m_histoTitles[i],
                                                                       0.0*mm, limit3D*mm, 500 ) ) );
      m_IPres_signed3D_histos.push_back( Aida2ROOT::aida2root( book( "IPres_signed3D_"+m_histoIDs[i], 
                                                                     "Resolution of signed 3D IP for tracks with "
                                                                     +m_histoTitles[i], 
                                                                     -limit3D*mm, limit3D*mm, 500 ) ) );

      m_IPres_signed3DPhSpCorrect_histos.push_back( Aida2ROOT::aida2root( book( "IPres_signed3DPhSpCorrect_"+m_histoIDs[i], 
                                                                                "Resolution of signed 3D IP for tracks with "
                                                                                +m_histoTitles[i]+" with phase space correction",
                                                                                -limit3D*mm, limit3D*mm, 500 ) ) );
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
      m_IPres_X_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, -limit1D*mm, limit1D*mm ) );

      strID = "IPres_Y_"+m_histoIDs[i];
      strTitle = "Resolution of IP_Y for tracks with "+m_histoTitles[i];
      m_IPres_Y_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, -limit1D*mm, limit1D*mm ) );

      strID = "IPres_Z_"+m_histoIDs[i];
      strTitle = "Resolution of IP_Z for tracks with "+m_histoTitles[i];
      m_IPres_Z_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, -20.*limit1D*mm, 20.*limit1D*mm ) );

      strID = "IPres_unsigned3D_"+m_histoIDs[i];
      strTitle = "Resolution of unsigned 3D IP for tracks with "+m_histoTitles[i];
      m_IPres_unsigned3D_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, 0.0*mm, limit3D*mm ) );

      strID = "IPres_signed3D_"+m_histoIDs[i];
      strTitle = "Resolution of signed 3D IP for tracks with "+m_histoTitles[i];
      m_IPres_signed3D_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, -limit3D*mm, limit3D*mm ) );

      strID = "IPres_signed3DPhSpCorrect_"+m_histoIDs[i];
      strTitle = "Resolution of signed 3D IP for tracks with "+m_histoTitles[i]+" with phase space correction";
      m_IPres_signed3DPhSpCorrect_histos.push_back( new TH1D( strID.c_str(), strTitle.c_str(), 500, -limit3D*mm, limit3D*mm ) );
      m_IPres_signed3DPhSpCorrect_histos[i]->Sumw2();
      
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
  if( m_useVariableBins ) m_h_GaussWidthVsInversePT_X->SetBins( (int)m_bins.size() - 1, &m_bins[0] );

  m_h_GaussWidthVsInversePT_Y = Aida2ROOT::aida2root( book( "GaussWidth_IP_Y_Vs_InversePT", 
                                                            "Width of Gaussian fit to IP_Y resolution Vs 1/PT", 
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_GaussWidthVsInversePT_Y->SetXTitle(XTitle.c_str());
  m_h_GaussWidthVsInversePT_Y->SetYTitle("Width of Gaussian (mm)");
  if( m_useVariableBins ) m_h_GaussWidthVsInversePT_Y->SetBins( (int)m_bins.size() - 1, &m_bins[0] );
  
  m_h_GaussWidthVsInversePT_Z = Aida2ROOT::aida2root( book( "GaussWidth_IP_Z_Vs_InversePT", 
                                                            "Width of Gaussian fit to IP_Z resolution Vs 1/PT", 
                                                            m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                                            (int)m_bins.size() -1 ));
  m_h_GaussWidthVsInversePT_Z->SetXTitle(XTitle.c_str());
  m_h_GaussWidthVsInversePT_Z->SetYTitle("Width of Gaussian (mm)");
  if( m_useVariableBins ) m_h_GaussWidthVsInversePT_Z->SetBins( (int)m_bins.size() - 1, &m_bins[0] );

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
    if( m_useVariableBins ) m_h_SampleMeanVsInversePT_unsigned3D->SetBins( (int)m_bins.size() - 1, &m_bins[0] );
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
    if( m_useVariableBins ) m_h_SglGausMeanVsInversePT_unsigned3D->SetBins( (int)m_bins.size() - 1, &m_bins[0] );
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
    if( m_useVariableBins ) m_h_DblGausMeanVsInversePT_unsigned3D->SetBins( (int)m_bins.size() - 1, &m_bins[0] );
  }

  if( m_fitOption3D == "TakeMean" ) m_h_MeanVsInversePT_unsigned3D = m_h_SampleMeanVsInversePT_unsigned3D;
  else if ( m_fitOption3D == "FitSglGaus" ) m_h_MeanVsInversePT_unsigned3D = m_h_SglGausMeanVsInversePT_unsigned3D;
  else if ( m_fitOption3D == "FitDblGaus" ) m_h_MeanVsInversePT_unsigned3D = m_h_DblGausMeanVsInversePT_unsigned3D;
  else{
    Warning( "'FitOption3D' parameter must take values 'TakeMean', 'FitSglGaus', or 'FitDblGaus'" );
    return StatusCode::FAILURE;
  }
  

  m_h_GaussWidthVsInversePT_signed3D = 
    Aida2ROOT::aida2root( book( "GaussWidth_signed3DIP_Vs_InversePT",
                                "Width of Gaussian fit to absolute signed 3D IP resolution Vs 1/PT",
                                m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                (int)m_bins.size() - 1 ) );
  m_h_GaussWidthVsInversePT_signed3D->SetXTitle(XTitle.c_str());
  m_h_GaussWidthVsInversePT_signed3D->SetYTitle("Width of Gaussian (mm)");
  if( m_useVariableBins ) m_h_GaussWidthVsInversePT_signed3D->SetBins( (int)m_bins.size() - 1, &m_bins[0] );

  m_h_GaussWidthVsInversePT_signed3DPhSpCorrect = 
    Aida2ROOT::aida2root( book( "GaussWidth_signed3DIP_PhSpCorrect_Vs_InversePT",
                                "Width of Gaussian fit to signed absolute 3D IP resolution with phase space correction Vs 1/PT",
                                m_bins[0], m_bins[ (int)m_bins.size() - 1 ], 
                                (int)m_bins.size() - 1 ) );
  m_h_GaussWidthVsInversePT_signed3DPhSpCorrect->SetXTitle(XTitle.c_str());
  m_h_GaussWidthVsInversePT_signed3DPhSpCorrect->SetYTitle("Width of Gaussian (mm)");
  if( m_useVariableBins ) m_h_GaussWidthVsInversePT_signed3DPhSpCorrect->SetBins( (int)m_bins.size() - 1, &m_bins[0] );

  // book additional histograms of track multiplicity and frequency of 1/PT
  m_h_TrackMultiplicity = Aida2ROOT::aida2root( book( "TrackMultiplicity", "PV Track Multiplicity", 0.0, 150.0, 75 ));
  m_h_TrackMultiplicity->SetXTitle("Number of tracks");

  m_h_InversePTVsNTracks = Aida2ROOT::aida2root( book( "NTracks_Vs_InversePT", "Number of tracks found in each bin of 1/PT", 
                                                       m_bins[0], m_bins[ (int)m_bins.size() - 1 ], (int)m_bins.size() -1 ));
  m_h_InversePTVsNTracks->SetXTitle(XTitle.c_str());
  m_h_InversePTVsNTracks->SetYTitle("Number of tracks");
  if( m_useVariableBins ) m_h_InversePTVsNTracks->SetBins( (int)m_bins.size() - 1, &m_bins[0] );

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

    m_p_XphiResiduals = Aida2ROOT::aida2root( bookProfile1D( "XphiResiduals", 
                                                             "Residuals of X IP res. wrt. 1/PT parametrisation Vs Phi", 
                                                             -pi, pi ) );
    m_p_XphiResiduals->SetXTitle("Track phi");
    m_p_XphiResiduals->SetYTitle("Residuals (mm)");

    m_p_XetaResiduals = Aida2ROOT::aida2root( bookProfile1D( "XetaResiduals", 
                                                             "Residuals of X IP res. wrt. 1/PT parametrisation Vs Eta", 
                                                             1.5, 5.5 ) );
    m_p_XetaResiduals->SetXTitle("Track phi");
    m_p_XetaResiduals->SetYTitle("Residuals (mm)");

    m_p_YphiResiduals = Aida2ROOT::aida2root( bookProfile1D( "YphiResiduals", 
                                                             "Residuals of Y IP res. wrt. 1/PT parametrisation Vs Phi", 
                                                             -pi, pi ) );
    m_p_YphiResiduals->SetXTitle("Track phi");
    m_p_YphiResiduals->SetYTitle("Residuals (mm)");

    m_p_YetaResiduals = Aida2ROOT::aida2root( bookProfile1D( "YetaResiduals", 
                                                             "Residuals of Y IP res. wrt. 1/PT parametrisation Vs Eta", 
                                                             1.5, 5.5 ) );
    m_p_YetaResiduals->SetXTitle("Track phi");
    m_p_YetaResiduals->SetYTitle("Residuals (mm)");

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

        // skip 'Velo' type tracks as they have no PT measurement
        if( (*tr)->type() == Track::Velo ) continue;

        const LHCb::Track* track = &(**tr);

        double inversePT;
        if( !m_useLogScale ) inversePT = 1./(track->pt()/GeV);
        else inversePT = log10( 1./(track->pt()/GeV) );

        // Skip tracks outwith the bin range
        if( inversePT > *(m_bins.rbegin()) || *(m_bins.begin()) > inversePT ) continue;

        counter("Tracks selected")++;

        // if CheckTrackMCAssociations is set the tracks are associated to MC particles and the 
        // number that are associated to prompt or secondary particles, or have no association is counted
        // If UseOnlyPrompt is set, only track associated to prompt MC particles are used
        /*if( m_checkAssoc ){
          if( !checkMCAssoc( track, PVpos ) ) continue;
          }*/
        
        // refit PV removing current track
        if( m_refitPVs /*&& !m_useMCPV*/ ){
          
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

        // If tracks are to be smeared the error on each measurement used to make the track 
        // is increased in quadrature by SmearConstant and the track is refitted
        // this has been used to simulate worsening the VELO resolution
        if( m_smearConstant != 0 ){

          track = smearTrack( track );
          
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

        // extrapolate the current track to its point of intersection with the horizontal plane
        // containing the PV to get the Z IP
        Gaudi::Plane3D horizontalPVplane( Gaudi::XYZVector(0,1,0), PVpos );
        LHCb::State horizontalIntersectState;
        m_trackExtrapolator->propagate( *track, horizontalPVplane, horizontalIntersectState );
        double ZIP = horizontalIntersectState.position().z() - PVpos.z();

        // fill histograms
        fillHistos( IP3D, XYIP, ZIP, inversePT );

        // fill histos of residuals of the 1/PT parametrisation as a fn. of eta and phi 
        if( m_calcResiduals ){
          if( m_useLogScale ) inversePT = pow( 10, inversePT );
          m_p_3DphiResiduals->Fill( track->phi(), sqrt( IP3D.mag2() ) - m_res3DyIntercept - m_res3Dgrad * inversePT
                                    - m_res3Dquad * inversePT * inversePT );
          m_p_3DetaResiduals->Fill( track->pseudoRapidity(), sqrt( IP3D.mag2() ) - m_res3DyIntercept - m_res3Dgrad * inversePT 
                                    - m_res3Dquad * inversePT * inversePT );
          m_p_XphiResiduals->Fill( track->phi(), XYIP.x() - m_res1DyIntercept - m_res1Dgrad * inversePT 
                                   - m_res1Dquad * inversePT * inversePT );
          m_p_XetaResiduals->Fill( track->pseudoRapidity(), XYIP.x() - m_res1DyIntercept - m_res1Dgrad * inversePT 
                                   - m_res1Dquad * inversePT * inversePT );
          m_p_YphiResiduals->Fill( track->phi(), XYIP.y() - m_res1DyIntercept - m_res1Dgrad * inversePT 
                                   - m_res1Dquad * inversePT * inversePT );
          m_p_YetaResiduals->Fill( track->pseudoRapidity(), XYIP.y() - m_res1DyIntercept - m_res1Dgrad * inversePT 
                                   - m_res1Dquad * inversePT * inversePT );
        }

        if( m_smearConstant != 0 ) delete track;
        
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

  // do the same for IP Z resolution
  fitGaussAndPlotWidth( m_IPres_Z_histos, m_h_GaussWidthVsInversePT_Z );    

  // do the same for unsigned 3D IP
  fitGaussAndPlotWidth( m_IPres_signed3D_histos, m_h_GaussWidthVsInversePT_signed3D );

  // do the same for the signed 3D IP with phase space correction
  fitGaussAndPlotWidth( m_IPres_signed3DPhSpCorrect_histos, m_h_GaussWidthVsInversePT_signed3DPhSpCorrect );

  // fit a Landau to the unsigned 3D IP resolution for this bin of 1/PT and plot the MPV of the fit
  // if the fit succeeds
  if( m_fitOption3D == "TakeMean" || m_takeMean3D ) 
    plotMean( m_IPres_unsigned3D_histos, m_h_SampleMeanVsInversePT_unsigned3D );
  if( m_fitOption3D == "FitSglGaus" || m_fitSglGaus3D ) 
    fit2DGausAndPlotMean( m_IPres_unsigned3D_histos, m_h_SglGausMeanVsInversePT_unsigned3D );
  if( m_fitOption3D == "FitDblGaus" || m_fitDblGaus3D ) 
    fitDbl2DGausAndPlotMean( m_IPres_unsigned3D_histos, m_h_DblGausMeanVsInversePT_unsigned3D );

  for( int i = 0; i<(int)m_bins.size()-1; i++ ){  
    // plot the number of tracks in this bin of 1/PT
    double nEntries = m_IPres_unsigned3D_histos[i]->GetEntries();
    m_h_InversePTVsNTracks->SetBinContent( i+1, nEntries );
    m_h_InversePTVsNTracks->SetBinError( i+1, sqrt( nEntries ) );
  }
    
  if( !m_saveUnderlyingHistos ){
    for( int i=0; i<(int)m_bins.size()-1; i++ ){
      delete m_IPres_X_histos[i];
      delete m_IPres_Y_histos[i];
      delete m_IPres_Z_histos[i];
      delete m_IPres_unsigned3D_histos[i];
      delete m_IPres_signed3D_histos[i];
      delete m_IPres_signed3DPhSpCorrect_histos[i];
    }
  }

  if( m_refitPVs ) sc = m_vertexer.release();

  if( m_fitOutputProfiles ){
    fitOutputProfiles();
  }
  
  return sc && GaudiAlgorithm::finalize();  // must be called after all other actions
}


//====================================================================
// fill histograms for a given set of track IPs and PT
//====================================================================
StatusCode Velo::VeloIPResolutionMonitor::fillHistos( Gaudi::XYZVector IP3D, 
                                                      Gaudi::XYZVector XYIP, double ZIP, double inversePT )
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
StatusCode Velo::VeloIPResolutionMonitor::plotInBin( Gaudi::XYZVector IP3D, 
                                                     Gaudi::XYZVector XYIP, double ZIP, int binNo )
{

  m_IPres_X_histos[binNo]->Fill( XYIP.x()/mm );
  m_IPres_Y_histos[binNo]->Fill( XYIP.y()/mm );
  m_IPres_Z_histos[binNo]->Fill( ZIP/mm );

  m_IPres_unsigned3D_histos[binNo]->Fill( sqrt( IP3D.Mag2() )/mm );
  float sign = 1.;
  if( IP3D.z() <= 0. ) sign = -1.;
  double signedIP3D = sqrt( IP3D.Mag2() )*sign/mm;
  m_IPres_signed3D_histos[binNo]->Fill( signedIP3D );
  if( signedIP3D!=0 && IP3D.z() !=0 )
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

//=========================================================================
//  Fit the output profiles with parabolas and display the fit results
//=========================================================================

StatusCode Velo::VeloIPResolutionMonitor::fitOutputProfiles()
{
  
  TF1* quad;
  if( !m_useLogScale ){
    quad = new TF1( "Quadratic", "pol2(0)" );
    
    quad->SetParameters( m_h_GaussWidthVsInversePT_X->GetMinimum(),
                         ( m_h_GaussWidthVsInversePT_X->GetMaximum() - m_h_GaussWidthVsInversePT_X->GetMinimum() )
                         / (m_bins[m_nBins-1] - m_bins[0]),
                         0 );
    m_h_GaussWidthVsInversePT_X->Fit( quad, "QN" );
    info() << "Results of quadratic fit to IP_X resolution:" << endmsg;
    info() << "Y Intercept: " << quad->GetParameter(0) << endmsg;
    info() << "Linear term: " << quad->GetParameter(1) << endmsg;
    info() << "Quadratic term: " << quad->GetParameter(2) << endmsg;
    info() << "" << endmsg;
    
    quad->SetParameters( m_h_GaussWidthVsInversePT_Y->GetMinimum(),
                         ( m_h_GaussWidthVsInversePT_Y->GetMaximum() - m_h_GaussWidthVsInversePT_Y->GetMinimum() )
                         / (m_bins[m_nBins-1] - m_bins[0]),
                         0 );
    m_h_GaussWidthVsInversePT_Y->Fit( quad, "QN" );
    info() << "Results of quadratic fit to IP_Y resolution:" << endmsg;
    info() << "Y Intercept: " << quad->GetParameter(0) << endmsg;
    info() << "Linear term: " << quad->GetParameter(1) << endmsg;
    info() << "Quadratic term: " << quad->GetParameter(2) << endmsg;
    info() << "" << endmsg;
    
    quad->SetParameters( m_h_MeanVsInversePT_unsigned3D->GetMinimum(),
                         ( m_h_MeanVsInversePT_unsigned3D->GetMaximum() 
                           - m_h_MeanVsInversePT_unsigned3D->GetMinimum() )
                         / (m_bins[m_nBins-1] - m_bins[0]),
                         0 );
    m_h_MeanVsInversePT_unsigned3D->Fit( quad, "QN" );
    info() << "Results of quadratic fit to IP_3D resolution:" << endmsg;
    info() << "Y Intercept: " << quad->GetParameter(0) << endmsg;
    info() << "Linear term: " << quad->GetParameter(1) << endmsg;
    info() << "Quadratic term: " << quad->GetParameter(2) << endmsg;
    info() << "" << endmsg;
    
  }
    
  else {
    quad = new TF1( "Quadratic", "10^(pol2(0))" );

    quad->SetParameters( m_h_GaussWidthVsInversePT_X->GetMinimum(),
                         ( m_h_GaussWidthVsInversePT_X->GetMaximum() - m_h_GaussWidthVsInversePT_X->GetMinimum() )
                         / (pow(10, m_bins[m_nBins-1]) - pow(10, m_bins[0])),
                         0 );
    m_h_GaussWidthVsInversePT_X->Fit( quad, "QN" );
    info() << "Results of quadratic fit to IP_X resolution:" << endmsg;
    info() << "Y Intercept: " << quad->GetParameter(0) << endmsg;
    info() << "Linear term: " << quad->GetParameter(1) << endmsg;
    info() << "Quadratic term: " << quad->GetParameter(2) << endmsg;
    info() << "" << endmsg;
    
    quad->SetParameters( m_h_GaussWidthVsInversePT_Y->GetMinimum(),
                         ( m_h_GaussWidthVsInversePT_Y->GetMaximum() - m_h_GaussWidthVsInversePT_Y->GetMinimum() )
                         / (pow(10, m_bins[m_nBins-1]) - pow(10,m_bins[0])),
                         0 );
    m_h_GaussWidthVsInversePT_Y->Fit( quad, "QN" );
    info() << "Results of quadratic fit to IP_Y resolution:" << endmsg;
    info() << "Y Intercept: " << quad->GetParameter(0) << endmsg;
    info() << "Linear term: " << quad->GetParameter(1) << endmsg;
    info() << "Quadratic term: " << quad->GetParameter(2) << endmsg;
    info() << "" << endmsg;
    
    quad->SetParameters( m_h_MeanVsInversePT_unsigned3D->GetMinimum(),
                         ( m_h_MeanVsInversePT_unsigned3D->GetMaximum() 
                           - m_h_MeanVsInversePT_unsigned3D->GetMinimum() )
                         / (pow(10, m_bins[m_nBins-1]) - pow(10, m_bins[0])),
                         0 );
    m_h_MeanVsInversePT_unsigned3D->Fit( quad, "QN" );
    info() << "Results of quadratic fit to IP_3D resolution:" << endmsg;
    info() << "Y Intercept: " << quad->GetParameter(0) << endmsg;
    info() << "Linear term: " << quad->GetParameter(1) << endmsg;
    info() << "Quadratic term: " << quad->GetParameter(2) << endmsg;
    info() << "" << endmsg;
    
  }
    
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Smear VELO measurement errors on a track and refit it
//=========================================================================

const LHCb::Track* Velo::VeloIPResolutionMonitor::smearTrack( const LHCb::Track* track )
{  

  LHCb::Track* smearedTrack = track->clone();
  m_measurementProvider->load( *smearedTrack );
  m_trackFitter->fit( *smearedTrack );
  
  for( LHCb::Track::ConstNodeRange::const_iterator inode = smearedTrack->nodes().begin();
        inode != smearedTrack->nodes().end(); ++inode){

    if( (*inode)->type() != LHCb::Node::HitOnTrack ) continue;
    
    if( (*inode)->measurement().lhcbID().isVelo() ){

      // make plots of measurement resolution vs pitch for R and Phi measurements separately, before smearing

      double f = std::sqrt( (*inode)->errMeasure2()/(*inode)->errResidual2()) ;
      double resolution = f*(*inode)->residual();

      double measRes = (*inode)->measurement().resolution( (*inode)->state().position(),
                                                           (*inode)->state().momentum() );
      
      if( (*inode)->measurement().type() == LHCb::Measurement::VeloPhi 
          || (*inode)->measurement().type() == LHCb::Measurement::VeloLitePhi ){
        
        const LHCb::VeloCluster* clus = static_cast<LHCb::VeloPhiMeasurement&>
          ((*inode)->measurement()).cluster() ;
        const DeVeloPhiType* phiDet = &(((VeloPhiMeasurement&)((*inode)->measurement())).sensor());
        Gaudi::XYZPoint globalPoint = (*inode)->state().position() ;
        Gaudi::XYZPoint localPoint = 
          (*inode)->measurement().detectorElement()->geometry()->toLocal( globalPoint ) ;
        double pitch =  localPoint.Rho() * phiDet->phiPitch( unsigned(clus->strip(0)) ) ;

        plot2D( pitch, resolution, "UnsmearedPhi_ClusterResolutionVsPitch_Scatter", 
                "Unsmeared Phi Cluster Resolution Vs Pitch", 0.03, 0.11, -0.3, 0.3, 100, 200 );
        plot2D( pitch, measRes, "UnsmearedPhi_MeasurementResolutionVsPitch_Scatter", 
                "Unsmeared Phi Measurement Resolution Vs Pitch", 0.03, 0.11, 0., 0.04, 100, 200 );

      }
      else{   
        const LHCb::VeloCluster* clus = 
          static_cast<LHCb::VeloRMeasurement&>((*inode)->measurement()).cluster() ;
        const DeVeloRType* rDet = &(((VeloRMeasurement&)((*inode)->measurement())).sensor());
        Gaudi::XYZPoint globalPoint = (*inode)->state().position() ;
        double pitch = rDet->rPitch( unsigned(clus->strip(0)) ) ;

        plot2D( pitch, resolution, "UnsmearedR_ClusterResolutionVsPitch_Scatter", 
                "Unsmeared R Cluster Resolution Vs Pitch", 0.03, 0.11, -0.3, 0.3, 100, 200 );
        plot2D( pitch, measRes, "UnsmearedR_MeasurementResolutionVsPitch_Scatter", 
                "Unsmeared R Measurement Resolution Vs Pitch", 0.03, 0.11, 0., 0.04, 100, 200 );
      }      
      
      double smearFactor = sqrt( measRes*measRes + m_smearConstant*m_smearConstant )/measRes;
      (*inode)->measurement().setErrMeasure( (*inode)->measurement().errMeasure() * smearFactor );
      
    }
  }          

  m_trackFitter->fit( *smearedTrack );

  // make plots of measurement resolution vs pitch after smearing

  for( LHCb::Track::ConstNodeRange::const_iterator inode = smearedTrack->nodes().begin();
        inode != smearedTrack->nodes().end(); ++inode){

    if( (*inode)->type() != LHCb::Node::HitOnTrack ) continue;
    
    if( (*inode)->measurement().lhcbID().isVelo() ){

      // make plots of measurement resolution vs pitch for R and Phi measurements separately, before smearing

      double f = std::sqrt( (*inode)->errMeasure2()/(*inode)->errResidual2()) ;
      double resolution = f*(*inode)->residual();

      double measRes = (*inode)->measurement().resolution( (*inode)->state().position(),
                                                           (*inode)->state().momentum() );
      
      if( (*inode)->measurement().type() == LHCb::Measurement::VeloPhi 
          || (*inode)->measurement().type() == LHCb::Measurement::VeloLitePhi ){
        
        const LHCb::VeloCluster* clus = static_cast<LHCb::VeloPhiMeasurement&>
          ((*inode)->measurement()).cluster() ;
        const DeVeloPhiType* phiDet = &(((VeloPhiMeasurement&)((*inode)->measurement())).sensor());
        Gaudi::XYZPoint globalPoint = (*inode)->state().position() ;
        Gaudi::XYZPoint localPoint = 
          (*inode)->measurement().detectorElement()->geometry()->toLocal( globalPoint ) ;
        double pitch =  localPoint.Rho() * phiDet->phiPitch( unsigned(clus->strip(0)) ) ;

        plot2D( pitch, resolution, "SmearedPhi_ClusterResolutionVsPitch_Scatter", 
                "Smeared Phi Cluster Resolution Vs Pitch", 0.03, 0.11, -0.3, 0.3, 100, 200 );
        plot2D( pitch, measRes, "SmearedPhi_MeasurementResolutionVsPitch_Scatter", 
                "Smeared Phi Measurement Resolution Vs Pitch", 0.03, 0.11, 0., 0.04, 100, 200 );

      }
      else{   
        const LHCb::VeloCluster* clus = 
          static_cast<LHCb::VeloRMeasurement&>((*inode)->measurement()).cluster() ;
        const DeVeloRType* rDet = &(((VeloRMeasurement&)((*inode)->measurement())).sensor());
        Gaudi::XYZPoint globalPoint = (*inode)->state().position() ;
        double pitch = rDet->rPitch( unsigned(clus->strip(0)) ) ;

        plot2D( pitch, resolution, "SmearedR_ClusterResolutionVsPitch_Scatter", 
                "Smeared R Cluster Resolution Vs Pitch", 0.03, 0.11, -0.3, 0.3, 100, 200 );
        plot2D( pitch, measRes, "SmearedR_MeasurementResolutionVsPitch_Scatter", 
                "Smeared R Measurement Resolution Vs Pitch", 0.03, 0.11, 0., 0.04, 100, 200 );

      }

    }
  }          
          
  return const_cast< const LHCb::Track* >( smearedTrack );
}

//=========================================================================
//  Check MC association of tracks, rejecting non-prompt and/or using MC PV position if selected
//=========================================================================
/*bool Velo::VeloIPResolutionMonitor::checkMCAssoc( const LHCb::Track* track, Gaudi::XYZPoint PVpos )
{
  
  LinkedTo<LHCb::MCParticle,LHCb::Track> directLink( evtSvc(), msgSvc(), TrackLocation::Default );
  LHCb::MCParticle* mcPart = directLink.first( track );
  if( mcPart==NULL ){ 
    counter("Ghost tracks")++; 
    if( m_useMCPV || m_useOnlyPrompt ) return false; 
  }
  const LHCb::MCVertex* originv=mcPart->primaryVertex();
  if( !originv ){ 
    counter("No MC primary vertex")++; 
    if( m_useMCPV || m_useOnlyPrompt ) return false; 
  }

  if( mcPart->mother()==NULL || (*(mcPart->mother()->endVertices().begin()))->position() == originv->position() ) {
    counter("Prompt assoc. MC particles")++;
  }
  else { 
    counter("Secondary assoc. MC particles")++;
    if( m_useOnlyPrompt ) return false;
  }
  // if UseMCPV is set, the position of the origin vertex of the MC particle associated to the track is
  // used to calculate the IPs
  if( m_useMCPV ) PVpos = originv->position();
  return true;
          
}*/
