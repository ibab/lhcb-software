// $Id: FilterTrueTracks.h,v 1.5 2010-06-01 09:44:18 pkoppenb Exp $
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
#include "Event/Track.h"

class FilterTrueTracks : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterTrueTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterTrueTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  MCParts finalStates( ) const;
  bool isSignal(MCParts&, const Range&) const;
  LHCb::Track::ConstVector signalTracks(MCParts&) const ;
  StatusCode save(const LHCb::Track::ConstVector&) const ;

private:

  IMCDecayFinder* m_mcDecFinder ;        ///<  MCDecayFinder
  std::string m_mcParticlePath ;         ///< MC truth path
  std::vector< std::string >  m_tracksPath ;  ///< Tracks input paths
  std::string m_outputPath ;  ///< Tracks output paths
  bool m_filterOut ;          ///< Revert logic and keep only non-signal?
  IParticlePropertySvc* m_ppSvc ;
  
  
};
#endif // FILTERTRUETRACKS_H
