#ifndef _TrackEraseExtraInfo_H_
#define _TrackEraseExtraInfo_H_

/** @class TrackEraseExtraInfo TrackEraseExtraInfo.h
 *
 *
 *  @author S. Hansmann-Menzemer
 *  @date   20.07.2009
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>
#include <vector>
#include <map>


class TrackEraseExtraInfo: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackEraseExtraInfo(const std::string& name,
		      ISvcLocator* pSvcLocator);
  virtual ~TrackEraseExtraInfo();

  virtual StatusCode initialize();

  virtual StatusCode execute();

private:

  std::string m_inputLocation;
   
};

#endif
