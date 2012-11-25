#ifndef TracksFromParticles_H_
#define TracksFromParticles_H_

#include "Kernel/DaVinciAlgorithm.h"
#include "Event/TrackTypes.h"

/** @class TracksFromParticles TracksFromParticles.h
 *  
 *  Algorithm to create Tracks from Particles object
 * 
 *  @author M. Needham
 *  @date   2012-04-27
 */

namespace LHCb{
  class Track;
}


class TracksFromParticles: public DaVinciAlgorithm{

 public:

  /// destructor  
  virtual ~TracksFromParticles();

  /// constructor
  TracksFromParticles(const std::string& name, ISvcLocator* pSvc);
  
  /// execute
  virtual StatusCode execute();


 private:

 
  std::string m_outputLocation;

 
};




#endif
