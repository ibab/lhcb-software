// $Id: L0CaloToMCParticleAsct.h,v 1.2 2003-06-23 11:52:26 ocallot Exp $
#ifndef L0CALOTOMCPARTICLEASCT_H 
#define L0CALOTOMCPARTICLEASCT_H 1

// Include files
// from STL
#include <string>

// LHCbKernel
#include "Relations/IAssociator.h" 
#include "Relations/IRelation.h" 

// from Gaudi
#include "GaudiKernel/Algorithm.h"

class MCParticle;
class L0CaloCandidate;

/** @class L0CaloToMCParticleAsct L0CaloToMCParticleAsct.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2002-07-01
 */
class L0CaloToMCParticleAsct : public Algorithm {
  // friend factory for instantiation 
  friend class AlgFactory<L0CaloToMCParticleAsct>; 
public:
  /// Standard constructor
  L0CaloToMCParticleAsct( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0CaloToMCParticleAsct( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputContainer;
  std::string m_outputTable;
  double      m_minimalFraction;
};
#endif // L0CALOTOMCPARTICLEASCT_H
