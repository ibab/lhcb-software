// $Id: DecayTreeTupleBase.h,v 1.3 2009-01-20 17:53:49 pkoppenb Exp $
#ifndef JBOREL_DECAYTREETUPLEBASE_H
#define JBOREL_DECAYTREETUPLEBASE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

class IDecayFinder;
class TupleToolDecay;

class IMCParticleTupleTool;
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
  //! Call the fill methode which does not take a particle as argument
  StatusCode fillEventRelatedVariables( Tuples::Tuple& );


 protected:
  //! Retrieve from the desktop all the top level particles that match
  //! the decay descriptor
  bool getDecayMatches( const LHCb::Particle::ConstVector& src
			, LHCb::Particle::ConstVector& target );

  /// Call successively all OnePart's fill method 
  bool fillOnePart( Decays::OnePart*, Tuples::Tuple&, const LHCb::Particle* mother, const LHCb::Particle* );
  std::vector<std::string> getEventTools() const;

  bool initializeDecays();
  void initializeStufferTools(std::vector< IParticleTupleTool* >& pTools);
  void initializeOnePartsStufferTools( Decays::OnePart*, const TupleToolDecay*,std::vector< IParticleTupleTool* >& pTools  );

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

  std::vector< IMCParticleTupleTool* > m_mTools; // TMP
  std::vector< IEventTupleTool* > m_eTools;

  IDecayFinder* m_dkFinder;
  std::vector<std::string> getParticleTools(const std::vector< IParticleTupleTool* > pTools) const;

};



#endif // JBOREL_DECAYTREETUPLEBASE_H
