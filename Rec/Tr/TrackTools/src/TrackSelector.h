// $Id: TrackSelector.h,v 1.1 2006-02-21 14:54:17 mneedham Exp $
#ifndef _TrackSelector_H
#define _TrackSelector_H

/** @class TrackSelector TrackSelector TrackTools/TrackSelector.h
 *
 * General track Selection tool
 * you can cut on p, hits, chi^2, pt (what more do you want)
 * default no cut
 *
 *  @author M.Needham
 *  @date   30/12/2005
 */
  
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackSelector.h"
#include <string>

namespace LHCb{
 class Track;
};
 
class TrackSelector: public GaudiTool, virtual public ITrackSelector {

public:

  /// constructer
  TrackSelector(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);
                                                                                
  virtual ~TrackSelector();
                                                                        
  /// the method
  virtual bool accept(const LHCb::Track& aTrack) const;

private: 

  double weightedMeasurementSum(const LHCb::Track& aTrack) const;

  double m_vWeight;
  double m_oWeight;
  double m_iWeight;

  double m_chi2Cut;
  double m_pCut;
  double m_ptCut;
  double m_hitCut;
                                                                              
};

#endif
