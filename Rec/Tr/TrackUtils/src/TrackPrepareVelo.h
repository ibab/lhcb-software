// $Id: TrackPrepareVelo.h,v 1.3 2006-10-23 12:20:48 erodrigu Exp $
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

  virtual StatusCode initialize();
  virtual StatusCode execute();

private:

  bool used(const LHCb::Track* aTrack, const LHCb::Tracks* bestCont) const;
  void prepare(LHCb::Track* aTrack, const int i) const;

  bool m_ignoreBest;

  std::string m_inputLocation;
  std::string m_outputLocation;
  std::string m_bestLocation;
  double      m_ptVelo;

};

#endif
