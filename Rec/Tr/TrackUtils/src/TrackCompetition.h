// $Id: TrackCompetition.h,v 1.1 2008-11-29 10:26:54 mneedham Exp $
#ifndef _TrackCompetition_H_
#define _TrackCompetition_H_

/** @class TrackCompetition TrackCompetition.h
 *
 *  Copy a container of tracks. By default do not copy tracks that failed the fit
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>

namespace LHCb{
  class Track;
}

class TrackCompetition: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackCompetition(const std::string& name,
              ISvcLocator* pSvcLocator);

  virtual ~TrackCompetition();

  virtual StatusCode execute();

private:

  std::string m_inputLocation;
  std::string m_outputLocation;
  double m_fracUsed;
 
};

#endif
