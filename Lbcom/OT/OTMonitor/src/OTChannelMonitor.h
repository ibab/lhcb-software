#ifndef OTCHANNELMONITOR_H 
#define OTCHANNELMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"

#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"
#include "OTDAQ/IOTRawBankDecoder.h"
// histograms 
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"

/** @class OTChannelMonitor OTChannelMonitor.h
 *
 *  @author Francesco Dettori
 *  @date   2012-11-15
 * Algorithm to store histograms for a monitoring of bad channels in the Outer Tracker 
 * It stores one histogram for each otis and one histogram for each layer
 */
class OTChannelMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  OTChannelMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  // Copied from OTTrackMonitor
  // this should be moved to OTChannelID 
  enum {
    nStations=3,
    nLayers=4,
    nQuadrants=4,
    nModules=9,
    nOtis=4,
    NumUniqueStation=3, 
    NumUniqueLayer=12, 
    NumUniqueModule=432, 
    NumUniqueOtis=NumUniqueModule*4,
    NumUniqueMonoLayer = nStations*nLayers*2
  } ;
  inline int uniqueLayer(const LHCb::OTChannelID& channelID)
  {
    return (channelID.station() - 1) * 4 + channelID.layer();
  }

  inline int uniqueQuarter(const LHCb::OTChannelID& channelID)
  {
    return uniqueLayer(channelID) * 4 + channelID.quarter();
  }

  inline int uniqueModule(const LHCb::OTChannelID& channelID)
  {
    return uniqueQuarter(channelID) * 9 + channelID.module() - 1;
  }

  inline int uniqueOtis( const LHCb::OTChannelID& channelID)
  {
    return uniqueModule(channelID) * 4 + (channelID.straw() - 1) / 32;
  }

  void debugChannel(const LHCb::OTChannelID& channelID);
  void scaleHistogram(AIDA::IHistogram1D* hist);
  
    
  virtual ~OTChannelMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  const DeOTDetector* m_otdet;
  size_t m_numEvents;
  bool m_isOnline;
  bool m_scaleHistos;
  
  ToolHandle<IOTRawBankDecoder> m_decoder ;
  
  // HISTOS
  AIDA::IHistogram1D* m_OtisHist[NumUniqueOtis];
  AIDA::IHistogram1D* m_LayerHist[NumUniqueLayer];
  

  std::map<int, std::string> m_otisNames;
  std::map<int, std::string> m_layerNames;

  void getLayerGeometry(int uniqueLayer, std::vector<double> & min_modules_x,  
		   std::vector<double> & min_modules_y);
  double getChannelWeight(const DeOTModule *module, LHCb::OTChannelID aChan);

};
#endif // OTCHANNELMONITOR_H
