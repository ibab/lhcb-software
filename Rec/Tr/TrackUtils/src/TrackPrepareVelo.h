// $id: TrackPrepareVelo.h,v 1.4 2007/03/21 09:25:14 cattanem Exp $
#ifndef _TrackPrepareVelo_H_
#define _TrackPrepareVelo_H_

/** @class TrackPrepareVelo TrackPrepareVelo.h
 *
 *  Fake a T seed from a long track 
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/Track.h"

#include <string>

class TrackPrepareVelo: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackPrepareVelo(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackPrepareVelo();

  virtual StatusCode initialize();
  virtual StatusCode execute();

private:

  /// see if the track was used as an ancestor to one in bestCont
  bool used(const LHCb::Track* aTrack, const LHCb::Tracks* bestCont) const;
  /// set the states on the track with the required q/p
  void prepare(LHCb::Track* aTrack, const int i) const;

  bool m_ignoreBest;           ///< set by making m_bestLocation==""

  std::string m_inputLocation; ///< input TES location 
  std::string m_outputLocation;///< output TES location 
  std::string m_bestLocation;  ///< location to look for tracks already used and skip here
  double      m_ptVelo;        ///< pT to assign to tracks
  bool        m_reverseCharge; ///< Reverse the even=- odd=+ assigned charge

};

#endif
