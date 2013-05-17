// $Id: HltGuidedForward.h,v 1.1 2009-05-30 12:40:21 graven Exp $
#ifndef HLTGUIDEDFORWARD_H 
#define HLTGUIDEDFORWARD_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"            // Interface

#include "TfKernel/TStationHitManager.h"
#include "PatKernel/PatForwardHit.h"
#include "HltBase/IL0ConfExtrapolator.h"
#include "HltBase/FwdHypothesis.h"

/** @class HltGuidedForward HltGuidedForward.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2008-04-08
 */
class HltGuidedForward : public GaudiTool, virtual public ITracksFromTrack {
public: 
  /// Standard constructor
  HltGuidedForward( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~HltGuidedForward( ); ///< Destructor

  virtual StatusCode initialize();

  virtual StatusCode tracksFromTrack(const LHCb::Track& track0, 
                                     std::vector<LHCb::Track*>& tracks);
  
protected:

  double m_nSigma;

  IL0ConfExtrapolator* m_l0ConfExtrapolator;
  Tf::TStationHitManager<PatForwardHit>* m_hitMan;
  ITracksFromTrack* m_forwardTool;
};
#endif // HLTGUIDEDFORWARD_H
