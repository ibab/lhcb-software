#ifndef _TrackGhostClassificationBase_H
#define _TrackGhostClassificationBase_H

#include <vector>

#include "GaudiAlg/GaudiTool.h"
#include "MCInterfaces/ITrackGhostClassification.h"
#include "MCInterfaces/ILHCbIDsToMCParticles.h"

#include "Kernel/LHCbID.h"
#include "Event/GhostTrackInfo.h"

namespace LHCb{
  class Track;
}

class TrackGhostClassificationBase: public GaudiTool, virtual public ITrackGhostClassification {

public:

  /// constructer
  TrackGhostClassificationBase(const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

  /** destructer */
  virtual ~TrackGhostClassificationBase();


  /** initialize */
  StatusCode initialize();

 
  /**
  * Information on what a ghost track is....
  *  @param aTrack to link
  *  @param tinfo link info
  *  @return StatusCode whether classification was successfull
  */
  StatusCode info(const LHCb::Track& aTrack, LHCb::GhostTrackInfo& tinfo) const;

  /**
  *  Information on a list of LHCbIDs
  *  @param start first iterator
  *  @param stop  last  iterator
  *  @param tinfo link info
  *  @return StatusCode whether classification was successfull
  */
  virtual StatusCode info(LHCbIDs::const_iterator& start, 
                    LHCbIDs::const_iterator& stop, LHCb::GhostTrackInfo& tinfo) const;

  /**
  *  Check this is a ghost .
  *  @param aTrack to link
  *  @return bool true if a ghost
  */
  virtual bool isGhost(const LHCb::Track& aTrack) const;

  /**
   *  Check this is a ghost .
   *  @param aTrack to link
   *  @return bool true if a ghost
   */
  virtual bool isGhost(LHCbIDs::const_iterator& start, 
                       LHCbIDs::const_iterator& stop) const;

 protected:

  typedef std::vector<LHCb::LHCbID> LHCbIDs;

  /** generic information */
  StatusCode generic(const LHCb::Track& aTrack, LHCb::GhostTrackInfo& tinfo) const;

  /** generic information */
  StatusCode generic(LHCbIDs::const_iterator& start, 
               LHCbIDs::const_iterator& stop, LHCb::GhostTrackInfo& tinfo) const;

  /** specific information */
  StatusCode specific(const LHCb::Track& aTrack, LHCb::GhostTrackInfo& tinfo) const;

  /** specific information */
  virtual StatusCode specific(LHCbIDs::const_iterator& start, 
                LHCbIDs::const_iterator& stop, LHCb::GhostTrackInfo& tinfo) const;


  /** pointer to linker tool */
  ILHCbIDsToMCParticles* linkTool() const;

  /** purity cut */
  double purityCut() const;

  /** spillover test */
  bool spillover(const LHCb::GhostTrackInfo::LinkPair& aPair) const;

  /** check if real */ 
  bool isReal(const LHCb::GhostTrackInfo::LinkPair& bestPair) const;

  /** check if ghost*/
  bool isGhost(const ILHCbIDsToMCParticles::LinkMap& lmap) const;

  /** get the second best link */
  LHCb::GhostTrackInfo::LinkPair secondBest(const LHCb::GhostTrackInfo& tinfo,
                                            const LHCb::MCParticle* bestPart) const;

  /** check if decay in flight pi -> mu, K -> mu */
  bool decayInFlight(LHCb::GhostTrackInfo::LinkPair& p1,
		     LHCb::GhostTrackInfo::LinkPair& p2 ) const;
      
  /** check if e+/e- pair */
  bool conversion(LHCb::GhostTrackInfo::LinkPair& p1,
		  LHCb::GhostTrackInfo::LinkPair& p2 ) const;

  /** hadronic interaction */
  bool interaction(const LHCb::GhostTrackInfo& tinfo ) const;

  /** electromagnetic shower */
  bool emShower(const LHCb::GhostTrackInfo& tinfo ) const;

  /** K+/K- from Phi */ 
  bool fromPhi(LHCb::GhostTrackInfo::LinkPair& p1,
	       LHCb::GhostTrackInfo::LinkPair& p2) const;

  /** get the best pair from the GhostTrackInfo */  
  LHCb::GhostTrackInfo::LinkPair bestPair(const LHCb::GhostTrackInfo& tinfo) const;

  /** get the best pair from the Linker Map */  
  LHCb::GhostTrackInfo::LinkPair bestPair(const LHCb::GhostTrackInfo::LinkMap& lMap) const;

  /** get the best pair from the ids */  
  LHCb::GhostTrackInfo::LinkPair bestPair(const LHCbIDs& ids) const;
 
  /** purity is more than m_purityCut*/
  bool isMatched(const LHCb::GhostTrackInfo::LinkPair& aPair) const;

 protected:
  double m_purityCut;

 private:
  ILHCbIDsToMCParticles* m_linkTool;
 
};

inline ILHCbIDsToMCParticles* TrackGhostClassificationBase::linkTool() const{
  return m_linkTool;
}

inline double TrackGhostClassificationBase::purityCut() const{
  return m_purityCut;
}

#endif
