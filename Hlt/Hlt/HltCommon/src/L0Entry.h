// $Id: L0Entry.h,v 1.1 2006-09-22 11:36:11 hernando Exp $
#ifndef L0ENTRY_H 
#define L0ENTRY_H 1

// Include files
// from Gaudi


#include "Event/HltSummary.h"
#include "Event/L0DUReport.h"
#include "HltCommon/HltAlgorithm.h"

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

};
#endif // L0ENTRY_H
