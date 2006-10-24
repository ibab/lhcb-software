// $Id: L0Entry.h,v 1.3 2006-10-24 09:44:03 hernando Exp $
#ifndef L0ENTRY_H 
#define L0ENTRY_H 1

// Include files
// from Gaudi


#include "Event/HltSummary.h"
#include "Event/L0DUReport.h"
#include "HltBase/HltAlgorithm.h"

/** @class L0Entry L0Entry.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-01-16
 */
class L0Entry : public HltAlgorithm {
public: 
  /// Standard constructor
  L0Entry( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0Entry( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_l0Location;

  StringArrayProperty m_l0ChannelsName;

protected:

  HltHisto m_histoL0;

  LHCb::L0DUReport* m_l0;

  std::vector<int> m_l0Channels;

protected:

  void monitor();
  
  HltHisto m_histoL0EtHad;
  HltHisto m_histoL0Et1Had;  

  HltHisto m_histoL0EtEle;
  HltHisto m_histoL0Et1Ele;  

  HltHisto m_histoL0EtGamma;
  HltHisto m_histoL0Et1Gamma;  

  HltHisto m_histoL0EtPi0L;
  HltHisto m_histoL0Et1Pi0L;  

  HltHisto m_histoL0EtPi0G;
  HltHisto m_histoL0Et1Pi0G;

  HltHisto m_histoL0Et;

  HltHisto m_histoL0SPD;  

};
#endif // L0ENTRY_H
