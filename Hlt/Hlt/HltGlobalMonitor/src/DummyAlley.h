// $Id: DummyAlley.h,v 1.4 2008-08-29 14:31:55 bdepaula Exp $
#ifndef DUMMYALLEY_H 
#define DUMMYALLEY_H 1

// Include files
// from Gaudi


#include "Event/HltSummary.h"
#include "Event/L0DUReport.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "Kernel/IWriteSelResult.h"

/** @class DummyAlley DummyAlley.h
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
class DummyAlley : public HltAlgorithm {
public: 
  /// Standard constructor
  DummyAlley( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DummyAlley( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  int m_RightType;
  int m_WrongType;

protected:

  std::string m_l0Location;

  StringArrayProperty m_l0ChannelsName;

protected:

  Hlt::SelectionContainer0 m_selection;
  LHCb::L0DUReport* m_l0;

};
#endif // DUMMYALLEY_H
