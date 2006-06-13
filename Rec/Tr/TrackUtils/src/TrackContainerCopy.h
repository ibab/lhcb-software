// $Id: TrackContainerCopy.h,v 1.1 2006-06-13 12:24:14 mneedham Exp $
#ifndef _TrackContainerCopy_H_
#define _TrackContainerCopy_H_

/** @class TrackContainerCopy TrackContainerCopy Tr/TrackContainerCopy.h
 *
 *  Fake a T seed from a long track 
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

};

#endif
