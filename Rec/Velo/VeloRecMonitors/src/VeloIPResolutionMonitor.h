// $Id: VeloIPResolutionMonitor.h,v 1.7 2010-02-01 14:59:29 malexand Exp $
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

#include <Event/RecVertex.h>

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

    StatusCode calculateIPs( const LHCb::RecVertex*, const LHCb::Track*, double&, double&, double&, double&, double&, double& );
    StatusCode distance( const LHCb::RecVertex*, LHCb::State&, double&, double&, int );

    StatusCode fillHistos(double&, double&, double&, double&);

    StatusCode plotInBin(double&, double&, double&, int& );

    bool filterBeamGas( const LHCb::RecVertices* & );

    StatusCode plotRMS( std::vector< TH1D* >, TH1D* );
    StatusCode fitGaussAndPlotWidth( std::vector< TH1D* >, TH1D* );
    StatusCode fitDblGaussAndPlotWidth( std::vector< TH1D* >, TH1D* );

    StatusCode plotMean( std::vector< TH1D* >, TH1D* );
    StatusCode fit2DGausAndPlotMean( std::vector< TH1D* >, TH1D* );
    StatusCode fitDbl2DGausAndPlotMean( std::vector< TH1D* >, TH1D* );
    StatusCode fitLandauAndPlotMPV( std::vector< TH1D* >, TH1D* );

  protected:

  private:

    const LHCb::RecVertex* m_pv;
    const LHCb::Track* m_track;

    bool m_requireL0;
    bool m_filterBeamGas;
    bool m_useBeamGas;
    bool m_useCollisions;

    bool m_useLogScale;
    bool m_statOverflows;
    std::vector<double> m_bins;
    double m_InversePTMin;
    double m_InversePTMax;
    int m_nBins;
    unsigned int m_minPVnTracks;

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
    TH1D* m_h_TrackMultiplicity;
    TH1D* m_h_InversePTFreq;

    std::string m_fitOption;
    
    TProfile* m_p_3DphiResiduals;
    TProfile* m_p_3DetaResiduals;
  
    ITrackExtrapolator* m_trackExtrapolator;
    ToolHandle<ITrackVertexer> m_vertexer;
    IMeasurementProvider* m_measurementProvider;
    bool m_loadMeasurements;
    ITrackFitter* m_trackFitter;
  
    Tuple m_tuple;
    bool m_writeTuple;
    
  };
}

#endif // VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H
