// $Id: TupleToolDecay.h,v 1.5 2009-02-03 16:14:15 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLDECAY_H
#define JBOREL_TUPLETOOLDECAY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
//#include "Event/Particle.h"
//#include "Kernel/IParticleTupleTool.h"
#include "DecayTreeTupleBase/ITupleToolDecay.h"

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
class TupleToolDecay : public GaudiTool,
                       virtual public ITupleToolDecay
{

public:

  /// Standard constructor
  TupleToolDecay( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~TupleToolDecay( ); ///< Destructor

  virtual StatusCode initializeDecay( const std::string&, bool );

public:

  std::string decay() const;

  std::string getInfo() const;
  void printInfo() const;

  IDecayFinder* decayFinder() const;
  IMCDecayFinder* mcDecayFinder() const;

  bool hasMatched() const;
  void hasMatched( bool state );

  bool inheritTools() const;

  const std::string& getName() const;
  const std::vector<std::string>& getStuffers() const;

private:

  bool isMC() const { return m_isMC; }

private:

  bool m_hasMatched;
  bool m_inheritTools;

  std::string m_myName;

  std::vector<std::string> m_stufferList;

  IDecayFinder* m_dkFinder;
  IMCDecayFinder* m_mcdkFinder;
  bool m_isMC ;

};

#endif // JBOREL_TUPLETOOLDECAY_H
