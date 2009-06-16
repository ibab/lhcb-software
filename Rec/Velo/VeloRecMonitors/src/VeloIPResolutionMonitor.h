// $Id: VeloIPResolutionMonitor.h,v 1.1 2009-06-16 11:50:24 malexand Exp $
#ifndef VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H 
#define VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

#include "TrackInterfaces/ITrackExtrapolator.h"

#include <TH1D.h>

/** @class VeloIPResolutionMonitor VeloIPResolutionMonitor.h
 *  
 *  An algorithm to monitor IP resolutions as a function of 1/PT.
 *  The IP of tracks used to make a primary vertex is taken on that primary vertex.
 *  Assuming these tracks are from prompt particles, the true IP should be 0, so the
 *  calculated IP is in fact the resolution on the IP. Histograms produced are the 
 *  MPV of a Landau distribution fitted to the absolute 3D IP resolution against 1/PT, 
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

  protected:

  private:
    bool m_requireL0;
    
    bool m_useVariableBins;
    std::vector<double> m_bins;
    double m_InversePTMin;
    double m_InversePTMax;
    int m_nBins;

    bool m_saveUnderlyingHistos;
    
    std::vector<std::string> m_histoIDs;
    std::vector<std::string> m_histoTitles;
    std::vector< TH1D* > m_IPres_X_histos;
    std::vector< TH1D* > m_IPres_Y_histos;
    std::vector< TH1D* > m_IPres_Z_histos;
    std::vector< TH1D* > m_IPres_abs_histos;
    TH1D* m_h_InversePTVsGaussWidth_X;
    TH1D* m_h_InversePTVsGaussWidth_Y;
    TH1D* m_h_InversePTVsGaussWidth_Z;
    TH1D* m_h_InversePTVsMPVofLandau_abs;
    TH1D* m_h_TrackMultiplicity;
    TH1D* m_h_InversePTVsNTracks;
  
    ITrackExtrapolator* m_trackExtrapolator;
  };
}

#endif // VELORECMONITORS_VELOIPRESOLUTIONMONITOR_H
