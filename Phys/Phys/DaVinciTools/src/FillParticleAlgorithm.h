// $Id: FillParticleAlgorithm.h,v 1.1 2002-03-27 20:34:59 gcorti Exp $
#ifndef LIB_FILLPARTICLEALGORITHM_H 
#define LIB_FILLPARTICLEALGORITHM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IParticlePropertySvc;

/** @class FillParticleAlgorithm FillParticleAlgorithm.h Lib/FillParticleAlgorithm.h
 *  Fill class Particles with information from AxParticleCandidate
 *
 *  @author Sandra Amato
 *  @date   25/02/2002
 */
class FillParticleAlgorithm : public Algorithm {
public:
  /// Standard constructor
  FillParticleAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FillParticleAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  // Particle Property Service
  IParticlePropertySvc* m_ppSvc;

};
#endif // LIB_FILLPARTICLEALGORITHM_H
