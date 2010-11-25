// $Id: $
#ifndef MEASUREIPRESOLUTION_H 
#define MEASUREIPRESOLUTION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/MCParticle.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

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
  IParticlePropertySvc* m_ppSvc;

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
