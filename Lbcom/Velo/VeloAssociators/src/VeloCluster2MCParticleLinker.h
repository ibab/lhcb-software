// $Id: VeloCluster2MCParticleLinker.h,v 1.1 2006-03-17 16:09:37 szumlat Exp $
#ifndef VELOCLUSTER2MCPARTICLELINKER_H 
#define VELOCLUSTER2MCPARTICLELINKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class VeloCluster2MCParticleLinker VeloCluster2MCParticleLinker.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-03-15
 */

class VeloCluster2MCParticleLinker : public GaudiAlgorithm {
public: 
  /// Standard constructor
  VeloCluster2MCParticleLinker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloCluster2MCParticleLinker( ); ///< Destructor
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

  std::string asctName();
  
private:

  std::string m_asctName;
  std::string m_inputClusters;
  std::string m_inputParts;

};
//
inline std::string VeloCluster2MCParticleLinker::asctName()
{
  return ( m_asctName );
}
#endif // VELOCLUSTER2MCPARTICLELINKER_H
