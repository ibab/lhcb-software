// $Id: PackMCParticle.h,v 1.1 2006-02-20 08:21:40 cattanem Exp $
#ifndef PACKMCPARTICLE_H 
#define PACKMCPARTICLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackMCParticle PackMCParticle.h
 *  Pack the MCParticles
 *
 *  @author Olivier Callot
 *  @date   2005-03-18
 */
class PackMCParticle : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackMCParticle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackMCParticle( );    ///< Destructor
  virtual StatusCode execute();  ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // PACKMCPARTICLE_H
