// $Id: LongTrackReferenceCreator.h,v 1.6 2007-11-30 15:18:06 wouter Exp $
#ifndef _LongTrackReferenceCreator_H
#define _LongTrackReferenceCreator_H

/** @class LongTrackReferenceCreator LongTrackReferenceCreator.h
 *
 * Implementation of TrackCaloMatch tool
 * see interface header for description
 *
 *  @author M.Needham
 *  @date   30/12/2005
 */

#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/ITrackManipulator.h"

#include <string>

namespace LHCb{
  class Track;
  class State;
  class Measurement;
}

class ITrackExtrapolator;
class IMagneticFieldSvc;
class ITrajPoca;

class LongTrackReferenceCreator: public GaudiTool,
			     virtual public ITrackManipulator  {

public:

  /** constructer */
  LongTrackReferenceCreator(const std::string& type,
                            const std::string& name,
                            const IInterface* parent);  

  /** destructer */
  virtual ~LongTrackReferenceCreator();

  /** intialize */
  StatusCode initialize();

  /** add reference info to the track */  
  StatusCode execute(LHCb::Track& aTrack) const ;

private:
  ITrackExtrapolator* m_extrapolator;
};

#endif
