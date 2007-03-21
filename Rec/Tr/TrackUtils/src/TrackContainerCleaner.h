// $Id: TrackContainerCleaner.h,v 1.2 2007-03-21 09:25:14 cattanem Exp $
#ifndef _TrackContainerCleaner_H_
#define _TrackContainerCleaner_H_

/** @class TrackContainerCleaner TrackContainerCleaner.h
 *
 *  Clean out tracks with some criteria from the container
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"

#include <string>
#include <vector>

class TrackContainerCleaner: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackContainerCleaner(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackContainerCleaner();

  virtual StatusCode initialize();

  virtual StatusCode execute();

private:

  std::string m_inputLocation;

  std::string m_selectorName;
  ITrackSelector* m_selector;

};

#endif
