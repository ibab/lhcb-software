// $Id: VeloSamplingMonitor.h,v 1.1 2008-08-21 11:39:38 erodrigu Exp $
#ifndef VELORECMONITORS_VELOSAMPLINGMONITOR_H 
#define VELORECMONITORS_VELOSAMPLINGMONITOR_H 1

// Include files
// -------------

// from VeloEvent
#include "VeloEvent/VeloTELL1Data.h"

// from DigiEvent
#include "Event/VeloCluster.h"

// local
#include "VeloMonitorBase.h"

/** @class VeloSamplingMonitor VeloSamplingMonitor.h
 *  
 *
 *  @author Eduardo Rodrigues
 *  @date   2008-08-20
 */

namespace Velo
{
  
  class VeloSamplingMonitor : public VeloMonitorBase {

  public: 
    /// Standard constructor
    VeloSamplingMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloSamplingMonitor( );    ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
  private:
    
    // Retrieve the VeloClusters
    LHCb::VeloClusters* veloClusters( std::string samplingLocation );
    
    // Retrieve the VeloTell1Data
    LHCb::VeloTELL1Datas* veloTell1Data( std::string samplingLocation );

    // Monitor the VeloClusters
    void monitorClusters( std::string samplingLocation,
                          LHCb::VeloClusters* clusters );

    // Monitor the VeloTell1Data
    void monitorTell1Data( std::string samplingLocation,
                           LHCb::VeloTELL1Datas* tell1Datas );

    // Data members

    // Job options
    std::vector<std::string> m_samplingLocations;

  };
  
}

#endif // VELORECMONITORS_VELOSAMPLINGMONITOR_H
