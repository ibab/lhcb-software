// $Id: DummyLumiAlley.h,v 1.3 2008-08-13 16:03:52 panmanj Exp $
#ifndef DUMMYLUMIALLEY_H 
#define DUMMYLUMIALLEY_H 1

// Include files
// from Gaudi


#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"

/** @class DummyLumiAlley DummyLumiAlley.h
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
class DummyLumiAlley : public HltAlgorithm {
public: 
  /// Standard constructor
  DummyLumiAlley( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DummyLumiAlley( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  Hlt::SelectionContainer0 m_selection;

};
#endif // DUMMYLUMIALLEY_H
