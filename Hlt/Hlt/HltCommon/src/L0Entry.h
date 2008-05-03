// $Id: L0Entry.h,v 1.9 2008-05-03 15:19:53 graven Exp $
#ifndef L0ENTRY_H 
#define L0ENTRY_H 1

// Include files
// from Gaudi


#include "Event/HltSummary.h"
#include "Event/L0DUReport.h"
#include "HltBase/HltAlgorithm.h"

/** @class L0Entry L0Entry.h
 *  
 *  functionality:
 *      give pass if the L0 has fired a given decision (controlled via options)
 *      if monitor, make histogram with the L0 decisions
 *
 *  Options:
 *      L0ChannelNames: list of names of the L0 decision to accept
 *                      if no names are listed, all decision are accepted
 *         names valid: "hadron","muon","dimuon", "electron","photon",
                        "localPi0","globalPi0" 
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

private:

  std::string m_l0Location;

  StringArrayProperty m_l0ChannelsName;

  Hlt::Histo* m_histoL0;

  std::vector<int> m_l0Channels;

  void defineL0();

};
#endif // L0ENTRY_H
