#ifndef _IGhostProbability_H
#define _IGhostProbability_H
 
#include "GaudiKernel/IAlgTool.h"
 
static const InterfaceID IID_IGhostProbability( "IGhostProbability", 0, 0 );
 
/** @class IGhostProbability
 *
 *  interface for the ghost probability calculation
 *
 *  @author P.Seyfert
 *  @date   27/01/2015
 */

namespace LHCb{
 class Track;
}
  
class IGhostProbability: virtual public IAlgTool{
 
public:
                                                                                
  /** Retrieve interface ID */
  static const InterfaceID& interfaceID() { return IID_IGhostProbability ; }                                                                                                
  /** Add the reference information */
  virtual StatusCode execute(LHCb::Track& aTrack) const = 0;

  /** consider this the beginning of a new event */
  virtual StatusCode beginEvent() = 0;

  /** reveal the variable names for a track type */
  virtual std::vector<std::string> variableNames(LHCb::Track::Types type) const = 0;

  /** reveal the variable values for a track */
  virtual std::vector<float> netInputs(LHCb::Track& aTrack) const = 0;
};
                                                                                
#endif
