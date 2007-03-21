// $Id: TTrackFromLong.h,v 1.2 2007-03-21 09:25:14 cattanem Exp $
#ifndef _TTrackFromLong_H_
#define _TTrackFromLong_H_

/** @class TTrackFromLong TTrackFromLong.h
 *
 *  Fake a T seed from a long track 
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"

#include <string>
#include <vector>

namespace LHCb{
  class Track;
}

class TTrackFromLong: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TTrackFromLong(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TTrackFromLong();

  virtual StatusCode execute();

private:

  LHCb::Track* convert(const LHCb::Track* aTrack) const;

  std::string m_inputLocation;
  std::string m_outputLocation;


};

#endif
