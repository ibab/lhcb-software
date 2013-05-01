#ifndef _LHCbIDsToMCParticles_H
#define _LHCbIDsToMCParticles_H

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"

// from LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedTo.h"


#include "MCInterfaces/ILHCbIDsToMCParticles.h"

/** @class LHCbIDsToMCParticles LHCbIDsToMCParticles.h 
*
*  Link ids to MCParticles
*
*  @author M.Needham
*  @date   31/04/2006
*/

namespace LHCb{
  class STCluster;
  class VeloCluster;
  class MuonCoord;
}

class LHCbIDsToMCParticles: public GaudiTool, virtual public ILHCbIDsToMCParticles,
                            virtual public IIncidentListener {

public:

  
   /// constructer
   LHCbIDsToMCParticles(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

   /** destructer */
   virtual ~LHCbIDsToMCParticles();


   /** initialize */
   StatusCode initialize();

  /**
    Trivial link from list of IDs to all particles contributing
    @param start  iterator to first id
    @param stop   iterator to last id
    @param output vector by reference
    @return StatusCode
  */
  virtual StatusCode link(LHCbIDs::const_iterator& start, 
                          LHCbIDs::const_iterator& stop, LinkMap& output) const;


  /**
    Trivial link from list of ALL ids in track to particles contributing
    @param aTrack track
    @param output vector by reference
    @return StatusCode
  */
  virtual StatusCode link(const LHCb::Track& aTrack, LinkMap& output) const;



  /**
    Trivial link from single id to particles contributing
    @param id
    @param output vector by reference
    @return StatusCode
  */
  StatusCode link(const LHCb::LHCbID& id, LinkMap& output) const;


  /** Implement the handle method for the Incident service.
  *  This is used to inform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  void handle( const Incident& incident );

private:

  typedef LinkedTo<LHCb::MCParticle,LHCb::STCluster> STLinks;
  typedef LinkedTo<LHCb::MCParticle> OTLinks;
  typedef LinkedTo<LHCb::MCParticle,LHCb::VeloCluster> VeloLinks;
  typedef LinkedTo<LHCb::MCParticle,LHCb::MuonCoord> MuonLinks;

  template<typename ID, typename LINKER>
  void linkToDetTruth(const ID& id, LINKER& aLinker, LinkMap& output ) const;

  StatusCode linkIT(const LHCb::LHCbID& id, LinkMap& output) const;
  StatusCode linkTT(const LHCb::LHCbID& id, LinkMap& output) const;
  StatusCode linkOT(const LHCb::LHCbID& id, LinkMap& output) const;
  StatusCode linkVelo(const LHCb::LHCbID& id, LinkMap& output) const;
  StatusCode linkMuon(const LHCb::LHCbID& id, LinkMap& output) const;

  mutable STLinks m_itLinks;
  mutable STLinks m_ttLinks;
  mutable OTLinks m_otLinks;
  mutable VeloLinks m_veloLinks;
  mutable MuonLinks m_muonLinks;

  mutable bool m_configuredOT;
  mutable bool m_configuredIT;
  mutable bool m_configuredTT;
  mutable bool m_configuredVelo;
  mutable bool m_configuredMuon;
 
};


#include "Event/MCParticle.h"
template<typename ID, typename LINKER>
void LHCbIDsToMCParticles::linkToDetTruth(const ID& id, LINKER& aLinker, LinkMap& output ) const{

  LHCb::MCParticle* aParticle = aLinker.first(id);
  if (0 != aParticle) {
    while ( 0 != aParticle ) {
      output[aParticle] += 1; 
      aParticle = aLinker.next();
    }
  }
  else {
    output[0] += 1;
  }
}
#endif


  
