#ifndef CHARGEDPARTICLESTOTRACKS_H 
#define CHARGEDPARTICLESTOTRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

#include "Event/Track.h"
#include "Event/Particle.h"

#include <string>
#include <vector>
#include <map>

class ITrackFitter;

/** @class ChargedParticlesToTracks ChargedParticlesToTracks.h
 *  
 *
 *  @author Frederic Dupertuis
 *  @date   2012-10-08
 */
class ChargedParticlesToTracks : public GaudiHistoAlg {
public: 
  /// Standard constructor
  ChargedParticlesToTracks( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~ChargedParticlesToTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  void DumpTracks(LHCb::Particle::ConstVector particles);
  
private:

  bool m_refit;

  float m_masswindow;
     
  ITrackFitter* m_trackFit;
  ITrackFitter* m_trackPreFit;

  std::vector<std::string> m_partloc;
  std::string m_trackOutputLocation;
  
  std::map<std::string,std::string> m_linesname;

  LHCb::Track::Container* m_trackCont;
};
#endif // CHARGEDPARTICLESTOTRACKS_H
