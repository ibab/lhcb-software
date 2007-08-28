// $Id: FilterTrueTracks.h,v 1.1 2007-08-28 13:54:21 pkoppenb Exp $
#ifndef FILTERTRUETRACKS_H 
#define FILTERTRUETRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Linker/LinkerTool.h"

typedef LinkerTool<LHCb::Track,LHCb::MCParticle> Asct;
typedef Asct::DirectType Table ;
typedef Asct::Range Range ;
typedef Table::iterator iter ;
typedef std::vector<std::pair<const LHCb::MCParticle*,bool> > MCParts ;
/** @class FilterTrueTracks FilterTrueTracks.h
 *  
 *  Algorithm that keeps only true tracks associated
 *  to a given decay chain
 *
 *  @author Patrick Koppenburg
 *  @date   2007-08-17
 */
class IMCDecayFinder;

class FilterTrueTracks : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterTrueTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterTrueTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  MCParts finalStates() const;
  bool isSignal( MCParts&, const Range&) const;

private:
  IMCDecayFinder* m_mcDecFinder ;        ///<  MCDecayFinder
  std::string m_mcParticlePath ;         ///< MC truth path
  std::vector< std::string >  m_tracksPath ;  ///< Tracks input paths
  std::string m_outputPath ;  ///< Tracks output paths
  bool m_filterOut ;          ///< Revert logic and keep only non-signal?
  int m_allTracks ;           ///< number of tracks
  int m_savedTracks ;           ///< number of tracks saved
  int m_signalParts ;           ///< number of signal particles
  int m_foundParts ;           ///< number of signal particles found
  
};
#endif // FILTERTRUETRACKS_H
