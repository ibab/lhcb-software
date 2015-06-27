#ifndef _TrackEraseExtraInfo_H_
#define _TrackEraseExtraInfo_H_

/** @class TrackEraseExtraInfo TrackEraseExtraInfo.h
 *  Algorithm that removes extra info from tracks such that it is not written to DST
 *
 *  Parameters:
 * - InputLocation: Input location for tracks.
 * - ErasableInfo: List of extra infos to erase.
 * - PrintExtraInfo: Print the extra info on the track before erasure.
 *
 *  @author S. Hansmann-Menzemer
 *  @date   20.07.2009
 *
 *  @author Michel De Cian
 *  @date   2015-06-27
 */

#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>
#include <vector>


class TrackEraseExtraInfo: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackEraseExtraInfo(const std::string& name,
		      ISvcLocator* pSvcLocator);
  virtual ~TrackEraseExtraInfo();

  virtual StatusCode initialize();

  virtual StatusCode execute();

private:

  std::string      m_inputLocation;
  std::vector<int> m_erasableInfo;
  bool             m_printExtraInfo;
  
  
};

#endif
