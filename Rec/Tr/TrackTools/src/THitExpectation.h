// $Id: THitExpectation.h,v 1.2 2007-10-10 18:32:17 smenzeme Exp $
#ifndef _THitExpectation_H
#define _THitExpectation_H

/** @class THitExpectation THitExpectation.h
 *
 * Implementation of TTHitExpectation tool
 * see interface header for description
 *
 *  @author M.Needham
 *  @date   22/5/2007
 */

#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IHitExpectation.h"

#include <string>

namespace LHCb{
  class Track;
  class State;
}



class ITrackExtrapolator;
class IMagneticFieldSvc;

class THitExpectation: public GaudiTool,
                       virtual public IHitExpectation  {

public:

  /** constructer */
  THitExpectation(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);  


  /** intialize */
  StatusCode initialize();

protected:

  Tf::Tsa::Parabola xParabola(const LHCb::Track& aTrack, const double z) const;

  Tf::Tsa::Line xLine(const LHCb::Track& aTrack, const double z) const;

  Tf::Tsa::Line yLine(const LHCb::Track& aTrack, const double z) const;

  ITrackExtrapolator* m_extrapolator;

private:

  double curvature(const LHCb::State& aState) const;

  IMagneticFieldSvc* m_pIMF;

};



#endif
