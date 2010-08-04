// $Id: VeloIPResolutionMonitor.h,v 1.9 2010/04/02 16:09:45 malexand Exp $
#ifndef VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H 
#define VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include "TrackInterfaces/IPVOfflineTool.h"

#include <Event/RecVertex.h>

#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TRandom3.h>
#include <TF1.h>

#include "Event/MCHeader.h"
#include "Event/MCParticle.h"
#include "Kernel/Particle2MCLinker.h"

#include <utility>

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

  protected:

  private:

    std::string m_vertexLocation ;
    std::string m_trackLocation ;

    const LHCb::RecVertex* m_pv;
    const LHCb::Track* m_track;

    bool m_withMC;

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

    TH2D* m_h_ipXVsPhi ;
    TH1D* m_h_ipXVsPhiMean ;
    TH1D* m_h_ipXVsPhiSigma ;
    
    TH2D* m_h_ipXVsEta ;
    TH1D* m_h_ipXVsEtaMean ;
    TH1D* m_h_ipXVsEtaSigma ;
    
    TH2D* m_h_ipYVsPhi ;
    TH1D* m_h_ipYVsPhiMean ;
    TH1D* m_h_ipYVsPhiSigma ;
    
    TH2D* m_h_ipYVsEta ;
    TH1D* m_h_ipYVsEtaMean ;
    TH1D* m_h_ipYVsEtaSigma ;
    
    std::string m_fitOption;
    
    TProfile* m_p_3DphiResiduals;
    TProfile* m_p_3DetaResiduals;
  
    ITrackExtrapolator* m_trackExtrapolator;
    ToolHandle<ITrackVertexer> m_vertexer;
    IPVOfflineTool* m_pvtool;
  
    Tuple m_tuple;
    bool m_writeTuple;

    Gaudi::XYZPoint extrapolateToZ(const LHCb::Track*, double );
    Gaudi::XYZPoint extrapolateToPOCA(const LHCb::Track*, Gaudi::XYZPoint );    

    StatusCode calculateIPs( const LHCb::RecVertex*, const LHCb::Track*, double&, double&, double&, double&, double&, double& );
    StatusCode distance( const LHCb::RecVertex*, LHCb::State&, double&, double&, int );

    StatusCode fillHistos(double&, double&, double&, double&);

    StatusCode plotInBin(double&, double&, double&, int& );

    StatusCode plotRMS( std::vector< TH1D* >, TH1D* );
    StatusCode fitGaussAndPlotWidth( std::vector< TH1D* >, TH1D* );
    StatusCode fitDblGaussAndPlotWidth( std::vector< TH1D* >, TH1D* );

    StatusCode plotMean( std::vector< TH1D* >, TH1D* );
    StatusCode fit2DGausAndPlotMean( std::vector< TH1D* >, TH1D* );
    StatusCode fitDbl2DGausAndPlotMean( std::vector< TH1D* >, TH1D* );
    StatusCode fitLandauAndPlotMPV( std::vector< TH1D* >, TH1D* );


    void fillMeanAndProfile( TH1D* mean, TH1D* profile, TH1D** histos, int option ) ;
    void getBinsFromTH2D( TH2D* h, std::string id, std::string title, std::string unit, TH1D** out ) ;
    std::pair< std::pair<Double_t,Double_t>, std::pair<Double_t,Double_t> > result1D( TH1D* h, int opt );

    void rebinHisto( TH1D*, int nbins=0 ) ;
    void rebinHistos( TH1D** , int , int nbins=0 ) ;
    
    StatusCode checkMCAssoc( const LHCb::Track*, const LHCb::RecVertex*, LHCb::MCVertex*&, double&, unsigned int& );
    
  };
}

#endif // VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H
