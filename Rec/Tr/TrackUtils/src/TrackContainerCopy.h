// $Id: TrackContainerCopy.h,v 1.2 2006-07-27 12:01:54 cattanem Exp $
#ifndef _TrackContainerCopy_H_
#define _TrackContainerCopy_H_

/** @class TrackContainerCopy TrackContainerCopy Tr/TrackContainerCopy.h
 *
 *  Copy a container of tracks. By default do not copy tracks that failed the fit
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>


class TrackContainerCopy: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackContainerCopy(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackContainerCopy();

  virtual StatusCode execute();

private:

  std::string m_inputLocation;
  std::string m_outputLocation;
  bool        m_copyFailures;   ///< If true, copy also tracks that failed fit  
};

#endif
