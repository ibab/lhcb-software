// $Id: VeloSamplingMonitor.h,v 1.4 2009-06-05 13:20:33 krinnert Exp $
#ifndef VELORECMONITORS_VELOSAMPLINGMONITOR_H 
#define VELORECMONITORS_VELOSAMPLINGMONITOR_H 1

// Include files
// -------------
#include "VeloDet/DeVelo.h"

// from VeloEvent
#include "VeloEvent/VeloTELL1Data.h"

// from DigiEvent
#include "Event/VeloCluster.h"

// from ROOT
#include "TH1D.h"
#include "TH2D.h"

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
    
    /// determine whether a sensor is top or bottom
    bool isTop(unsigned int sensorNumber) {
      if(sensorNumber < 64){
        if((sensorNumber%4==0)||(sensorNumber%4==1)){return false;}
        else{return true;}
      } else if ( sensorNumber < 128 ) {
        if((sensorNumber%4==2)||(sensorNumber%4==3)){return false;}
        else{return true;}
      } else {
        if ( sensorNumber == 129 || sensorNumber == 130 ) { return true; }
        else { return false; }
      }
    }
    
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

    // Monitor difference in MPV of ADC between TAEs
    void monitorTAEDiff(); 

    // Data members
    const DeVelo* m_velo;

    TH2D* m_histClusADCSampAll;
    TH1D* m_histTaeADCDiffNext;
    TH1D* m_histTaeADCDiffPrev;
    TH1D* m_histTaeADCDiffDiff;

  
    // Job options
    std::vector<std::string> m_samplingLocations;

  };
  
}

#endif // VELORECMONITORS_VELOSAMPLINGMONITOR_H
