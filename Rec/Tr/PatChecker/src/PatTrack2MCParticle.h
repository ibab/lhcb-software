#ifndef PATTRACK2MCPARTICLE_H 
#define PATTRACK2MCPARTICLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedTo.h"

// from PhysEvent
#include "Event/MCParticle.h"
#include "Event/Track.h"


/** @class PatTrack2MCParticle PatTrack2MCParticle.h
 *  Associate MCParticles to tracks produced by the pattern.
 *  Convert to TES if needed...
 *
 *  @author Olivier Callot
 *  @date   2005-06-01
 *  @update for A-Team framework 2007-08-20 SHM 
 */

  class PatTrack2MCParticle : public GaudiAlgorithm {
  public: 
    /// Standard constructor
    PatTrack2MCParticle( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~PatTrack2MCParticle( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    
  protected:
    void associateTrack ( const LHCb::Track* tr,
			  LinkedTo<LHCb::MCParticle>& lhcbIdLink,
			  LinkerWithKey<LHCb::MCParticle, LHCb::Track>& trackLink
			  );
  private:
    std::string m_lhcbLinks;
    double m_fractionOK;
    std::vector<std::string> m_locations;
  };

#endif // PATTRACK2MCPARTICLE_H
