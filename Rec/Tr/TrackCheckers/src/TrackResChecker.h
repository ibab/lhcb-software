// $Id: TrackResChecker.h,v 1.6 2008-12-04 09:03:55 cattanem Exp $
#ifndef TRACKRESCHECKER_H
#define TRACKRESCHECKER_H 1
 
// Include files
#include <map>
#include <string>

#include "TrackCheckerBase.h"

// linkers
#include "Linker/LinkedTo.h"

#include "Event/MCParticle.h"

class ITrackProjectorSelector;
class IHistoTool ;

namespace LHCb{
  class Track;
  class State;
  class OTMeasurement;
}

/** @class TrackResChecker TrackResChecker.h
 * 
 * Class for track monitoring
 *  @author M. Needham.
 *  @date   6-5-2007
 */                 
                                                           
class TrackResChecker : public TrackCheckerBase {
                                                                             
public:
                                                                             
  /** Standard constructor */
  TrackResChecker( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~TrackResChecker();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm execute */
  virtual StatusCode execute();

  /** Algorithm finalize */
  virtual StatusCode finalize();
 

private:
  
  void resolutionHistos(const IHistoTool& histotool,
			const LHCb::Track& track, 
			const LHCb::MCParticle& mcPart) const ;
  
  void pullplots(const IHistoTool& histotool,
		 const LHCb::State& trueState, const LHCb::State& recState,
		 const std::string& location) const;
  
  void checkAmbiguity(const IHistoTool& histotool,
		      const LHCb::Track& track, 
		      const LHCb::MCParticle& mcPart) const;
  
  void plotsByMeasType(const IHistoTool& histotool,
		       const LHCb::Track& track, 
		       const LHCb::MCParticle& mcPart ) const;

  const IHistoTool* createHistoTool( const std::string& name ) const ;

private:

  bool m_plotsByMeasType;
  
  typedef LinkedTo<LHCb::MCParticle> OTLinks;
  mutable OTLinks m_otLinker;
  unsigned int  m_minToCountAmb;
  bool m_checkAmbiguity;
  double m_minAmbDist;
  
  ITrackProjectorSelector* m_projectorSelector;
  //typedef std::map< int, const ITrackResCheckerTool*> ToolMap ;
  //ToolMap m_toolmap ;
  typedef std::map< int, const IHistoTool*> HistoToolMap ;
  HistoToolMap m_histoTools ;
  
};

#endif // TRACKRESCHECKER_H
