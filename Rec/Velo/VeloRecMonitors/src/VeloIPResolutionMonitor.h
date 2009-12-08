// $Id: VeloIPResolutionMonitor.h,v 1.5 2009-12-08 01:24:42 malexand Exp $
#ifndef VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H 
#define VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
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
  
  class VeloIPResolutionMonitor : public GaudiTupleAlg {
  public: 
    /// Standard constructor
    VeloIPResolutionMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloIPResolutionMonitor( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

    StatusCode fillHistos(Gaudi::XYZVector, Gaudi::XYZVector, double);

    StatusCode plotInBin(Gaudi::XYZVector, Gaudi::XYZVector, int );

    StatusCode fitGaussAndPlotWidth( std::vector< TH1D* >, TH1D* );
    StatusCode fitLandauAndPlotMPV( std::vector< TH1D* >, TH1D* );
    StatusCode fit2DGausAndPlotMean( std::vector< TH1D* >, TH1D* );
    StatusCode fitDbl2DGausAndPlotMean( std::vector< TH1D* >, TH1D* );
    StatusCode plotMean( std::vector< TH1D* >, TH1D* );
    
  protected:

  private:

    bool m_requireL0;

    bool m_useLogScale;
    std::vector<double> m_bins;
    double m_InversePTMin;
    double m_InversePTMax;
    int m_nBins;

    bool m_saveUnderlyingHistos;
    float m_limitIntercept1D;
    float m_limitGradient1D;
    float m_limitIntercept3D;
    float m_limitGradient3D;
    float m_limitFactor;

    bool m_refitPVs;

    bool m_calcResiduals;
    float m_res3DyIntercept;
    float m_res3Dgrad;
    float m_res3Dquad;
    
    std::vector<std::string> m_histoIDs;
    std::vector<std::string> m_histoTitles;
    std::vector< TH1D* > m_IPres_X_histos;
    std::vector< TH1D* > m_IPres_Y_histos;
    std::vector< TH1D* > m_IPres_unsigned3D_histos;
    TH1D* m_h_GaussWidthVsInversePT_X;
    TH1D* m_h_GaussWidthVsInversePT_Y;
    TH1D* m_h_MeanVsInversePT_unsigned3D;
    TH1D* m_h_SampleMeanVsInversePT_unsigned3D;
    TH1D* m_h_SglGausMeanVsInversePT_unsigned3D;
    TH1D* m_h_DblGausMeanVsInversePT_unsigned3D;
    TH1D* m_h_TrackMultiplicity;
    TH1D* m_h_InversePTFreq;

    bool m_takeMean3D;
    bool m_fitSglGaus3D;
    bool m_fitDblGaus3D;
    std::string m_fitOption3D;
    
    TProfile* m_p_3DphiResiduals;
    TProfile* m_p_3DetaResiduals;
  
    ITrackExtrapolator* m_trackExtrapolator;
    ToolHandle<ITrackVertexer> m_vertexer;
  
    Tuple m_tuple;
    bool m_writeTuple;
    
  };
}

#endif // VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H
