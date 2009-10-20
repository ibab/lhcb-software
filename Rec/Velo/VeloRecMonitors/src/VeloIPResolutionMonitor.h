// $Id: VeloIPResolutionMonitor.h,v 1.3 2009-10-20 14:44:16 malexand Exp $
#ifndef VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H 
#define VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/IMeasurementProvider.h"

#include <TH1D.h>
#include <TProfile.h>

/** @class VeloIPResolutionMonitor VeloIPResolutionMonitor.h
 *  
 *  An algorithm to monitor IP resolutions as a function of 1/PT.
 *  The IP of tracks used to make a primary vertex is taken on that primary vertex.
 *  Assuming these tracks are from prompt particles, the true IP should be 0, so the
 *  calculated IP is in fact the resolution on the IP. Histograms produced are the 
 *  mean of the absolute unsigned 3D IP resolution against 1/PT, 
 *  and the width of a Gaussian fitted to each 1D IP resolution against 1/PT.
 *
 *  @author Michael Thomas Alexander
 *  @date   2009-05-15
 */

namespace Velo
{
  
  class VeloIPResolutionMonitor : public GaudiHistoAlg {
  public: 
    /// Standard constructor
    VeloIPResolutionMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloIPResolutionMonitor( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

    StatusCode fillHistos(Gaudi::XYZVector, Gaudi::XYZVector, double, double);

    StatusCode plotInBin(Gaudi::XYZVector, Gaudi::XYZVector, double, int );

    StatusCode fitGaussAndPlotWidth( std::vector< TH1D* >, TH1D* );
    StatusCode fitLandauAndPlotMPV( std::vector< TH1D* >, TH1D* );
    StatusCode fit2DGausAndPlotMean( std::vector< TH1D* >, TH1D* );
    StatusCode fitDbl2DGausAndPlotMean( std::vector< TH1D* >, TH1D* );
    StatusCode plotMean( std::vector< TH1D* >, TH1D* );

    StatusCode fitOutputProfiles();

    //bool checkMCAssoc( const LHCb::Track*, Gaudi::XYZPoint );
    const LHCb::Track* smearTrack( const LHCb::Track* );
    
  protected:

  private:

    bool m_requireL0;
    
    bool m_useVariableBins;
    bool m_useLogScale;
    std::vector<double> m_bins;
    double m_InversePTMin;
    double m_InversePTMax;
    int m_nBins;

    bool m_saveUnderlyingHistos;
    float m_gradient1D;
    float m_gradient3D;
    float m_limitFactor;

    bool m_refitPVs;

    bool m_calcResiduals;
    float m_3DyIntercept;
    float m_3Dgrad;
    float m_3Dquad;
    float m_1DyIntercept;
    float m_1Dgrad;
    float m_1Dquad;
    
    /*bool m_checkAssoc;
    bool m_useOnlyPrompt;
    bool m_useMCPV;*/
    
    double m_smearConstant;
    
    std::vector<std::string> m_histoIDs;
    std::vector<std::string> m_histoTitles;
    std::vector< TH1D* > m_IPres_X_histos;
    std::vector< TH1D* > m_IPres_Y_histos;
    std::vector< TH1D* > m_IPres_Z_histos;
    std::vector< TH1D* > m_IPres_unsigned3D_histos;
    std::vector< TH1D* > m_IPres_signed3D_histos;
    std::vector< TH1D* > m_IPres_signed3DPhSpCorrect_histos;
    TH1D* m_h_GaussWidthVsInversePT_X;
    TH1D* m_h_GaussWidthVsInversePT_Y;
    TH1D* m_h_GaussWidthVsInversePT_Z;
    TH1D* m_h_MeanVsInversePT_unsigned3D;
    TH1D* m_h_SampleMeanVsInversePT_unsigned3D;
    TH1D* m_h_SglGausMeanVsInversePT_unsigned3D;
    TH1D* m_h_DblGausMeanVsInversePT_unsigned3D;
    TH1D* m_h_GaussWidthVsInversePT_signed3D;
    TH1D* m_h_GaussWidthVsInversePT_signed3DPhSpCorrect;
    TH1D* m_h_TrackMultiplicity;
    TH1D* m_h_InversePTVsNTracks;

    bool m_takeMean3D;
    bool m_fitSglGaus3D;
    bool m_fitDblGaus3D;
    std::string m_fitOption3D;
    
    bool m_fitOutputProfiles;
    
    TProfile* m_p_3DphiResiduals;
    TProfile* m_p_3DetaResiduals;
    TProfile* m_p_XphiResiduals;
    TProfile* m_p_XetaResiduals;
    TProfile* m_p_YphiResiduals;
    TProfile* m_p_YetaResiduals;
  
    ITrackExtrapolator* m_trackExtrapolator;
    ToolHandle<ITrackVertexer> m_vertexer;
    ITrackFitter* m_trackFitter;
    IMeasurementProvider* m_measurementProvider;
    
  };
}

#endif // VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H
