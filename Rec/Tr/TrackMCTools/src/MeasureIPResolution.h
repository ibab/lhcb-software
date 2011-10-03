#ifndef MEASUREIPRESOLUTION_H 
#define MEASUREIPRESOLUTION_H 1

// Include files
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

namespace LHCb {
  class IParticlePropertySvc;
  class MCParticle;
}

/** @class MeasureIPResolution MeasureIPResolution.h
 *  Measure the IP resolution of Velo tracks
 *
 *  @author Olivier Callot
 *  @date   2010-10-05
 */
class MeasureIPResolution : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MeasureIPResolution( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MeasureIPResolution( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void printMCParticle ( const LHCb::MCParticle* part );
  
private:
  std::string m_containerName;
  mutable LHCb::IParticlePropertySvc* m_ppSvc; ///< Pointer to particle property service

  int m_nTracks;
  double m_averX;
  double m_averY;
  int m_nbInCore;
  double m_sumRInCore;
  double m_sumR2InCore;
  double m_sumIPS;
  double m_sumIPS2;
};
#endif // MEASUREIPRESOLUTION_H
