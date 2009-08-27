// $Id: TrackCheckerBase.h,v 1.9 2009-08-27 07:54:36 smenzeme Exp $
#ifndef TRACKCHECKERBASE_H
#define TRACKCHECKERBASE_H 1
 
// linkers
#include "Linker/LinkerTool.h"

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

#include <string>

// interfaces
#include "MCInterfaces/IMCReconstructible.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/ITrajPoca.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "MCInterfaces/IIdealStateCreator.h"
#include "MCInterfaces/IVisPrimVertTool.h"
#include "MCInterfaces/ITrackGhostClassification.h"


#include "Event/Track.h"
#include "Event/MCParticle.h"

/** @class TrackCheckerBase TrackCheckerBase.h "TrackCheckers/TrackCheckerBase"
 * 
 *  Base class for track monitoring: essentially a 'box' of common tools

 *  @author M. Needham.
 *  @date   7-5-2007
 */                 
                                                           
class TrackCheckerBase : public GaudiHistoAlg {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackCheckerBase( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackCheckerBase();

  /** Algorithm initialization */
  virtual StatusCode initialize();

  /** Algorithm execute */
  virtual StatusCode execute();

  /** Algorithm finalize */
  virtual StatusCode finalize();

  /** Input track container location 
  *  @return location
  */
  const std::string& inputContainer() const; 


 public:
   
  /** Get a pointer to Magnetic field service
  *  @return field service
  */
  IMagneticFieldSvc* fieldSvc() const;
 
  /** Get a pointer to the poca tool
  *  @return poca tool
  */
  ITrajPoca* pocaTool() const;

  /** Get a pointer to the track selection tool
  *  @return field service
  */
  IMCReconstructible* selector() const;

  /** Get a pointer to the idealStateCreator
  *  @return IdealStateCreator
  */
  IIdealStateCreator* idealStateCreator() const;

  /** Get a pointer to the track extrapolator
  *  @return extrapolator
  */
  ITrackExtrapolator* extrapolator() const;

  
  typedef LinkerTool<LHCb::Track, LHCb::MCParticle> AsctTool;
  typedef AsctTool::InverseType InverseTable;
  typedef InverseTable::Range InverseRange;
  typedef InverseTable::iterator inverse_iterator;

  typedef AsctTool::DirectType DirectTable;
  typedef DirectTable::Range DirectRange;
  typedef DirectTable::iterator direct_iterator;

  /** Direct association table track to particle */
  DirectTable* directTable() const;

  /** Inverse association table particle to track */
  InverseTable* inverseTable() const;

  /** initialize per event */
  StatusCode initializeEvent();

  /** small struct for link info */
  struct LinkInfo{
    const LHCb::Track* track;
    unsigned int clone;
    double purity;
  };

  /** link to truth 
  * @param  aTrack track
  * @return linked particle 
  */
  const LHCb::MCParticle* mcTruth(const LHCb::Track* aTrack) const;

  /** What tracks were reconstructed from this particle
  * @param particle
  * @return track 
  */
  LinkInfo reconstructed(const LHCb::MCParticle* particle) const;
  LinkInfo reconstructedInRefContainer(const LHCb::MCParticle* particle) const;


  /** What tracks were reconstructed from this particle
  * @param particle
  * @return rec tracks 
  */
  TrackCheckerBase::InverseRange reconstructedTracks(const LHCb::MCParticle* particle) const;

  /** Selected as type
  *
  * @return bool
  */
  bool selected(const LHCb::MCParticle* particle) const;

  /** Whether to split by algorithm
  *  @return splitByAlgorithm true or false
  */ 
  bool splitByAlgorithm() const;

  /** Whether to split by algorithm
  *  @return splitByType true or false
  */ 
  bool splitByType() const { return m_splitByType ; }

  /** To avoid hard coding...
  *  @return string all
  */ 
  const std::string& all() const;

  /** Pointer to the visible primary vertex tool
  *  @return IVisPrimVertTool
  */
  IVisPrimVertTool* visPrimVertTool() const;

  /** Pointer to ghost classification tool
  *  @return ITrackGhostClassification
  */
  ITrackGhostClassification* ghostClassification() const;

  /** Is a b child ? ie has b quark somewhere in history
  * @param  mcPart MC particle
  * @return bool true/false 
  */ 
  bool bAncestor(const LHCb::MCParticle* mcPart) const;

  /** Is a lambda/ks 
  * @param  mcPart MC particle
  * @return bool true/false 
  */
  bool ksLambdaAncestor(const LHCb::MCParticle* mcPart) const;


  /** are all stable daughters of this particle reconstructible?
   * @param  mcPart MC particle
   * @return bool true/false
   */

  bool allDaughtersReconstructible(const LHCb::MCParticle* mcPart) const;

  bool bAncestorWithReconstructibleDaughters(const LHCb::MCParticle* mcPart) const;
  
  const std::string& tracksInContainer() { return m_tracksInContainer ; }


 protected:

  std::string m_tracksRefContainer;   ///< Reference Tracks container location
 
 private:

  std::string m_extrapolatorName; 
  std::string m_selectorName;
  std::string m_tracksInContainer;    ///< Input Tracks container location 
  std::string m_linkerInTable;
 
  ITrajPoca*         m_poca;          ///<  Pointer to the ITrajPoca interface  
  IMagneticFieldSvc* m_pIMF;          ///<  Pointer to the magn. field service

  std::string m_selectionCriteria;
  IMCReconstructible::RecCategory m_recCat; ///<  Pointer to selector 
  IMCReconstructible* m_selector;           ///<  Pointer to selector   

  ITrackExtrapolator* m_extrapolator; ///<  Pointer to extrapolator
  IIdealStateCreator* m_stateCreator; ///<  IdealStateCreator
  IVisPrimVertTool* m_visPrimVertTool; ///< Visible primary vertices..

  std::string m_ghostToolName;
  ITrackGhostClassification* m_ghostClassification;  ///< Pointer to ghost tool 

  AsctTool m_associator;
  DirectTable* m_directTable;
  InverseTable* m_inverseTable;

  AsctTool m_associator_refContainer;
  DirectTable* m_directTable_refContainer;
  InverseTable* m_inverseTable_refContainer;


  bool m_splitByAlgorithm;
  bool m_splitByType;
  std::string m_all;                                                                       

};

inline IMagneticFieldSvc* TrackCheckerBase::fieldSvc() const{
  return m_pIMF;
}

inline ITrajPoca* TrackCheckerBase::pocaTool() const{
  return m_poca;
}

inline IMCReconstructible* TrackCheckerBase::selector() const{
  return m_selector;
}

inline bool TrackCheckerBase::selected(const LHCb::MCParticle* particle) const{
  return selector()->isReconstructibleAs(m_recCat, particle);
}

inline ITrackExtrapolator* TrackCheckerBase::extrapolator() const{
  return m_extrapolator;
}

inline IIdealStateCreator* TrackCheckerBase::idealStateCreator() const{
  return m_stateCreator;
}

inline IVisPrimVertTool* TrackCheckerBase::visPrimVertTool() const{
  return m_visPrimVertTool;
}

inline const std::string& TrackCheckerBase::inputContainer() const{
  return m_tracksInContainer;
}

inline TrackCheckerBase::DirectTable* TrackCheckerBase::directTable() const{
  return m_directTable;
} 

inline TrackCheckerBase::InverseTable* TrackCheckerBase::inverseTable() const{
  return m_inverseTable;
} 

inline bool TrackCheckerBase::splitByAlgorithm() const{
  return m_splitByAlgorithm;
}

inline const std::string& TrackCheckerBase::all() const{
  return m_all;
}

inline ITrackGhostClassification* TrackCheckerBase::ghostClassification() const {
  return m_ghostClassification;
}


#endif // TRACKCHECKERBASE_H
