// $Id: TrackToDST.h,v 1.5 2008-01-29 20:25:34 smenzeme Exp $
#ifndef _TrackToDST_H_
#define _TrackToDST_H_

/** @class TrackToDST TrackToDST.h
 *
 *  Fake a T seed from a long track 
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>
#include <map>
#include <string>

#include "Event/State.h"

namespace LHCb{
  class Track;
}

class TrackToDST: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackToDST(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackToDST();

  virtual StatusCode initialize();

  virtual StatusCode execute();

private:
 
  typedef std::vector<LHCb::State::Location> SLocations;

  mutable SLocations m_veloStates;
  mutable SLocations m_longStates;
  mutable SLocations m_upstreamStates;
  mutable SLocations m_downstreamStates;
  mutable SLocations m_tStates;
  mutable SLocations m_muonStates;

  typedef std::vector<std::string> Strings;
  Strings m_veloStrings;
  Strings m_longStrings;
  Strings m_upstreamStrings;
  Strings m_downstreamStrings;
  Strings m_tStrings;
  Strings m_muonStrings;

  void cleanStates(LHCb::Track* aTrack,  const SLocations& loc) const;
  void stringToLoc(const Strings& sCont, SLocations& loc) const;

  std::string m_inputLocation;
  bool m_storeAllStates;  

  typedef std::map<std::string, LHCb::State::Location> StateMap;
  StateMap m_theMap;

};

#endif
