// $Id: MCParticleLinkerMaker.h,v 1.1 2006-06-22 12:38:48 jpalac Exp $
#ifndef MCPARTICLELINKERMAKER_H 
#define MCPARTICLELINKERMAKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciTupleAlgorithm.h"

/** @class MCParticleLinkerMaker MCParticleLinkerMaker.h
 *
 * This Algorithm creates a Table to link Particles created with 
 * MCPArticleMaker tool to the corresponding MCParticle
 *
 *  @author Stefania Vecchi & Gabriele Balbi
 *  @date   2006-06-09
 */
class MCParticleLinkerMaker : public DaVinciTupleAlgorithm
{

public: 

  /// Standard constructor
  MCParticleLinkerMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCParticleLinkerMaker( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  std::string m_inputMC;
  std::string m_inputParticle;
  bool m_writeTuple;
  
};

#endif // MCPARTICLELINKERMAKER_H
