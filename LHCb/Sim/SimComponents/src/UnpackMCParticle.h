// $Id: UnpackMCParticle.h,v 1.1 2006-02-20 08:21:40 cattanem Exp $
#ifndef UNPACKMCPARTICLE_H 
#define UNPACKMCPARTICLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class UnpackMCParticle UnpackMCParticle.h
 *  Unpack the MCParticles
 *
 *  @author Olivier Callot
 *  @date   2005-03-18
 */
class UnpackMCParticle : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackMCParticle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackMCParticle( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // UNPACKMCPARTICLE_H
