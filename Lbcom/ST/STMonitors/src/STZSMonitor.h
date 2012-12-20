// $Id: STZSMonitor.h,v 1.2 2009-05-05 11:59:04 mtobin Exp $
#ifndef PUBLIC_STZSMONITOR_H 
#define PUBLIC_STZSMONITOR_H 1
//===========================================


//===========================================
// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"
#include <map>
#include "Kernel/ISTReadoutTool.h"



class ISTReadoutTool;
class DeSTDetector;



/** @class STZSMonitor STZSMonitor.h public/STZSMonitor.h
 *   
 *
 *  @author Mathias Knecht
 *  @date   2008-May-6
 */
class STZSMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  STZSMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STZSMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

  std::string m_readoutToolName;
  ISTReadoutTool* m_readoutTool;
  DeSTDetector* m_tracker;
  IHistogramSvc* m_histosvc;
  
  // service box mapping (for histograms per service boxes)  
  std::map<std::string,int> m_SvceBoxMap;
  
  // related to options
  std::string m_detType;  
  std::string m_inputLocation;
  std::string m_TAEtag;
  int m_skippedEvents;
  bool m_maskFirstChannel;
  bool m_maskLastChannel;
  
 
  // related to histograms
  int m_evtCounter; 
  int m_TotalCharge;
 
 
  // HISTOGRAMS:
  
  // general histograms for full detector
  AIDA::IHistogram1D *evt_number;	// event number 
  AIDA::IHistogram1D *CLUSTERCOUNT; 	// cluster number for the event
  AIDA::IHistogram1D *CLUSTERCHARGESUM; // Average charge of the event (all clusters charge summed divided by number of clusters)

  // specific histograms, for full detector
  AIDA::IHistogram1D *CLUSTERSIZE;	// cluster size (number of strips hit)
  AIDA::IHistogram1D *TOTALCHARGE;	// distributions of the charge (landau)	
  AIDA::IHistogram1D *HIGHTHRESHOLD;	// high threshold 
  AIDA::IHistogram1D *NEIGHBOURSUM;	// neighbour strips sum
  AIDA::IHistogram1D *RELNEIGHBOURSUM;	// relative neighbour strips sum	
  
  // Per station  
  AIDA::IHistogram1D *CLUSTERSIZESTA[3];	// same as above, but per station
  AIDA::IHistogram1D *TOTALCHARGESTA[3];	// same as above, but per station
  AIDA::IHistogram1D *HIGHTHRESHOLDSTA[3];	// same as above, but per station	
  AIDA::IHistogram1D *NEIGHBOURSUMSTA[3];	// same as above, but per station
  AIDA::IHistogram1D *RELNEIGHBOURSUMSTA[3];	// same as above, but per station
  
  // Per box for IT, and per detector region for TT
  AIDA::IHistogram1D *CLUSTERSIZEBOX[3][4];	// same as above, but per box (IT) and detector region (TT)
  AIDA::IHistogram1D *TOTALCHARGEBOX[3][4];	// same as above, but per box (IT) and detector region (TT)
  AIDA::IHistogram1D *HIGHTHRESHOLDBOX[3][4];	// same as above, but per box (IT) and detector region (TT)
  AIDA::IHistogram1D *NEIGHBOURSUMBOX[3][4];	// same as above, but per box (IT) and detector region (TT)
  AIDA::IHistogram1D *RELNEIGHBOURSUMBOX[3][4];	// same as above, but per box (IT) and detector region (TT)
  
    
  // Occupancies (per layer in a box for IT, per readout sector for TT)   
  AIDA::IHistogram1D *OCCUPANCYIT[3][4][4];		// Occupancies (clusters per strip), per layer in a box (7 modules) for IT, 
  AIDA::IHistogram1D *OCCUPANCYTT[2][3][2][26];		// and per readout sector for TT

  
  // Charge per service box, only for IT (24 service boxes). for the timing
  AIDA::IHistogram1D *TOTALCHARGESVCEBOX[24];


  // Cluster maps (custom layout for IT, x-y plane strip mid-point for TT)
  AIDA::IHistogram2D *CLUSTERMAPIT;	
  AIDA::IHistogram2D *CLUSTERMAPTT;
  AIDA::IHistogram2D *CLUSTERMAPTTSta1Lay1;
  AIDA::IHistogram2D *CLUSTERMAPTTSta1Lay2;
  AIDA::IHistogram2D *CLUSTERMAPTTSta2Lay1;
  AIDA::IHistogram2D *CLUSTERMAPTTSta2Lay2;


 };
#endif // PUBLIC_STZSMONITOR_H
