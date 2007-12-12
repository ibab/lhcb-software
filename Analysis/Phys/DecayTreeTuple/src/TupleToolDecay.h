// $Id: TupleToolDecay.h,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLDECAY_H 
#define JBOREL_TUPLETOOLDECAY_H 1

class IDecayFinder;
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Event/Particle.h"

class IParticleTupleTool;

static const InterfaceID IID_TupleToolDecay ( "TupleToolDecay", 1, 0 );

/** @class TupleToolDecay TupleToolDecay.h jborel/TupleToolDecay.h
 * 
 * \brief Not really meant to be used outside DecayTreeTuple
 *
 * look at this later doc for more information.
 *
 * It has two properties:
 * 
 * - \b ToolList, a vector of string stating which tool are associated
 to this part of the decay
 *
 * - \b InheritTools, boolean, stating whether the tools given in the
 parent DecayTreeTuple should be inherit to this part of the decay.
 *
 * \sa DecayTreeTuple
 *
 *  @author Jeremie Borel
 *  @date   2007-11-02
 */
class TupleToolDecay : public GaudiTool {
public: 
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_TupleToolDecay; }

  /// Standard constructor
  TupleToolDecay( const std::string& type, 
		  const std::string& name,
		  const IInterface* parent);

  virtual ~TupleToolDecay( ); ///< Destructor

  StatusCode initialize( const std::string& );

  void printInfo() const ;
  std::string decay() const;
  std::string getInfo() const;

  void decayMembers ( const LHCb::Particle *head
		      , LHCb::Particle::ConstVector &members);

  
  bool hasMatched() const { return m_hasMatched; };
  void hasMatched( bool state ) { m_hasMatched=state; };
  
  bool inheritTools() const { return m_inheritTools; };

  std::string getName() const { return m_myName; };
  const std::vector<std::string>& getStuffers() const { return m_stufferList; };
  
 private:
  bool m_hasMatched;
  bool m_inheritTools;

  std::string m_myName;

  std::vector<std::string> m_stufferList;

  IDecayFinder* m_dkFinder;
};
#endif // JBOREL_TUPLETOOLDECAY_H
