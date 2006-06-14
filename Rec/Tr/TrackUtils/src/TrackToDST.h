// $Id: TrackToDST.h,v 1.1 2006-06-14 15:03:47 mneedham Exp $
#ifndef _TrackToDST_H_
#define _TrackToDST_H_

/** @class TrackToDST TrackToDST Tr/TrackToDST.h
 *
 *  Fake a T seed from a long track 
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>

namespace LHCb{
  class Track;
}

class TrackToDST: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackToDST(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackToDST();

  virtual StatusCode execute();

private:

  void cleanStates(LHCb::Track* aTrack) const;
  std::string m_inputLocation;
  bool m_allStates;  

};

#endif
