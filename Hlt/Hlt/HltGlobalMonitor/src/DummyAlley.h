// $Id: DummyAlley.h,v 1.3 2008-08-19 09:42:38 graven Exp $
#ifndef DUMMYALLEY_H 
#define DUMMYALLEY_H 1

// Include files
// from Gaudi


#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"

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

private:

  std::string m_l0Location;
  int m_RightType;
  int m_WrongType;

  StringArrayProperty m_l0ChannelsName;
  Hlt::SelectionContainer0 m_selection;

};
#endif // DUMMYALLEY_H
