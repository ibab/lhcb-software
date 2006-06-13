// $Id: TrackPrepareVelo.h,v 1.1 2006-06-13 12:24:14 mneedham Exp $
#ifndef _TrackPrepareVelo_H_
#define _TrackPrepareVelo_H_

/** @class TrackPrepareVelo TrackPrepareVelo Tr/TrackPrepareVelo.h
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

  virtual StatusCode execute();

private:

  bool used(const LHCb::Track* aTrack, const LHCb::Tracks* bestCont) const;
  void prepare(LHCb::Track* aTrack, const unsigned int i) const;

  std::string m_inputLocation;
  std::string m_outputLocation;
  std::string m_bestLocation;
  double m_ptVelo;

};

#endif
