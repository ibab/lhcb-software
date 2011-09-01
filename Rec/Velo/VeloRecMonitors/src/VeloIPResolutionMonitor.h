// $Id: VeloIPResolutionMonitor.h,v 1.9 2010/04/02 16:09:45 malexand Exp $
#ifndef VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H 
#define VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiHistoAlg.h>
#include <GaudiKernel/ToolHandle.h>

#include <TrackInterfaces/ITrackExtrapolator.h>
#include <TrackInterfaces/IPVOfflineTool.h>

#include <Event/RecVertex.h>

#include <TH1D.h>
#include <TH2D.h>

#include <string>
#include <sstream>

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

  protected:

  private:

    TH2D* m_h_IPXVsInversePT ;
    TH2D* m_h_IPYVsInversePT ; 
                               
    TH2D* m_h_IPXVsPhi ;
    TH2D* m_h_IPYVsPhi ;
    TH2D* m_h_IPXVsPhi_HighPT ;
    TH2D* m_h_IPYVsPhi_HighPT ; 
                               
    TH2D* m_h_IPXVsPhi_Eta45To50 ;
    TH2D* m_h_IPYVsPhi_Eta45To50 ; 
    TH2D* m_h_IPXVsPhi_Eta25To30 ;
    TH2D* m_h_IPYVsPhi_Eta25To30 ; 
    TH2D* m_h_IPXVsPhi_EtaLT0 ;
    TH2D* m_h_IPYVsPhi_EtaLT0 ; 
                                
    TH2D* m_h_IPXVsEta ; 
    TH2D* m_h_IPYVsEta ; 
                         
    TH1D* m_h_TrackMultiplicity ; 

    std::string m_vertexLocation ;
    std::string m_trackLocation ;

    const LHCb::RecVertex* m_pv;
    const LHCb::Track* m_track;

    double m_InversePTMin;
    double m_InversePTMax;
    int m_nBins;
    int m_nBinsPhi ;
    int m_nBinsEta ;
    
    unsigned int m_minPVnTracks;
    float m_maxTrackChi2PerNDOF ;
    float m_maxP ;
    unsigned int m_minNRHits ;
    unsigned int m_minNTTHits ;
    
    float m_limitIntercept1D;
    float m_limitGradient1D;
    float m_limitFactor;

    bool m_refitPVs;
    bool m_makePlotsVsPhiInBinsOfEta ;

    bool m_save2DHistos ;
    
    ITrackExtrapolator* m_trackExtrapolator;
    IPVOfflineTool* m_pvtool;
    
    StatusCode calculateIPs( const LHCb::RecVertex*, const LHCb::Track*, 
                             double&, double&, double&, double&, double&, double&, LHCb::State&, LHCb::State& );
    void distance( const LHCb::RecVertex*, LHCb::State&, double&, double&, int );

    void rebinHisto( TH1D*, int nbins=0 ) ;
    void rebinHistos( std::vector<TH1D*>, int nbins=0 ) ;

    void getBinsFromTH2D( TH2D* h, std::string id, std::string title, std::string unit, std::vector<TH1D*>& out ) ;
    void saveMeanAndSigmaProfiles( TH2D* ) ;
    void saveMeanAndSigmaProfilesAndXProjection( TH2D*, std::string ) ;

    TH2D* book2DHisto( std::string, std::string, unsigned int, double, double, unsigned int, double, double, bool ) ;
    
  };
}

#endif // VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H
