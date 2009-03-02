// $Id: STNZSMonitor.h,v 1.1.1.1 2009-03-02 19:13:44 mtobin Exp $
#ifndef PUBLIC_STNZSMONITOR_H 
#define PUBLIC_STNZSMONITOR_H 1
//===========================================


//===========================================
// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


#include "GaudiKernel/IHistogramSvc.h"


#include <map>

#include "Kernel/ISTReadoutTool.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"


class ISTReadoutTool;


/** @class STNZSMonitor STNZSMonitor.h public/STNZSMonitor.h
 *   
 *
 *  @author Mathias Knecht
 *  @date   2007-10-03
 */
class STNZSMonitor : public GaudiAlgorithm {
public: 
  /// Standard constructor
  STNZSMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STNZSMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
 
  // options
  std::string m_inputLocation;
  std::string m_LCMSADCs;
  std::string m_PedSubADCs; 
  bool m_disable2D;
  std::string m_readoutToolName;
  std::string m_detType;
  float m_NoiseIncr;
  int m_ConvLimit;
  bool m_OnlineMode;
 
  bool toberesettedIT[8];
  bool toberesettedTT[6];
  
  int iBeetleIT;
  int iBeetleTT;
  int iModuleIT;
  int iSectorTT;
  

  ISTReadoutTool*   m_readoutTool;
  bool              m_useLink[48][24];
  int               maskedpositive[24];
  int               evt_counter;
  IHistogramSvc*    m_histosvc;
  
  std::map<unsigned int, int> m_map;
  unsigned int q;

  
  int cmsADC[48][24][128];
  int cmsADCsq[48][24][128];
  
  int absADC[48][24][128];
 	
 
  int pedsubADC[48][24][128];	
  int pedsubADCsq[48][24][128];
  
  

  float noise_PedSub[48][24][128];
  float noise_CMS[48][24][128];
  float noise_Pulsed[48][24][128];
  int mean[48][24][128];
  int pedestals[48][24][128];
 
 
  // NON SPECIFIC HISTOS
  AIDA::IHistogram1D *evt_number;
  AIDA::IHistogram2D *RAWNOISEBOARD[48]; 
  AIDA::IHistogram2D *CMSNOISEBOARD[48]; 
  AIDA::IHistogram1D *CMSNOISEDISTRBOARD[48]; 
  AIDA::IHistogram2D *MEANBOARD[48];  
  AIDA::IHistogram1D *MEANDISTRBOARD[48];  

  AIDA::IHistogram2D *PCNDISTR[48]; 
  AIDA::IHistogram1D *OPTLINKNOEVT[48];
  AIDA::IHistogram1D *SYNCEVTSIZERROR[48];
  AIDA::IHistogram1D *OPTLNKNOCLOCK[48]; 
  AIDA::IHistogram1D *TLKLNKLOSS[48];
  AIDA::IHistogram1D *SYNCRAMFULL[48];
  AIDA::IHistogram1D *PCNERROR[48]; 
 
  // SPECIFIC HISTOS
  
  AIDA::IHistogram1D *CMSNOISEDISTRMODIT[48][8];
  AIDA::IHistogram1D *CMSNOISEDISTRMODTT[48][6]; 
 
  AIDA::IHistogram1D *CMSNOISEIT[48][8];
  AIDA::IHistogram1D *CMSNOISETT[48][6]; 

  AIDA::IHistogram1D *RAWNOISEIT[48][8];
  AIDA::IHistogram1D *RAWNOISETT[48][6]; 
 
  AIDA::IHistogram1D *MEANIT[48][8];
  AIDA::IHistogram1D *MEANTT[48][6];
 
  AIDA::IHistogram1D *SoverNIT[48][8];
  AIDA::IHistogram1D *SoverNTT[48][6];
 
  AIDA::IHistogram1D *PEDESTALIT[48][8];
  AIDA::IHistogram1D *PEDESTALTT[48][6];
 
  AIDA::IHistogram1D *RAWADCIT[48][8];
  AIDA::IHistogram1D *RAWADCTT[48][6];
   
  AIDA::IHistogram1D *HEADERIT[48][8];
  AIDA::IHistogram1D *HEADERTT[48][6];

   AIDA::IHistogram2D *LCMSIT[48][8];
  AIDA::IHistogram2D *LCMSTT[48][6];
 
};
#endif // PUBLIC_STMONITOR_H
