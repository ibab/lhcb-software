// $Id: DecayTreeTupleBase.h,v 1.1 2009-01-20 10:00:44 pkoppenb Exp $
#ifndef JBOREL_DECAYTREETUPLEBASE_H
#define JBOREL_DECAYTREETUPLEBASE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

class IDecayFinder;
class TupleToolDecay;

class IParticleTupleTool;
class IEventTupleTool;
namespace Decays{  
  class OnePart;
}

/** @class DecayTreeTupleBase DecayTreeTupleBase.h jborel/DecayTreeTupleBase.h
 *
 * Base class for algorithms providing a DecayTreeTuple
 *
 * \sa DecayTreeTuple, MCDecayTreeTuple
 *
 * \author Patrick Koppenburg based on Jérémie Borel's DecayTreeTuple
 * \date 2009-01-20
 */
class DecayTreeTupleBase : public DVAlgorithm  {

 public:
  /// Standard constructor
  DecayTreeTupleBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DecayTreeTupleBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
 protected:
  //! Trigger all the fill procedures
  StatusCode fillTuple( Tuples::Tuple&, const LHCb::Particle::ConstVector& );
  //! Call the fill methode of all the particles
  StatusCode fillParticles( Tuples::Tuple&, const LHCb::Particle::ConstVector& );
  //! Call the fill methode which does not take a particle as argument
  StatusCode fillEventRelatedVariables( Tuples::Tuple& );


 protected:
  //! Retrieve from the desktop all the top level particles that match
  //! the decay descriptor
  bool getDecayMatches( const LHCb::Particle::ConstVector& src
			, LHCb::Particle::ConstVector& target );

  //! Check if ready to fill or trigger the initialization
  bool sizeCheckOrInit( const LHCb::Particle::ConstVector& );
  void matchSubDecays( const LHCb::Particle::ConstVector& );

  std::vector<std::string> getEventTools() const;
  std::vector<std::string> getParticleTools() const;

  bool initializeDecays();
  void initializeStufferTools();
  void initializeOnePartsStufferTools( Decays::OnePart*, const TupleToolDecay* );

  std::string getBranchName( const LHCb::Particle* p );

  int getOffset( const LHCb::Particle* p
		 , const LHCb::Particle::ConstVector& v
		 , bool secure=true );

  std::map< std::string, std::string > m_decayMap;
  std::vector<TupleToolDecay*> m_decays;
  std::string m_headDecay;

  std::string m_tupleName; //, m_eventColName;
  bool m_useLabName;
  bool m_tupleNameAsToolName;

  std::vector<std::string> m_toolList;

  std::vector<Decays::OnePart*> m_parts;

  std::vector< IParticleTupleTool* > m_pTools;
  std::vector< IEventTupleTool* > m_eTools;

  IDecayFinder* m_dkFinder;

  unsigned int m_nSuccessEvent, m_nSuccessCandidate, m_nFailedEvent, m_nFailedCandidate;

};



#endif // JBOREL_DECAYTREETUPLEBASE_H
