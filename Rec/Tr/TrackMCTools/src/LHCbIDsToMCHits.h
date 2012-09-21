#ifndef _LHCbIDsToMCHits_H
#define _LHCbIDsToMCHits_H

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"

// from LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedTo.h"


#include "MCInterfaces/ILHCbIDsToMCHits.h"

/** @class LHCbIDsToMCHits LHCbIDsToMCHits.h
*
*  Link ids to MCHits
*
*  @author M.Needham
*  @date   31/04/2006
*/

namespace LHCb{
  class STCluster;
  class VeloCluster;
  class VPCluster;
  class MuonCoord;
}

class LHCbIDsToMCHits: public GaudiTool, virtual public ILHCbIDsToMCHits,
                            virtual public IIncidentListener {

public:

  
   /// constructer
   LHCbIDsToMCHits(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

   /** destructer */
   virtual ~LHCbIDsToMCHits();


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

  typedef LinkedTo<LHCb::MCHit,LHCb::STCluster> STLinks;
  typedef LinkedTo<LHCb::MCHit> OTLinks;
  typedef LinkedTo<LHCb::MCHit,LHCb::VeloCluster> VeloLinks;
  typedef LinkedTo<LHCb::MCHit,LHCb::VPCluster> VPLinks;
  typedef LinkedTo<LHCb::MCHit,LHCb::MuonCoord> MuonLinks;

  template<typename ID, typename LINKER>
  void linkToDetTruth(const ID& id, LINKER& aLinker, LinkMap& output ) const;

  void linkIT(const LHCb::LHCbID& id, LinkMap& output) const;
  void linkTT(const LHCb::LHCbID& id, LinkMap& output) const;
  void linkOT(const LHCb::LHCbID& id, LinkMap& output) const;
  void linkVelo(const LHCb::LHCbID& id, LinkMap& output) const;
  void linkVP(const LHCb::LHCbID& id, LinkMap& output) const;
  void linkMuon(const LHCb::LHCbID& id, LinkMap& output) const;

  mutable STLinks m_itLinks;
  mutable STLinks m_ttLinks;
  mutable OTLinks m_otLinks;
  mutable VeloLinks m_veloLinks;
  mutable VPLinks m_vPLinks;
  mutable MuonLinks m_muonLinks;

  mutable bool m_configuredOT;
  mutable bool m_configuredIT;
  mutable bool m_configuredTT;
  mutable bool m_configuredVelo;
  mutable bool m_configuredVP;
  mutable bool m_configuredMuon;

   
  std::string m_endString;

};


#include "Event/MCHit.h"
template<typename ID, typename LINKER>
void LHCbIDsToMCHits::linkToDetTruth(const ID& id, LINKER& aLinker, LinkMap& output ) const{

  LHCb::MCHit* aHit = aLinker.first(id);
  if (0 != aHit) {
    while ( 0 != aHit ) {
      output[aHit] += 1; 
      aHit = aLinker.next();
    }
  }
  else {
    output[0] += 1;
  }
}
#endif


  
