// $Id: HltTrackFit.h,v 1.1 2008-11-03 08:10:27 albrecht Exp $
#ifndef HLTTRACKFIT_H 
#define HLTTRACKFIT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"            // Interface
#include "TrackInterfaces/ITrackFitter.h"

/** @class HltTrackFit HltTrackFit.h
 *
 *  Class to do a track fit for the HLT. To be called via HltTrackUpgrade  
 *  with RecoName = "FitTrack"
 *
 *  The idea is to use the fast track fit, to be configured via options. 
 *  Subdetectors can be switched off via options.
 *  (if you just run this tool, you get the offline fit)
 *
 *
 *  @author Johannes Albrecht
 *  @date   2008-10-31
 */
class HltTrackFit : public GaudiTool, virtual public ITracksFromTrack {
public: 
  /// Standard constructor
  HltTrackFit( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~HltTrackFit( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize();

  virtual StatusCode tracksFromTrack( const LHCb::Track& seed,
                                      std::vector<LHCb::Track*>& tracks );
  
protected:

private:

  std::string m_fitName;
  ITrackFitter* m_fit;

};
#endif // HLTTRACKFIT_H
