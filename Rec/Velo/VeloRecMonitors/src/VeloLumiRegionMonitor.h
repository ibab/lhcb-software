// $Id: VeloLumiRegionMonitor.h,v 1.1 2008-08-31 15:52:08 krinnert Exp $
#ifndef VELORECMONITOR_VELOLUMIREGIONMONITOR_H 
#define VELORECMONITOR_VELOLUMIREGIONMONITOR_H 1

// Include files
// -------------
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiUtils/Aida2ROOT.h"

// ROOT
#include "TH1F.h"

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

    // Monitor the VeloLumiRegion
    void monitorLumiRegion();

    double d_vertexDistributionMeanX;
    double d_vertexDistributionMeanY;
    double d_vertexDistributionMeanZ;
    double d_vertexDistributionSigmaX;
    double d_vertexDistributionSigmaY;
    double d_vertexDistributionSigmaZ;
    
    Rndm::Numbers m_rndm;
  };
}

#endif // VELORECMONITOR_VELOLUMIREGIONMONITOR_H
