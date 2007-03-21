// $Id: TrackPrepareForFit.h,v 1.2 2007-03-21 09:25:14 cattanem Exp $
#ifndef _TrackPrepareForFit_H_
#define _TrackPrepareForFit_H_

/** @class TrackPrepareForFit TrackPrepareForFit.h
 *
 *  Add p (from somewhere...) to a track...
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/Track.h"

#include <string>

class TrackPrepareForFit: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackPrepareForFit(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackPrepareForFit();

  virtual StatusCode execute();

private:


  void prepare(LHCb::Track* aTrack, const int i) const;

  std::string m_inputLocation;
  double      m_ptVelo;
  double m_curvValue;
  bool m_fixP;

};

#endif
