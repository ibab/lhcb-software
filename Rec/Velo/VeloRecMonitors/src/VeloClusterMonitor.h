#ifndef VELORECMONITORS_VELOCLUSTERMONITOR_H 
#define VELORECMONITORS_VELOCLUSTERMONITOR_H 1

// Include files
// -------------
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TROOT.h"
//#include "AIDA/IFitter.h"

#include "Event/VeloCluster.h"

#include "VeloMonitorBase.h"

/** @class VeloClusterMonitor VeloClusterMonitor.h
 *  
 *  @author Shanzhen Chen
 *  @date   2015-07-03
 *
 *  @author Eduardo Rodrigues
 *  @date   2008-08-18
 *
 *  @author Mark Tobin, Kazu Akiba
 *  @date   2008-06-28
 *
 *  Based on original version by
 *  @author Aras Papadelis, Thijs Versloot
 *  @date   2006-04-30
 *
 */

namespace Velo
{
  
  class VeloClusterMonitor : public VeloMonitorBase {
    
  public:
    /// Standard constructor
    VeloClusterMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloClusterMonitor( );     ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
//    Double_t langaufun(Double_t *x, Double_t *par);
//    TF1 *langaufit(TH1D *his, Double_t *fitrange, Double_t *startvalues,
//                   Double_t *parlimitslo, Double_t *parlimitshi,
//                   Double_t *fitparams,
//                   Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF);
//    Int_t langaupro(double *params, double &maxx, double &FWHM) ;
   
  protected:
    
  private:

    // Retrieve the VeloClusters
    StatusCode veloClusters();

    // Monitor the VeloClusters
    void monitorClusters();

    // Fill the high-multiplicity histogram
    void fillHighMultiplicity(unsigned int nClustersDecoded);

    // Get number of clusters from raw event
    unsigned int getNClustersFromRaw();

    // Convoluted Landau Gaussian function
    //Double_t langaufun(Double_t *x, Double_t *par);

  private:
    // Data members
    std::string m_tae;

    LHCb::VeloClusters* m_clusters;
    std::vector<unsigned int> m_nClustersPerSensor;
    
    // Histograms
    AIDA::IHistogram1D* m_hNCluEvt;
    AIDA::IHistogram1D* m_hNCluEvtZoom;
    AIDA::IHistogram1D* m_hNClustersHM;
    AIDA::IHistogram1D* m_hCluSize;
    AIDA::IHistogram2D* m_hCluSizeSens;
    AIDA::IHistogram1D* m_hCluADC_low;
    AIDA::IHistogram2D* m_hCluADCSens;
    AIDA::IHistogram1D* m_hCluADCR;
    AIDA::IHistogram1D* m_hCluADCPhi;
    AIDA::IHistogram1D* m_hSeedADC;
    AIDA::IHistogram1D* m_hSeedADCR;
    AIDA::IHistogram1D* m_hSeedADCPhi;
    AIDA::IHistogram1D* m_hIncADC;
    AIDA::IHistogram1D* m_hIncADCR;
    AIDA::IHistogram1D* m_hIncADCPhi;
    AIDA::IHistogram2D* m_hActiveLinkSens;
    AIDA::IHistogram1D* m_hNCluEvt_Sensor[84];
    TH1D* m_histCluADC;
    TH1D* m_histCluADC_Sensor[84];
    TH1D* m_histCluADC_FitParLandauWidth;
    TH1D* m_histCluADC_FitParMPV;
    TH1D* m_histCluADC_FitParArea;
    TH1D* m_histCluADC_FitParGSigma;
    TH1D* m_histCluADC_Sensor_FitParLandauWidth;
    TH1D* m_histCluADC_Sensor_FitParMPV;
    TH1D* m_histCluADC_Sensor_FitParArea;
    TH1D* m_histCluADC_Sensor_FitParGSigma;
    TH1D* m_histCluADC_Sensor_MPV;
    TH1D* m_histCluADC_Sensor_FWHM;
    TF1* func[84];
    TF1* func_all;

    std::vector<AIDA::IHistogram1D*> m_hNCluSens;
    
    // Job options
    std::string m_clusterCont;
    bool        m_perSensorPlots;
    bool        m_highMultiplicityPlot;
  };
}

#endif // VELORECMONITORS_VELOCLUSTERMONITOR_H
