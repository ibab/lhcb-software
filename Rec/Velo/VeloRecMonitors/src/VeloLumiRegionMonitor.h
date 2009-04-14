// $Id: VeloLumiRegionMonitor.h,v 1.2 2009-04-14 11:50:04 mjohn Exp $
#ifndef VELORECMONITOR_VELOLUMIREGIONMONITOR_H 
#define VELORECMONITOR_VELOLUMIREGIONMONITOR_H 1

#include <deque>

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiUtils/Aida2ROOT.h"

// from LHCb
#include "Event/ODIN.h"

// ROOT
#include "TH2F.h"
#include "TProfile.h"

// local
#include "VeloMonitorBase.h"

namespace Velo
{
  
  class VeloLumiRegionMonitor : public VeloMonitorBase {
  public: 
    /// Standard constructor
    VeloLumiRegionMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloLumiRegionMonitor( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
  private:
    LHCb::ODIN* m_odin;

    // Monitor the VeloLumiRegion
    void monitorLumiRegion();
    void genExampleLumiRegion();
    void calculateSlopesInVertexList();
    void updateDataPoints();

    TH1D* hX;
    TH1D* hY;
    TH1D* hZ;
    TH1D* hNtrks;
    TH1D* hNtrksU;
    TH1D* hX_cumu;
    TH1D* hY_cumu;
    TH1D* hZ_cumu;
    TProfile* hX_prof;
    TProfile* hY_prof;
    TProfile* hZ_prof;
    TH1D* hX_hist;
    TH1D* hY_hist;
    TH1D* hZ_hist;
    TH1D* hXZ_hist;
    TH1D* hYZ_hist;

    double d_vertexDistributionMeanX;
    double d_vertexDistributionMeanY;
    double d_vertexDistributionMeanZ;
    double d_vertexDistributionRMSX;
    double d_vertexDistributionRMSY;
    double d_vertexDistributionRMSZ;
    double d_vertexDistributionGMeanX;
    double d_vertexDistributionGMeanY;
    double d_vertexDistributionGMeanZ;
    double d_vertexDistributionSigmaX;
    double d_vertexDistributionSigmaY;
    double d_vertexDistributionSigmaZ;
    double d_vertexDistributionSlopeXZ;
    double d_vertexDistributionSlopeYZ;
    double d_vertexDistributionSlopeErrorXZ;
    double d_vertexDistributionSlopeErrorYZ;

    std::vector< std::pair<Gaudi::XYZPoint,int> > vertexList;

    std::string s_correspondingHltLineName;
    std::string s_vertexReportLocation;
    double d_generatedX;
    double d_refreshRate;
    int i_minTracks;
    int n_acceptableVertices;
    Rndm::Numbers m_rndmGaus;
    Rndm::Numbers m_rndmExpo;
    bool b_genData;

    long i_counterEx;
    long i_counterAl;

    std::deque< std::pair<double,double> > vX_hist;
    std::deque< std::pair<double,double> > vY_hist;
    std::deque< std::pair<double,double> > vZ_hist;
    std::deque< std::pair<double,double> > vXZ_hist;
    std::deque< std::pair<double,double> > vYZ_hist;
  };
}

#endif // VELORECMONITOR_VELOLUMIREGIONMONITOR_H
